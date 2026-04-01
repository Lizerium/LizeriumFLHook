/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 01 апреля 2026 13:06:06
 * Version: 1.0.13
 */

// Player Control plugin for FLHookPlugin
// Jan 2010 by Cannon
//
// This is free software; you can redistribute it and/or modify it as
// you wish without restriction. If you do then I would appreciate
// being notified and/or mentioned somewhere.

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <float.h>
#include <FLHook.h>
#include <plugin.h>
#include <math.h>
#include <list>
#include <set>

#include "Main.h"
#include "ZoneUtilities.h"

/** Сопоставление идентификатора системы с информацией о системе */
map<uint, SYSTEMINFO> mapSystems;

/** Сопоставление идентификаторов системы с зонами */
multimap<uint, ZONE> zones;

/** Сопоставление идентификатора системы с информацией о начальных точках */
multimap<uint, JUMPPOINT> jumpPoints;

/** Умножаем mat1 на mat2 и получаем результат */
static TransformMatrix MultiplyMatrix(TransformMatrix &mat1, TransformMatrix &mat2)
{
	TransformMatrix result = { 0 };
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				result.d[i][j] += mat1.d[i][k] * mat2.d[k][j];
	return result;
}

/** Настройка матрицы преобразований с помощью вектора p и вращения r */
static TransformMatrix SetupTransform(Vector &p, Vector &r)
{
	// Перевести градусы в радианы
#define PI (3.14159265358f)
	float ax = r.x * (PI / 180);
	float ay = r.y * (PI / 180);
	float az = r.z * (PI / 180);

	// Исходная матрица
	TransformMatrix smat = { 0 };
	smat.d[0][0] = smat.d[1][1] = smat.d[2][2] = smat.d[3][3] = 1;

	// Матрица перевода
	TransformMatrix tmat;
	tmat.d[0][0] = 1;  tmat.d[0][1] = 0;  tmat.d[0][2] = 0;  tmat.d[0][3] = 0;
	tmat.d[1][0] = 0;  tmat.d[1][1] = 1;  tmat.d[1][2] = 0;  tmat.d[1][3] = 0;
	tmat.d[2][0] = 0;  tmat.d[2][1] = 0;  tmat.d[2][2] = 1;  tmat.d[2][3] = 0;
	tmat.d[3][0] = -p.x; tmat.d[3][1] = -p.y; tmat.d[3][2] = -p.z; tmat.d[3][3] = 1;

	// Матрица поворота по оси X
	TransformMatrix xmat;
	xmat.d[0][0] = 1;        xmat.d[0][1] = 0;        xmat.d[0][2] = 0;        xmat.d[0][3] = 0;
	xmat.d[1][0] = 0;        xmat.d[1][1] = cos(ax);  xmat.d[1][2] = sin(ax);  xmat.d[1][3] = 0;
	xmat.d[2][0] = 0;        xmat.d[2][1] = -sin(ax); xmat.d[2][2] = cos(ax);  xmat.d[2][3] = 0;
	xmat.d[3][0] = 0;        xmat.d[3][1] = 0;        xmat.d[3][2] = 0;        xmat.d[3][3] = 1;

	// Матрица вращения по оси Y
	TransformMatrix ymat;
	ymat.d[0][0] = cos(ay);  ymat.d[0][1] = 0;        ymat.d[0][2] = -sin(ay); ymat.d[0][3] = 0;
	ymat.d[1][0] = 0;        ymat.d[1][1] = 1;        ymat.d[1][2] = 0;        ymat.d[1][3] = 0;
	ymat.d[2][0] = sin(ay);  ymat.d[2][1] = 0;        ymat.d[2][2] = cos(ay);  ymat.d[2][3] = 0;
	ymat.d[3][0] = 0;        ymat.d[3][1] = 0;        ymat.d[3][2] = 0;        ymat.d[3][3] = 1;

	// Матрица вращения по оси Y
	TransformMatrix zmat;
	zmat.d[0][0] = cos(az);  zmat.d[0][1] = sin(az);  zmat.d[0][2] = 0;        zmat.d[0][3] = 0;
	zmat.d[1][0] = -sin(az); zmat.d[1][1] = cos(az);  zmat.d[1][2] = 0;        zmat.d[1][3] = 0;
	zmat.d[2][0] = 0;        zmat.d[2][1] = 0;        zmat.d[2][2] = 1;        zmat.d[2][3] = 0;
	zmat.d[3][0] = 0;        zmat.d[3][1] = 0;        zmat.d[3][2] = 0;        zmat.d[3][3] = 1;

	TransformMatrix tm;
	tm = MultiplyMatrix(smat, tmat);
	tm = MultiplyMatrix(tm, xmat);
	tm = MultiplyMatrix(tm, ymat);
	tm = MultiplyMatrix(tm, zmat);


	return tm;
}

