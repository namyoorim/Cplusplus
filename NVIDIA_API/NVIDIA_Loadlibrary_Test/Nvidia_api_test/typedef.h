typedef unsigned long      NvU32; /* 0 to 4294967295 */

typedef struct _NVDRS_GPU_SUPPORT
{
	NvU32 geforce    :  1;
	NvU32 quadro     :  1;
	NvU32 nvs			:  1;
	NvU32 reserved4  :  1;
	NvU32 reserved5  :  1;
	NvU32 reserved6  :  1;
	NvU32 reserved7  :  1;
	NvU32 reserved8  :  1;
	NvU32 reserved9  :  1;
	NvU32 reserved10 :  1;
	NvU32 reserved11 :  1;
	NvU32 reserved12 :  1;
	NvU32 reserved13 :  1;
	NvU32 reserved14 :  1;
	NvU32 reserved15 :  1;
	NvU32 reserved16 :  1;
	NvU32 reserved17 :  1;
	NvU32 reserved18 :  1;
	NvU32 reserved19 :  1;
	NvU32 reserved20 :  1;
	NvU32 reserved21 :  1;
	NvU32 reserved22 :  1;
	NvU32 reserved23 :  1;
	NvU32 reserved24 :  1;
	NvU32 reserved25 :  1;
	NvU32 reserved26 :  1;
	NvU32 reserved27 :  1;
	NvU32 reserved28 :  1;
	NvU32 reserved29 :  1;
	NvU32 reserved30 :  1;
	NvU32 reserved31 :  1;
	NvU32 reserved32 :  1;
} NVDRS_GPU_SUPPORT;

typedef enum _NVDRS_SETTING_TYPE
{
	NVDRS_DWORD_TYPE,
	NVDRS_BINARY_TYPE,
	NVDRS_STRING_TYPE,
	NVDRS_WSTRING_TYPE
} NVDRS_SETTING_TYPE;

typedef enum _NVDRS_SETTING_LOCATION
{
	NVDRS_CURRENT_PROFILE_LOCATION,
	NVDRS_GLOBAL_PROFILE_LOCATION,
	NVDRS_BASE_PROFILE_LOCATION,
	NVDRS_DEFAULT_PROFILE_LOCATION
} NVDRS_SETTING_LOCATION;

typedef struct _NVDRS_BINARY_SETTING 
{
	NvU32                valueLength;               //!< valueLength should always be in number of bytes.
	unsigned char                 valueData[4096];
} NVDRS_BINARY_SETTING;

typedef struct NVDRS_SETTING
{
	NvU32                      version;                //!< Structure Version
	unsigned short         settingName[2048];            //!< String name of setting
	NvU32                      settingId;              //!< 32 bit setting Id
	NVDRS_SETTING_TYPE         settingType;            //!< Type of setting value.  
	NVDRS_SETTING_LOCATION     settingLocation;        //!< Describes where the value in CurrentValue comes from. 
	NvU32                      isCurrentPredefined;    //!< It is different than 0 if the currentValue is a predefined Value, 
	//!< 0 if the currentValue is a user value. 
	NvU32                      isPredefinedValid;      //!< It is different than 0 if the PredefinedValue union contains a valid value. 
	union                                              //!< Setting can hold either DWORD or Binary value or string. Not mixed types.
	{
		NvU32                      u32PredefinedValue;    //!< Accessing default DWORD value of this setting.
		NVDRS_BINARY_SETTING       binaryPredefinedValue; //!< Accessing default Binary value of this setting.
		//!< Must be allocated by caller with valueLength specifying buffer size, 
		//!< or only valueLength will be filled in.
		unsigned short         wszPredefinedValue[2048];    //!< Accessing default unicode string value of this setting.
	};
	union                                              //!< Setting can hold either DWORD or Binary value or string. Not mixed types.
	{
		NvU32                      u32CurrentValue;    //!< Accessing current DWORD value of this setting.
		NVDRS_BINARY_SETTING       binaryCurrentValue; //!< Accessing current Binary value of this setting.
		//!< Must be allocated by caller with valueLength specifying buffer size, 
		//!< or only valueLength will be filled in.
		unsigned short         wszCurrentValue[2048];    //!< Accessing current unicode string value of this setting.
	};                                                 
} NVDRS_SETTING;
#define NVDRS_SETTING_VER1        MAKE_NVAPI_VERSION(NVDRS_SETTING, 1)
#define NVDRS_SETTING_VER         NVDRS_SETTING_VER1


typedef struct NVDRS_APPLICATION
{
	NvU32                      version;            //!< Structure Version
	NvU32                      isPredefined;       //!< Is the application userdefined/predefined
	unsigned short        appName[2048];            //!< String name of the Application
	unsigned short        userFriendlyName[2048];   //!< UserFriendly name of the Application
	unsigned short        launcher[2048];           //!< Indicates the name (if any) of the launcher that starts the Application
	unsigned short        fileInFolder[2048];       //!< Select this application only if this file is found.
	//!< When specifying multiple files, separate them using the ':' character.
	NvU32                      isMetro:1;          //!< Windows 8 style app
	NvU32                      reserved:31;        //!< Reserved. Should be 0.
} NVDRS_APPLICATION;
#define NVDRS_APPLICATION_VER        MAKE_NVAPI_VERSION(NVDRS_APPLICATION,3)


typedef struct NVDRS_PROFILE_V1
{
	NvU32                      version;            //!< Structure Version
	unsigned short        profileName[2048];        //!< String name of the Profile
	NVDRS_GPU_SUPPORT          gpuSupport;         //!< This read-only flag indicates the profile support on either
	//!< Quadro, or Geforce, or both.
	NvU32                       isPredefined;       //!< Is the Profile user-defined, or predefined
	NvU32                       numOfApps;          //!< Total number of applications that belong to this profile. Read-only
	NvU32                       numOfSettings;      //!< Total number of settings applied for this Profile. Read-only
} NVDRS_PROFILE_V1;
typedef NVDRS_PROFILE_V1         NVDRS_PROFILE;

#define NVDRS_PROFILE_VER1       MAKE_NVAPI_VERSION(NVDRS_PROFILE_V1,1)
#define NVDRS_PROFILE_VER        NVDRS_PROFILE_VER1
#define MAKE_NVAPI_VERSION(typeName,ver) (NvU32)(sizeof(typeName) | ((ver)<<16))

// function pointer types
typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef int (*NvAPI_Initialize_t)();
typedef int (*NvAPI_DRS_CreateSession_t)(int *session);
typedef int (*NvAPI_DRS_LoadSettings_t)(int session);
typedef int (*NvAPI_DRS_FindProfileByName_t)(int session, unsigned short *name, int* hprofile);
typedef int (*NvAPI_DRS_GetProfileInfo_t)(int session, int hprofile, NVDRS_PROFILE *profileinfo);
typedef int (*NvAPI_DRS_DestroySession_t)(int session);
typedef int (*NvAPI_DRS_EnumApplications_t)(int session, int hprofile, int num, NvU32 *numAppsRead, NVDRS_APPLICATION *apparray);
typedef int (*NvAPI_DRS_EnumSettings_t)(int session, int hprofile, int  num, NvU32 *numSetRead, NVDRS_SETTING *setArray);
typedef int (*NvAPI_DRS_FindApplicationByName_t)(int session, unsigned short *appName, int *phprofile, NVDRS_APPLICATION *papplication);