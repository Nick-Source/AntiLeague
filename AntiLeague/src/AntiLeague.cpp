#include <Windows.h>
#include "c_AntiLeague.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	c_AntiLeague cAntiLeague = c_AntiLeague::getInstance();
	cAntiLeague.Init();

	for (;;)
	{
		cAntiLeague.Attack<AntiLeagueProc>(AntiLeagueProc(), "AntiLeague", NULL);
		cAntiLeague.FreeLibrary();
	}
}