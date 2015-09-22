#include "maingame.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CMainGame MainGame;

	//entry of game engine
	MainGame.Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	return 0;
}