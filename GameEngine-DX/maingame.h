#ifndef _H_MAIN_GAME
#define _H_MAIN_GAME

#include "common.h"
#include "ObjectLoader.h";
#include "camera.h"

//effects constant buffer's structure
struct cbPerObject
{
	XMMATRIX  WVP;
};

//Vertex Structure
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,			//position
		float cr, float cg, float cb, float ca)	//color
		: pos(x,y,z), color(cr, cg, cb, ca){}

	XMFLOAT3 pos;
	XMFLOAT4 color;
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class CMainGame
{
public:
	CMainGame()
	{
		swapchain = NULL;
		dev = NULL;         
		devcon = NULL;    
		renderTargetView = NULL;
		squareIndexBuffer = NULL;
		depthStencilView = NULL;
		depthStencilBuffer = NULL;
		squareVertBuffer = NULL;
		VS = NULL;
		PS = NULL;
		VS_Buffer = NULL;
		PS_Buffer = NULL;
		vertLayout = NULL;
		cbPerObjectBuffer = NULL;
	};

	~CMainGame(){};
	
	//maingame starts here
	void Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);	
	//Create a window
	void InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	// sets up and initializes Direct3D
	void InitD3D(HWND hWnd);
	void UnInitD3D();
	void InitScene();
	void UpdateScene();
	void GameLoop();
	void DrawScene();


private:
	//Window
	HWND hWnd;

	//result handle
	HRESULT hr;

	//Object
	CObjectLoader ObjLoader;
	CCamera FixedCamera;
	
	//effects constant buffer
	cbPerObject cbPerObj;

	//D3D
	IDXGISwapChain *swapchain;					// the pointer to the swap chain interface
	ID3D11Device *dev;							// the pointer to our Direct3D device interface
	ID3D11DeviceContext *devcon;				// the pointer to our Direct3D device context
	ID3D11RenderTargetView *renderTargetView;   // the pointer to our back buffer
	ID3D11Buffer* squareIndexBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11Buffer* squareVertBuffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11InputLayout* vertLayout;
	ID3D11Buffer* cbPerObjectBuffer;

	//space matrix
	XMMATRIX WVP;
	XMMATRIX World;
};
#endif



