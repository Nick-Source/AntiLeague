#include "stdafx.h"
#include "AntiLeague.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	AntiLeague& cAntiLeague = AntiLeague::getInstance();

	cAntiLeague.Init();
	cAntiLeague.Attack<AntiLeagueProc>(AntiLeagueProc(), "AntiLeague", NULL);

	return 0;
}