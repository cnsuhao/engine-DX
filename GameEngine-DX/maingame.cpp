#include "maingame.h"
void CMainGame::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Create the window
	InitWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	//InitD3D
	InitD3D(hWnd);
	//Init the Scene
	InitScene();
	//load the object
	ObjLoader.LoadObject();
	//main loop here
	GameLoop();
	//clean D3D device
	UnInitD3D();

	return;
}

void CMainGame::InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Create a Window
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"GameEngine";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(NULL,
		L"GameEngine",
		L"EngineWindow",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}

void CMainGame::GameLoop()
{
	// enter the main loop
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		// render
		UpdateScene();
		DrawScene();
	}
}

void CMainGame::DrawScene()
{
	// clear the back buffer
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	devcon->ClearRenderTargetView(renderTargetView, clearColor);

	//Refresh the Depth/Stencil view
	devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Set the World/View/Projection matrix, then send it to constant buffer in effect file
	World = XMMatrixIdentity();
	WVP = World * FixedCamera.camView * FixedCamera.camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );

	//Draw the triangle
	devcon->DrawIndexed(6, 0, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}
//Init the D3D device
void CMainGame::InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH;                    // set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;                  // set the back buffer height
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.BufferCount = 1;                                    // one back buffer
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching
	scd.SampleDesc.Quality = 0;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	// create a device, device context and swap chain using scd
	hr = D3D11CreateDeviceAndSwapChain(	NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										NULL,
										NULL,
										NULL,
										D3D11_SDK_VERSION,
										&scd,
										&swapchain,
										&dev,
										NULL,
										&devcon);


	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	hr = dev->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
	pBackBuffer->Release();

	//Describe Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = SCREEN_WIDTH;
	depthStencilDesc.Height    = SCREEN_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	//Create the Depth/Stencil Buffer
	dev->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	//Using the DS buffer to create the DS view
	dev->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	// set the render target
	devcon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

//Clean the D3D device
void CMainGame::UnInitD3D()
{
	// clean up DirectX
	swapchain->SetFullscreenState(FALSE, NULL);	// switch to windowed mode
	swapchain->Release();						// close and release all existing COM objects
	renderTargetView->Release();
	dev->Release();
	devcon->Release();
	squareVertBuffer->Release();
	squareIndexBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();
}
// Init Scene
void CMainGame::InitScene()
{
	//compile the vertex shader from effect file
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	//compile the pixel shader from effect file
	hr = D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	
	//using buffer to create the shader objects
	hr = dev->CreateVertexShader(	VS_Buffer->GetBufferPointer(), 
									VS_Buffer->GetBufferSize(), 
									NULL,
									&VS);

	hr = dev->CreatePixelShader(	PS_Buffer->GetBufferPointer(), 
									PS_Buffer->GetBufferSize(), 
									NULL,
									&PS);
	
	//Set shader to the device context
	devcon->VSSetShader(VS, 0, 0);
	devcon->PSSetShader(PS, 0, 0);

	//vertex buffer
	Vertex v[] =
	{
		Vertex( -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f ),
		Vertex( -0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f ),
		Vertex(  0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f ),
		Vertex(  0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f ),
	};

	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	dev->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	devcon->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = dev->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

	//Set the vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	devcon->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );

	//Vertex Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	};

	UINT numElements = ARRAYSIZE(layout);

	//Create the Input Layout
	hr = dev->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(), 
		VS_Buffer->GetBufferSize(), &vertLayout );

	//Set the Input Layout
	devcon->IASetInputLayout( vertLayout );

	//Set Primitive Topology
	devcon->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// create the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//set the viewport
	devcon->RSSetViewports(1, &viewport);

	//Create the buffer to send the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
	
	//Init the camera
	FixedCamera.Init();
}
//Update Scene
void CMainGame::UpdateScene()
{
	//void
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;
		case WM_KEYDOWN:
		{
			if((UINT)wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
		} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}