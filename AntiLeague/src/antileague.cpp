#include "stdafx.h"
#include "AntiLeague.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    AntiLeague& AntiLeague = AntiLeague::getInstance();

    AntiLeague.Init();
    AntiLeague.Attack<Payload::AntiLeague>("AntiLeague");

    return 0;
}