/** Считайте информацию о размере/повороте и положении зоны 
	из указанного файла и рассчитайте матрицу трансформации зон, пригодных для добычи
  */
static void ReadSystemZones(const string &systemNick, const string &file)
{
	string path = "..\\data\\universe\\";
	path += file;

	INI_Reader ini;
	if (ini.open(path.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("zone"))
			{
				string zoneNick = "";
				Vector size = { 0,0,0 };
				Vector pos = { 0,0,0 };
				Vector rotation = { 0,0,0 };
				int damage = 0;
				bool encounter = false;
				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						zoneNick = ToLower(ini.get_value_string());
					}
					else if (ini.is_value("pos"))
					{
						pos.x = ini.get_value_float(0);
						pos.y = ini.get_value_float(1);
						pos.z = ini.get_value_float(2);
					}
					else if (ini.is_value("rotate"))
					{
						rotation.x = 0 - ini.get_value_float(0);
						rotation.y = 0 - ini.get_value_float(1);
						rotation.z = 0 - ini.get_value_float(2);
					}
					else if (ini.is_value("size"))
					{
						size.x = ini.get_value_float(0);
						size.y = ini.get_value_float(1);
						size.z = ini.get_value_float(2);
						if (size.y == 0 || size.z == 0)
						{
							size.y = size.x;
							size.z = size.x;
						}
					}
					else if (ini.is_value("damage"))
					{
						damage = ini.get_value_int(0);
					}
					else if (ini.is_value("encounter"))
					{
						encounter = true;
					}
				}

				ZONE lz;
				lz.sysNick = systemNick;
				lz.zoneNick = zoneNick;
				lz.systemId = CreateID(systemNick.c_str());
				lz.size = size;
				lz.pos = pos;
				lz.damage = damage;
				lz.encounter = encounter;
				lz.transform = SetupTransform(pos, rotation);
				zones.insert(zone_map_pair_t(lz.systemId, lz));
			}
			else if (ini.is_header("Object"))
			{
				string nickname = "";
				string jumpDestSysNick = "";
				bool bIsJump = false;
				bool bMissionObject = false;
				bool bDestructible = false;

				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
					{
						nickname = ToLower(ini.get_value_string());
					}
					else if (ini.is_value("goto"))
					{
						bIsJump = true;
						jumpDestSysNick = ini.get_value_string(0);
					}
				}

				if (bIsJump)
				{
					JUMPPOINT jp;
					jp.sysNick = systemNick;
					jp.jumpNick = nickname;
					jp.jumpDestSysNick = jumpDestSysNick;
					jp.System = CreateID(systemNick.c_str());
					jp.jumpID = CreateID(nickname.c_str());
					jp.jumpDestSysID = CreateID(jumpDestSysNick.c_str());
					jumpPoints.insert(jumppoint_map_pair_t(jp.System, jp));
				}
			}
		}
		ini.close();
	}
}

/** Ознакомьтесь со всеми системами в разделе universe ini */
void ZoneUtilities::ReadUniverse()
{
	zones.clear();

	// Прочитайте все системные ini-файлы еще раз, на этот раз извлекая информацию о размере/положении зоны для списка зон.
	INI_Reader ini;
	if (ini.open("..\\data\\universe\\universe.ini", false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("System"))
			{
				string systemNick = "";
				string file = "";
				float scale = 1.0f;
				while (ini.read_value())
				{
					if (ini.is_value("nickname"))
						systemNick = ini.get_value_string();
					if (ini.is_value("file"))
						file = ini.get_value_string();
					if (ini.is_value("NavMapScale"))
						scale = ini.get_value_float(0);
				}

				SYSTEMINFO sysInfo;
				sysInfo.sysNick = systemNick;
				sysInfo.systemId = CreateID(systemNick.c_str());
				sysInfo.scale = scale;
				mapSystems[sysInfo.systemId] = sysInfo;

				ReadSystemZones(systemNick, file);
			}
		}
		ini.close();
	}

	if (set_iPluginDebug > 2)
	{
		for (zone_map_iter_t i = zones.begin(); i != zones.end(); i++)
		{
			ConPrint(L"NOTICE: player control sys=%s zone=%s size=%0.0f,%0.0f,%0.0f\n",
				stows(i->second.sysNick).c_str(), stows(i->second.zoneNick).c_str(),
				i->second.size.x, i->second.size.y, i->second.size.z);
		}

		for (jumppoint_map_iter_t i = jumpPoints.begin(); i != jumpPoints.end(); i++)
		{
			ConPrint(L"NOTICE: player control sys=%s (%u) jump=%s \n",
				stows(i->second.sysNick).c_str(), i->second.System, stows(i->second.jumpNick).c_str());
		}
	}
}

