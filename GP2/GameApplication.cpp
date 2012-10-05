#include "GameApplication.h"

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 colour;
	D3DXVECTOR2 texCoords;
};

CGameApplication::CGameApplication(void) //this is the class constructor. We set every member value to NULL
{
	m_pWindow=NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
	m_pVertexBuffer=NULL;
	m_pIndexBuffer=NULL;
	m_pDepthStencilView=NULL;
	m_pDepthStencilTexture=NULL;
	m_pDiffuseTexture=NULL;
}

CGameApplication::~CGameApplication(void) //this is the deconstructor where we deallocate the resources and realease them from memory, we also delete the window here
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState(); //this line dellocates the resources
	if(m_pDiffuseTexture)
		m_pDiffuseTexture->Release();
	if(m_pVertexBuffer)
		m_pVertexBuffer->Release(); // this calls the release function wich releases the object E.G VertexBuffer from memory
	if(m_pIndexBuffer)
		m_pIndexBuffer->Release();
	if(m_pVertexLayout)
		m_pVertexLayout->Release();
	if(m_pEffect)
		m_pEffect->Release();
	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if(m_pDepthStencilTexture)
		m_pDepthStencilTexture->Release();
	if(m_pDepthStencilView)
		m_pDepthStencilView->Release();
	if(m_pSwapChain)
		m_pSwapChain->Release();
	if(m_pD3D10Device)
		m_pD3D10Device->Release();


	if(m_pWindow)
	{
		delete m_pWindow;
		m_pWindow=NULL;
	}
}

bool CGameApplication::init() //this function initializes our game
{
	if(!initWindow())
		return false;
	if(!initGraphics())
		return false;
	if(!initGame())
		return false;
	return true;
}



bool CGameApplication::initGame()
{
	DWORD dwShaderFlags=D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG)||defined(_DEBUG)
	dwShaderFlags|=D3D10_SHADER_DEBUG;
#endif
	ID3D10Blob *pErrors=NULL;
	if(FAILED(D3DX10CreateEffectFromFile(TEXT("texture.fx"),
		NULL,NULL,"fx_4_0",dwShaderFlags,0,
		m_pD3D10Device,NULL,NULL,&m_pEffect,
		&pErrors,NULL)))
	{
		MessageBoxA(NULL,(char*)pErrors->GetBufferPointer(),
			"error",
			MB_OK);
		return false;
	}

	

	m_pTechnique=m_pEffect->GetTechniqueByName("Render");

	D3D10_BUFFER_DESC bd;
	bd.Usage=D3D10_USAGE_DEFAULT;
	bd.ByteWidth=sizeof(Vertex)*8;
	bd.BindFlags=D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags=0;
	bd.MiscFlags=0;

	Vertex vertices[]=
	{
		{D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXCOLOR(1.0f,0.0f,0.0f,1.0f),D3DXVECTOR2(0.0f,3.0f)},//0
		{D3DXVECTOR3(2.0f,0.0f,0.0f),D3DXCOLOR(0.0f,1.0f,0.0f,1.0f),D3DXVECTOR2(3.0f,3.0f)},//1
		{D3DXVECTOR3(0.0f,2.0f,0.0f),D3DXCOLOR(0.0f,0.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f)},//2
		{D3DXVECTOR3(2.0f,2.0f,0.0f),D3DXCOLOR(1.0f,0.0f,1.0f,1.0f),D3DXVECTOR2(3.0f,0.0f)},//3
		{D3DXVECTOR3(0.0f,0.0f,2.0f),D3DXCOLOR(0.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,3.0f)},//4
		{D3DXVECTOR3(2.0f,0.0f,2.0f),D3DXCOLOR(1.0f,1.0f,0.0f,1.0f),D3DXVECTOR2(3.0f,3.0f)},//5
		{D3DXVECTOR3(0.0f,2.0f,2.0f),D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f)},//6
		{D3DXVECTOR3(2.0f,2.0f,2.0f),D3DXCOLOR(0.0f,0.0f,0.0f,1.0f),D3DXVECTOR2(3.0f,0.0f)},//7
	};

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem=vertices;

	if(FAILED(m_pD3D10Device->CreateBuffer(&bd,&InitData,&m_pVertexBuffer)))
		return false;

	

	int indices[]={2,1,0,2,1,3,6,5,4,6,5,7,3,6,2,3,6,7,1,4,0,1,4,5,3,5,1,3,5,7,2,4,0,2,4,6};

	D3D10_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage=D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth=sizeof(int)*36;
	indexBufferDesc.BindFlags=D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags=0;
	indexBufferDesc.MiscFlags=0;

	D3D10_SUBRESOURCE_DATA IndexBufferInitialData;
	IndexBufferInitialData.pSysMem=indices;

	if(FAILED(m_pD3D10Device->CreateBuffer(&indexBufferDesc,
		&IndexBufferInitialData,
		&m_pIndexBuffer)))
		return false;

	m_pD3D10Device->IASetIndexBuffer(m_pIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	D3D10_INPUT_ELEMENT_DESC layout[]=
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
		D3D10_INPUT_PER_VERTEX_DATA,0,},

		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,
		D3D10_INPUT_PER_VERTEX_DATA,0,},

		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,28,
		D3D10_INPUT_PER_VERTEX_DATA,0},


	};
	
	UINT numElements=sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC);
	D3D10_PASS_DESC PassDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

	if(FAILED(m_pD3D10Device->CreateInputLayout(layout,
		numElements,
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&m_pVertexLayout)))
	{
		return false;
	}

	m_pD3D10Device->IASetInputLayout(m_pVertexLayout);

	UINT stride=sizeof(Vertex);
	UINT offset=0;
	m_pD3D10Device->IASetVertexBuffers(0,1
		,&m_pVertexBuffer,&stride,&offset);

	m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXVECTOR3 cameraPos(0.0f,0.0f,-10.0f);
	D3DXVECTOR3 cameraLook(0.0f,0.0f,1.0f);
	D3DXVECTOR3 cameraUp(0.0f,1.0f,0.0f);
	D3DXMatrixLookAtLH(&m_matView,&cameraPos,
		&cameraLook,&cameraUp);

	D3D10_VIEWPORT vp;
	UINT numViewPorts=1;
	m_pD3D10Device->RSGetViewports(&numViewPorts,&vp);

	D3DXMatrixPerspectiveFovLH(&m_matProjection,(float)D3DX_PI*0.25f,
		vp.Width/(FLOAT)vp.Height,0.1f,100.0f);

	m_pViewMatrixVariable=
		m_pEffect->GetVariableByName("matView")->AsMatrix();
	m_pProjectionMatrixVariable=
		m_pEffect->GetVariableByName("matProjection")->AsMatrix();

	m_pProjectionMatrixVariable->SetMatrix((float*)m_matProjection);


	m_vecPosition=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vecScale=D3DXVECTOR3(1.0f,1.0f,1.0f);
	m_vecRotation=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_pWorldMatrixVariable=
	m_pEffect->GetVariableByName("matWorld")->AsMatrix();

	if(FAILED(D3DX10CreateShaderResourceViewFromFile(m_pD3D10Device,
		TEXT("face.png"),NULL,NULL,&m_pDiffuseTexture,NULL)))
	{
		MessageBox(NULL,TEXT("Can't load Texture"),TEXT("ERROR"),MB_OK);
		return false;
	}

	m_pDiffuseTextureVariable=m_pEffect->GetVariableByName("diffuseTexture")->AsShaderResource();	return true;
}

