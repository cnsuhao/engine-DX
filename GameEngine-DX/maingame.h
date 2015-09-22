#ifndef _H_MAIN_GAME
#define _H_MAIN_GAME

#include "common.h"
#include "ObjectLoader.h";

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class CMainGame
{
public:
	CMainGame(){};
	~CMainGame(){};
	
	void Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void InitD3D(HWND hWnd);    // sets up and initializes Direct3D
	void UnInit();
	void GameLoop();
	void RenderFrame();
	void RenderText();


private:
	//Object
	CObjectLoader ObjLoader;
	
	//D3D
	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
	ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer

};
#endif



