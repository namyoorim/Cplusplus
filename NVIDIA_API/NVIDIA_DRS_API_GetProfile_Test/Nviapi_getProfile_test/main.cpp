#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include "nvapi.h"
#include "NvApiDriverSettings.h"

void PrintError(NvAPI_Status status)
{
	NvAPI_ShortString szDesc = {0};
	NvAPI_GetErrorMessage(status, szDesc);
	printf(" NVAPI error: %s\n", szDesc);
	exit(-1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	NvAPI_Status status = NvAPI_Initialize();
	if(status != NVAPI_OK) 
		PrintError(status);

	NvDRSSessionHandle hSession = 0;
	status = NvAPI_DRS_CreateSession(&hSession);
	if(status != NVAPI_OK) 
		PrintError(status);

	status = NvAPI_DRS_LoadSettings(hSession);
	if(status != NVAPI_OK)
		PrintError(status);

	/*
	// 기본 베이스 프로파일 가지고 올 때
	NvDRSProfileHandle hProfile = 0;
	status = NvAPI_DRS_GetBaseProfile(hSession, &hProfile);
	if(status != NVAPI_OK) 
		PrintError(status);*/

	// 프로파일을 찾아서
	NvDRSProfileHandle hProfile = 0;
	NvAPI_UnicodeString name;
	memcpy_s(name, sizeof(name), L"(profile name)", 14 * sizeof(wchar_t));
	status = NvAPI_DRS_FindProfileByName(hSession, name, &hProfile);
	if (status != NVAPI_OK)
	{
		PrintError(status);
	}

	// 프로파일의 상세 정보 가져오기
	NVDRS_PROFILE profileInformation = { 0 };
	profileInformation.version = NVDRS_PROFILE_VER;
	status = NvAPI_DRS_GetProfileInfo(hSession, hProfile, &profileInformation);
	if (status != NVAPI_OK)
	{
		PrintError(status);
		return false;
	}
	wprintf(L"Profile Name: %s\n", profileInformation.profileName);
	printf("Number of Applications associated with the Profile: %d\n", profileInformation.numOfApps);

	// 설정된 실행파일 찍어줌
	if (profileInformation.numOfApps> 0)
	{
		NVDRS_APPLICATION *appArray = new NVDRS_APPLICATION[profileInformation.numOfApps];
		NvU32 numAppsRead = profileInformation.numOfApps, i;
		appArray[0].version = NVDRS_APPLICATION_VER;
		status = NvAPI_DRS_EnumApplications(hSession, hProfile, 0, &numAppsRead, appArray);
		if (status != NVAPI_OK)
		{
			PrintError(status);
			delete[] appArray;
			return false;
		}
		for (i = 0; i<numAppsRead; i++)
			wprintf(L"Executable: %s\n", appArray[i].appName);

		delete[] appArray;
	}

	// check
	bool aoc = false; // Ambient occlusion compatibility
	bool aos = false; // Ambient occlusion setting

	// 디폴드값이랑 다른거 찍어줌
	if (profileInformation.numOfSettings> 0)
	{
		NVDRS_SETTING*setArray = new NVDRS_SETTING[profileInformation.numOfSettings];
		NvU32 numSetRead = profileInformation.numOfSettings, i;
		setArray[0].version = NVDRS_SETTING_VER;
		status = NvAPI_DRS_EnumSettings(hSession, hProfile, 0, &numSetRead, setArray);
		if (status != NVAPI_OK)
		{
			PrintError(status);
			return false;
		}
		for (i = 0; i<numSetRead; i++)
		{
			if (setArray[i].settingLocation != NVDRS_CURRENT_PROFILE_LOCATION)
				continue;

			// Ambient occlusion compatibility
			if (setArray[i].settingId == 0x002C7F45 && setArray[i].u32CurrentValue == 0x0000002F)
				aoc = true;

			// Ambient occlusion setting
			if (setArray[i].settingId == AO_MODE_ID && setArray[i].u32CurrentValue == AO_MODE_HIGH)
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

	/*
	// Ambient occlusion compatibility(Setting ID : 0x002C7F45) ---> Value : 0x0000002F(Hawken)
	// 이 값은 상수선언이 안되있어서 직접..
	// 원래 디폴트 세팅값 : 0x00000000
	NVDRS_SETTING drsSetting1 = { 0 };
	drsSetting1.version = NVDRS_SETTING_VER;
	drsSetting1.settingId = 0x002C7F45;
	drsSetting1.settingType = NVDRS_DWORD_TYPE;
	drsSetting1.u32CurrentValue = 0x0000002F;
	
	// Ambient occlusion setting(Setting ID : 0x00667329) ---> Value : 0x00000003(High quality)  (AO_MODE_HIGH)
	// 원래 디폴트 세팅값 : 0x00000000 ( AO_MODE_OFF )
	NVDRS_SETTING drsSetting2 = {0};
	drsSetting2.version = NVDRS_SETTING_VER;
	drsSetting2.settingId = AO_MODE_ID;
	drsSetting2.settingType = NVDRS_DWORD_TYPE;
	drsSetting2.u32CurrentValue = AO_MODE_HIGH;

	status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting1);
	if(status != NVAPI_OK)
		PrintError(status);

	status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting2);
	if (status != NVAPI_OK)
		PrintError(status);

	status = NvAPI_DRS_SaveSettings(hSession);
	if(status != NVAPI_OK)
		PrintError(status);*/

	NvAPI_DRS_DestroySession(hSession);
	hSession = 0;

	return 0;
}

