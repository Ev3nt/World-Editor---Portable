#include <Windows.h>
#include <fstream>
#include <string>
#include <document.h>
#include <istreamwrapper.h>
#include <ostreamwrapper.h>
#include <prettywriter.h>

#include "Memory.h"

#define JSON_NAME "World Editor.json"

HMODULE hWE = GetModuleHandle(NULL);
HMODULE hStorm = GetModuleHandle("storm.dll");

rapidjson::Document doc;

LSTATUS CALLBACK BLZSRegQueryValueDword_Proxy(LPCSTR lpPath, LPCSTR lpValueName, LPDWORD lpcbData, LPBYTE lpData);

LSTATUS CALLBACK BLZSRegQueryValueString_Proxy(LPCSTR lpPath, LPCSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

LSTATUS CALLBACK BLZSRegSetValueDword_Proxy(LPCSTR lpPath, LPCSTR lpValueName, DWORD dwType, DWORD dwData);

LSTATUS CALLBACK BLZSRegSetValueString_Proxy(LPCSTR lpPath, LPCSTR lpValueName, DWORD dwType, LPCSTR lpData);

//--------------------------------------------------------------------------

BOOL WINAPI DllMain(HMODULE hModule, UINT ul_reason_for_call, LPVOID)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		if (!GetModuleHandle("game.dll"))
		{
			Exploit(hWE, hStorm, (LPCSTR)423, BLZSRegQueryValueDword_Proxy);
			Exploit(hWE, hStorm, (LPCSTR)422, BLZSRegQueryValueString_Proxy);
			Exploit(hWE, hStorm, (LPCSTR)426, BLZSRegSetValueDword_Proxy);
			Exploit(hWE, hStorm, (LPCSTR)425, BLZSRegSetValueString_Proxy);
		}
		else
			return FALSE;

	return TRUE;
}

//--------------------------------------------------------------------------

LSTATUS CALLBACK BLZSRegQueryValueDword_Proxy(LPCSTR lpPath, LPCSTR lpValueName, LPDWORD lpcbData, LPBYTE lpData)
{
	std::ifstream file(JSON_NAME);

	rapidjson::IStreamWrapper isw(file);
	doc.ParseStream(isw);

	file.close();

	if (!doc.IsObject())
		doc.SetObject();

	if (doc[lpPath].HasMember(lpValueName))
	{
		if (doc[lpPath][lpValueName].IsInt())
		{
			*(int*)lpData = doc[lpPath][lpValueName].GetInt();

			return 1;
		}
	}

	return 0;
}

LSTATUS CALLBACK BLZSRegQueryValueString_Proxy(LPCSTR lpPath, LPCSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	rapidjson::Document doc;

	std::ifstream file(JSON_NAME);

	rapidjson::IStreamWrapper isw(file);
	doc.ParseStream(isw);

	file.close();

	if (!doc.IsObject())
		doc.SetObject();

	if (doc[lpPath].HasMember(lpValueName))
		if (doc[lpPath][lpValueName].IsString())
		{
			strcpy_s((char*)lpData, MAX_PATH, doc[lpPath][lpValueName].GetString());

			return 1;
		}

	return 0;
}

LSTATUS CALLBACK BLZSRegSetValueDword_Proxy(LPCSTR lpPath, LPCSTR lpValueName, DWORD dwType, DWORD dwData)
{
	if (!doc.HasMember(lpPath))
		doc.AddMember(rapidjson::Value(lpPath, strlen(lpPath), doc.GetAllocator()), rapidjson::Value().SetObject(), doc.GetAllocator());

	if (!doc[lpPath].HasMember(lpValueName))
		doc[lpPath].AddMember(rapidjson::Value(lpValueName, strlen(lpValueName), doc.GetAllocator()), rapidjson::Value(0), doc.GetAllocator());

	std::ofstream file(JSON_NAME);

	rapidjson::OStreamWrapper osw(file);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);

	doc[lpPath][lpValueName].SetInt(dwData);

	doc.Accept(writer);
	file.close();

	return 1;
}

LSTATUS CALLBACK BLZSRegSetValueString_Proxy(LPCSTR lpPath, LPCSTR lpValueName, DWORD dwType, LPCSTR lpData)
{
	if (!doc.HasMember(lpPath))
		doc.AddMember(rapidjson::Value(lpPath, strlen(lpPath), doc.GetAllocator()), rapidjson::Value().SetObject(), doc.GetAllocator());

	if (!doc[lpPath].HasMember(lpValueName))
		doc[lpPath].AddMember(rapidjson::Value(lpValueName, strlen(lpValueName), doc.GetAllocator()), rapidjson::Value(lpData, strlen(lpData), doc.GetAllocator()), doc.GetAllocator());

	std::ofstream file(JSON_NAME);

	rapidjson::OStreamWrapper osw(file);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);

	doc[lpPath][lpValueName].SetString(lpData, strlen(lpData), doc.GetAllocator());

	doc.Accept(writer);
	file.close();

	return 1;
}