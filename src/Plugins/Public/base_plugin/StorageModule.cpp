/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 11 июля 2026 14:02:51
 * Version: 1.0.548
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