bool CGameApplication::run()//this function will run the while loop untill the window is closed. The while loop checks for windows messages, and update and render the scene if there are none
{
	while(m_pWindow->running())
	{
		if(!m_pWindow->checkForWindowMessages())
		{
			update();
			render();
		}
	}
	return false;
}

void CGameApplication::render() //This function draws to the screen
{
	
	float ClearColor[4]={0.0f,0.125f,0.3f,1.0f};

	m_pD3D10Device->ClearRenderTargetView(m_pRenderTargetView,ClearColor);

	m_pD3D10Device->ClearDepthStencilView(m_pDepthStencilView,
		D3D10_CLEAR_DEPTH,1.0f,0);

	m_pViewMatrixVariable->SetMatrix((float*)m_matView);

	m_pWorldMatrixVariable->SetMatrix((float*)m_matWorld);

	m_pDiffuseTextureVariable->SetResource(m_pDiffuseTexture);

	D3D10_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for(UINT p=0;p<techDesc.Passes;++p)
	{
	m_pTechnique->GetPassByIndex(p)->Apply(0);
	m_pD3D10Device->DrawIndexed(36,0,0);
	}

	m_pSwapChain->Present(0,0);
	
}

void CGameApplication::update()// This function is used to update the game state,Ai,Input Devices and physics
{
	D3DXMatrixScaling(&m_matScale,m_vecScale.x,m_vecScale.y,m_vecScale.z);

	D3DXMatrixRotationYawPitchRoll(&m_matRotation,m_vecRotation.y,
		m_vecRotation.x,m_vecRotation.z);

	D3DXMatrixTranslation(&m_matTranslation,m_vecPosition.x,
		m_vecPosition.y,m_vecPosition.z);

	D3DXMatrixMultiply(&m_matWorld,&m_matScale,&m_matRotation);
	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&m_matTranslation);

	//m_vecRotation.x+=0.0001f;
	m_vecRotation.y+=0.0001f;
	//m_vecRotation.z+=0.0001f;
}

