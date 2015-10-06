/*
* @namyoorim
* 참고사이트
* 1. http://eliang.blogspot.kr/2011/05/getting-nvidia-gpu-usage-in-c.html?showComment=1443685955761#c3449572205420189913
* 2. https://github.com/bo3b/3Dmigoto/blob/master/NVAPI/DllMain.cpp
*/

#include <windows.h>
#include <iostream>
#include "typedef.h"


int main()
{   
	HMODULE hmod = LoadLibraryA("nvapi.dll");
	if (hmod == NULL)
	{
		std::cerr << "Couldn't find nvapi.dll" << std::endl;
		return 1;
	}

	// nvapi.dll internal function pointers
	NvAPI_QueryInterface_t NvAPI_QueryInterface = NULL;
	NvAPI_Initialize_t NvAPI_Initialize = NULL;
	NvAPI_DRS_CreateSession_t NvAPI_DRS_CreateSession	= NULL;
	NvAPI_DRS_LoadSettings_t NvAPI_DRS_LoadSettings	= NULL;
	NvAPI_DRS_FindProfileByName_t NvAPI_DRS_FindProfileByName = NULL;
	NvAPI_DRS_GetProfileInfo_t NvAPI_DRS_GetProfileInfo = NULL;
	NvAPI_DRS_DestroySession_t NvAPI_DRS_DestroySession = NULL;
	NvAPI_DRS_EnumApplications_t NvAPI_DRS_EnumApplications = NULL;
	NvAPI_DRS_EnumSettings_t NvAPI_DRS_EnumSettings = NULL;
	NvAPI_DRS_FindApplicationByName_t NvAPI_DRS_FindApplicationByName = NULL;

	// nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
	NvAPI_QueryInterface = (NvAPI_QueryInterface_t) GetProcAddress(hmod, "nvapi_QueryInterface");

	// some useful internal functions that aren't exported by nvapi.dll
	NvAPI_Initialize = (NvAPI_Initialize_t) (*NvAPI_QueryInterface)(0x0150E828);
	NvAPI_DRS_CreateSession = (NvAPI_DRS_CreateSession_t)(*NvAPI_QueryInterface)(0x0694D52E);
	NvAPI_DRS_LoadSettings = (NvAPI_DRS_LoadSettings_t)(*NvAPI_QueryInterface)(0x375DBD6B);
	NvAPI_DRS_FindProfileByName = (NvAPI_DRS_FindProfileByName_t)(*NvAPI_QueryInterface)(0x7E4A9A0B);
	NvAPI_DRS_GetProfileInfo = (NvAPI_DRS_GetProfileInfo_t)(*NvAPI_QueryInterface)(0x61CD6FD6);
	NvAPI_DRS_DestroySession = (NvAPI_DRS_DestroySession_t)(*NvAPI_QueryInterface)(0xDAD9CFF8);
	NvAPI_DRS_EnumApplications = (NvAPI_DRS_EnumApplications_t)(*NvAPI_QueryInterface)(0x7FA2173A);
	NvAPI_DRS_EnumSettings = (NvAPI_DRS_EnumSettings_t)(*NvAPI_QueryInterface)(0xAE3039DA);
	NvAPI_DRS_FindApplicationByName = (NvAPI_DRS_FindApplicationByName_t)(*NvAPI_QueryInterface)(0xEEE566B2);

	if (NvAPI_Initialize == NULL || NvAPI_DRS_CreateSession == NULL || NvAPI_DRS_LoadSettings == NULL || NvAPI_DRS_FindProfileByName == NULL ||
		NvAPI_DRS_GetProfileInfo == NULL || NvAPI_DRS_DestroySession == NULL || NvAPI_DRS_EnumApplications == NULL || NvAPI_DRS_EnumSettings == NULL ||
		NvAPI_DRS_FindApplicationByName == NULL)
	{
		std::cerr << "Couldn't get functions in nvapi.dll" << std::endl;
		return 2;
	}

	// initialize NvAPI library, call it once before calling any other NvAPI functions
	(*NvAPI_Initialize)();

	int hSession = 0;
	(*NvAPI_DRS_CreateSession)(&hSession);
	(*NvAPI_DRS_LoadSettings)(hSession);

	// 실행 파일로 해당되는 프로필 찾기 --> 프로필 반환
	int hProfile = 0;
	NVDRS_APPLICATION *sudden_appArray = new NVDRS_APPLICATION;
	sudden_appArray->version = NVDRS_APPLICATION_VER;
	unsigned short sdden_name[2048];
	memcpy_s(sdden_name, sizeof(sdden_name), L"c:\\Nexon\\SuddenAttack\\suddenattack.exe", 42 * sizeof(wchar_t));
	(*NvAPI_DRS_FindApplicationByName)(hSession, sdden_name, &hProfile, sudden_appArray);

	/*
	// 프로필 이름으로 찾을 경우 --> 프로필 반환
	int hProfile = 0;
	unsigned short name[2048];
	memcpy_s(name, sizeof(name), L"Sudden Attack", 14 * sizeof(wchar_t));
	(*NvAPI_DRS_FindProfileByName)(hSession, name, &hProfile);*/

	NVDRS_PROFILE profileInformation = { 0 };
	profileInformation.version = NVDRS_PROFILE_VER;
	(*NvAPI_DRS_GetProfileInfo)(hSession, hProfile, &profileInformation);

	wprintf(L"Profile Name: %s\n", profileInformation.profileName);
	printf("Number of Applications associated with the Profile: %d\n", profileInformation.numOfApps);

	// 찾은 프로필 셋팅값 찍어줌
	if (profileInformation.numOfApps> 0)
	{
		NVDRS_APPLICATION *appArray = new NVDRS_APPLICATION[profileInformation.numOfApps];
		NvU32 numAppsRead = profileInformation.numOfApps, i;
		appArray[0].version = NVDRS_APPLICATION_VER;
		(*NvAPI_DRS_EnumApplications)(hSession, hProfile, 0, &numAppsRead, appArray);

		for (i = 0; i<numAppsRead; i++)
			wprintf(L"Executable: %s\n", appArray[i].appName);

		delete[] appArray;
	}

	// check
	bool aoc = false; // Ambient occlusion compatibility
	bool aos = false; // Ambient occlusion setting

	// 디폴트 값이랑 다른거 찍어줌
	if (profileInformation.numOfSettings> 0)
	{
		NVDRS_SETTING *setArray = new NVDRS_SETTING[profileInformation.numOfSettings];
		NvU32 numSetRead = profileInformation.numOfSettings, i;
		setArray[0].version = NVDRS_SETTING_VER;
		(*NvAPI_DRS_EnumSettings)(hSession, hProfile, 0, &numSetRead, setArray);

		for (i = 0; i<numSetRead; i++)
		{
			if (setArray[i].settingLocation != NVDRS_CURRENT_PROFILE_LOCATION)
				continue;

			// Ambient occlusion compatibility
			if (setArray[i].settingId == 0x002C7F45 && setArray[i].u32CurrentValue == 0x0000002F)
				aoc = true;

			// Ambient occlusion setting
			if (setArray[i].settingId == 0x00667329 && setArray[i].u32CurrentValue != 0x00000000)
				aos = true;

			printf("Setting ID: %X / ", setArray[i].settingId);
			printf("Setting Value: %X\n", setArray[i].u32CurrentValue);
		}
	}

	printf("--------------------------------------------------------------\n");

	if (aoc == true && aos == true)
		printf("Ambient occlusion가 설정된 컴퓨터입니다.\n");
	else
		printf("설정되어있지 않습니다..\n");

	(*NvAPI_DRS_DestroySession)(hSession);
	hSession = 0;

	getchar();

	return 0;
}