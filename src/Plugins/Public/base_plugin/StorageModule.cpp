/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 31 марта 2026 10:53:40
 * Version: 1.0.12
 */

#include "Main.h"


StorageModule::StorageModule(PlayerBase *the_base)
	: Module(TYPE_STORAGE), base(the_base)
{
}

StorageModule::~StorageModule()
{
}

wstring StorageModule::GetInfo(bool xml)
{
	return L"Cargo Storage";
}

void StorageModule::LoadState(INI_Reader &ini)
{
	while (ini.read_value())
	{
	}
}

void StorageModule::SaveState(FILE *file)
{
	fprintf(file, "[StorageModule]\n");
}