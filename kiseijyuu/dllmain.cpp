// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include<windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        MessageBox(NULL, L"DLL ���سɹ�", L"��ʾ��", MB_OK);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH: {
        MessageBox(NULL, L"DLL ж�سɹ�", L"��ʾ��", MB_OK);
        break;
    }
    }
    return TRUE;
}