/**
 Возвращает true, если местоположение корабля, указанное параметром position, находится в зоне добычи.
*/
bool ZoneUtilities::InZone(uint system, const Vector &pos, ZONE &rlz)
{
	// Для каждой зоны в системе проверьте, что pos находится внутри зоны.
	zone_map_iter_t start = zones.lower_bound(system);
	zone_map_iter_t end = zones.upper_bound(system);
	for (zone_map_iter_t i = start; i != end; i++)
	{
		const ZONE &lz = i->second;
		/** Преобразование точки pos в систему координат, заданную матрицей m */
		float x = pos.x*lz.transform.d[0][0] + pos.y*lz.transform.d[1][0]
			+ pos.z*lz.transform.d[2][0] + lz.transform.d[3][0];
		float y = pos.x*lz.transform.d[0][1] + pos.y*lz.transform.d[1][1]
			+ pos.z*lz.transform.d[2][1] + lz.transform.d[3][1];
		float z = pos.x*lz.transform.d[0][2] + pos.y*lz.transform.d[1][2]
			+ pos.z*lz.transform.d[2][2] + lz.transform.d[3][2];

		// Если r меньше/равно 1, то точка находится внутри эллипсоида.
		float result = sqrt(powf(x / lz.size.x, 2) + powf(y / lz.size.y, 2) + powf(z / lz.size.z, 2));
		if (result <= 1)
		{
			rlz = lz;
			return true;
		}
	}

	// Возвращаем итератор. 
	// Если iter - zones.end(), то точка не находится в зоне для добычи.
	return false;
}

/**
	Возвращает true, если местоположение корабля, указанное параметром position, находится в зоне смерти
*/
bool ZoneUtilities::InDeathZone(uint system, const Vector &pos, ZONE &rlz)
{
	// Для каждой зоны в системе проверьте, что pos находится внутри зоны.
	zone_map_iter_t start = zones.lower_bound(system);
	zone_map_iter_t end = zones.upper_bound(system);
	for (zone_map_iter_t i = start; i != end; i++) {
		const ZONE &lz = i->second;

		/** Преобразование точки pos в систему координат, заданную матрицей m */
		float x = pos.x*lz.transform.d[0][0] + pos.y*lz.transform.d[1][0]
			+ pos.z*lz.transform.d[2][0] + lz.transform.d[3][0];
		float y = pos.x*lz.transform.d[0][1] + pos.y*lz.transform.d[1][1]
			+ pos.z*lz.transform.d[2][1] + lz.transform.d[3][1];
		float z = pos.x*lz.transform.d[0][2] + pos.y*lz.transform.d[1][2]
			+ pos.z*lz.transform.d[2][2] + lz.transform.d[3][2];

		// Если r меньше/равно 1, то точка находится внутри эллипсоида.
		float result = sqrt(powf(x / lz.size.x, 2) + powf(y / lz.size.y, 2) + powf(z / lz.size.z, 2));
		if (result <= 1 && lz.damage > 250)
		{
			rlz = lz;
			return true;
		}
	}

	// Возвращаем итератор. Если iter имеет значение zones.end(), то точка не находится в зоне смерти.
	return false;
}

/**
	Возвращает указатель на объект system info для указанного идентификатора системы.
	Возвращаем 0, если идентификатор системы не существует.
*/
SYSTEMINFO *ZoneUtilities::GetSystemInfo(uint systemID)
{
	if (mapSystems.find(systemID) != mapSystems.end())
		return &mapSystems[systemID];
	return 0;
}