bool CGameApplication::initGraphics()//This function initilizes the Direct3D10
{
	RECT windowRect;
	GetClientRect(m_pWindow->getHandleToWindow(),&windowRect);//Retrives the width and height of the window. These valuse are needed for the creation of the swap chain

	UINT width=windowRect.right-windowRect.left;//stores the window width in an unsigned int
	UINT height=windowRect.bottom-windowRect.top;;//stores the window height in an unsigned int
	

	UINT createDeviceFlags=0;//Initilizes a unsigned int that holds the flags for device creation

#ifdef DEBUG //checks to see if development enviroment is in debug mode
	createDeviceFlags|=D3D10_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC sd;//this variable will hold all options for the creation of the swap chain
	ZeroMemory(&sd,sizeof(sd));//this function takes a memory address of a variable and sets all values in the variable to zero

	if (m_pWindow->isFullScreen())//checks to see if window if fullscreen
		sd.BufferCount=2;//if it is specify 2 buffers (Front and back bufffers)
	else
		sd.BufferCount=1;//if not use 1 buffer (Desktop uses as front buffer) 
	
	sd.OutputWindow=m_pWindow->getHandleToWindow();//asscoiates window handle with swap chain description
	sd.Windowed=(BOOL)(!m_pWindow->isFullScreen());//specifies if in windowd mode convert from a boolean to a BOOL (Not modifier used to state if in windowed mode or not)
	sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;//says buffer will be used as a render target

	sd.SampleDesc.Count=1;//sets the Multisampling(antialsing) parameters of swap chain
	sd.SampleDesc.Quality=0;//there turned off due to performance considerations

	//the following block sets the options for the buffers inside the swap chain
	sd.BufferDesc.Width=width;//sets width of buffer
	sd.BufferDesc.Height=height;//sets height of buffer
	sd.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;//set the format of buffer. It has 8bytes for each component(R,G,B,A)
	sd.BufferDesc.RefreshRate.Numerator=60;//sets refresh rate to 60Hz
	sd.BufferDesc.RefreshRate.Denominator=1;//Using an update on hte vertical black

	//The next block contains the function to create the swap chain and device in one call. This is surrounded in a IF and FAILED to check if the function has failed
	if(FAILED(D3D10CreateDeviceAndSwapChain(
		NULL,//pointer to IDXGIAdapter.(NULL uses as Default Adaptor)
		D3D10_DRIVER_TYPE_HARDWARE,//Type of driver flag hardware used as reference is very slow and should only be used for testing
		NULL,//handle to dynamic link libary(Will be NULL most of the time
		createDeviceFlags,//Used to give additional options when device is created(Used here to put device into dubug mode)
		D3D10_SDK_VERSION,//Version of D3D10 used
		&sd,//pinter to swap chain description.(& used to get pointer) this holds options for swap chain creation
		&m_pSwapChain,//address of pointer to swap chain interface(This will initialize IDCGISwapChain pointer)
		&m_pD3D10Device//address of pointer t D3D10Device(This will initilize ID3D10Device pointer)
		)))
		return false;

	//this block associates a buffer from swap chain with render targer view using the GetBuffer function of the spawp chain
	ID3D10Texture2D*pBackBuffer;
	if(FAILED(m_pSwapChain->GetBuffer(
		0,//index of buffer in swap chain(0 will retrive back buffer)
		__uuidof(ID3D10Texture2D),//id of type of interface that is being retrieved from swap chain
		(void**)&pBackBuffer//pointer to address of the buffer
		)))
		return false;

	//function creates a render target view
	if(FAILED(m_pD3D10Device->CreateRenderTargetView(pBackBuffer,//contains a pointer to reasource. Texture 2D interface inherits from this reasource so it can be passed as a parameter
		NULL,//contatins a pointer to structure that defines options for accessing parts of the render target view
		&m_pRenderTargetView//contains a pointer to address of render target view
		)))
	{
		pBackBuffer->Release();//this process allocates memory even if it fails to it must be released
		return false;
	}
	pBackBuffer->Release();

	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width=width;
	descDepth.Height=height;
	descDepth.MipLevels=1;
	descDepth.ArraySize=1;
	descDepth.Format=DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count=1;
	descDepth.SampleDesc.Quality=0;
	descDepth.Usage=D3D10_USAGE_DEFAULT;
	descDepth.BindFlags=D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags=0;
	descDepth.MiscFlags=0;

	if(FAILED(m_pD3D10Device->CreateTexture2D(&descDepth,
		NULL,&m_pDepthStencilTexture)))
		return false;

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format=descDepth.Format;
	descDSV.ViewDimension=D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice=0;

	if(FAILED(m_pD3D10Device->CreateDepthStencilView(
		m_pDepthStencilTexture,&descDSV,&m_pDepthStencilView)))
		return false;

	//binds a array of render targets to ouptput merger stage of pipeline
	m_pD3D10Device->OMSetRenderTargets(1,//specifies the amount of render targets to be bound to the pipeline
		&m_pRenderTargetView,//pointer to array of render targets
		m_pDepthStencilView//pointer to depth stencil view
		);

	D3D10_VIEWPORT vp;
	vp.Width=width;
	vp.Height=height;
	vp.MinDepth=0.0f;
	vp.MaxDepth=1.0f;
	vp.TopLeftX=0;
	vp.TopLeftY=0;
	m_pD3D10Device->RSSetViewports(1,&vp);


	return true;
}

bool CGameApplication::initWindow()//This function will Initilize the Win32 Window
{
	m_pWindow=new CWin32Window();	//This block of code allocates a new instanse of the win32 Window and calls the init function to create it
	if(!m_pWindow->init(TEXT("Lab 1-Create Device"),800,640,false))
		return false;

	return true;
}

