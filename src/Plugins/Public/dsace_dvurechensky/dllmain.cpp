/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 16 мая 2026 10:48:31
 * Version: 1.0.493
 */

// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <plugin.h>

// Определение структуры данных для DSACE_CHANGE_INFOCARD
struct InfoCardData {
    // Здесь должны быть поля структуры InfoCardData
};

// Прототип функции обратного вызова для приема сообщения
void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

