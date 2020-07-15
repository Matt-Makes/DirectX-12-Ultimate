//***************************************************************************************
//P698
// 引入我们需要的头文件，使用ComPtr的命名空间（namespace），链接库文件，。
//***************************************************************************************
#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <windowsx.h>
#include "D:\DirectX 12 3D\Resourse\this book source code\Common\d3dx12.h"
#include <comdef.h>
#include "D:\DirectX 12 3D\Resourse\this book source code\Common\d3dUtil.h"

using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")



//对后面的void再进行一次上层封装

//声明指针接口和变量
ComPtr<ID3D12Device> d3dDevice;
ComPtr<IDXGIFactory4> DXGIFac;
ComPtr<ID3D12Fence> fence;


// 命令分配器
ComPtr<ID3D12CommandAllocator> cmdAllocator;
// 命令队列
ComPtr<ID3D12CommandQueue> cmdQueue;
// 命令列表
ComPtr<ID3D12GraphicsCommandList> cmdList;
// 深度模板缓存资源
ComPtr<ID3D12Resource> depthStencilBuffer;
ComPtr<ID3D12Resource> swapChainBuffer[2];
// 交换链
ComPtr<IDXGISwapChain> swapChain;
// RTV描述符堆
ComPtr<ID3D12DescriptorHeap> rtvHeap;
// DSV描述符堆
ComPtr<ID3D12DescriptorHeap> dsvHeap;



// 视口
D3D12_VIEWPORT viewPort;
// 裁剪矩形
D3D12_RECT scissorRect;
// 渲染目标缓冲区描述符
UINT RTVDesSize = 0;
// 深度模板缓冲区描述符
UINT DSVDesSize = 0;
// 常量缓冲区描述符、着色器资源缓冲描述符和随机访问缓冲描述符
UINT CbvSrvUavDesSize = 0;
UINT mCurrentBackBuffer = 0;
// MSAA质量等级
D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQL;

void CreateDevice();
void CreateFence();
void GetDescriptorSize();
void SetMSAA();
void CreateCommandObject();
void CreateSwapChain();
void CreateDescriptorHeap();
void CreateRTV();
void CreateDSV();
void CreateViewPortAndScissorRect();
void FlushCmdQueue();






/*初始化Windows窗口。第一步要填充一个WNDCLASS的结构体，然后利用结构体注册一个实例，再根据实例调用CreateWindow函数就可以创建这个窗口。接着需要创建一个循环，来不断处理它接受的消息。而这个消息的回调函数我们也需要创建好。
		*/




		// 某个窗口的句柄，ShowWindow和UpdateWindow函数均要调用此句柄
HWND mhMainWnd = 0;

//窗口过程函数的声明,HWND是主窗口句柄
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);




/////////第一步是创建并显示窗口（结构体+实例+窗口）  (WNDCLASS)

//返回一个布尔值，如果窗口创建成功则返回true
bool InitWindow(HINSTANCE hInstance, int nShowCmd)
{
	//窗口初始化描述结构体(WNDCLASS)
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;	//当工作区宽高改变，则重新绘制窗口
	wc.lpfnWndProc = MainWndProc;	//指定窗口过程
	wc.cbClsExtra = 0;	//借助这两个字段来为当前应用分配额外的内存空间（这里不分配，所以置0）
	wc.cbWndExtra = 0;	//借助这两个字段来为当前应用分配额外的内存空间（这里不分配，所以置0）
	wc.hInstance = hInstance;	//应用程序实例句柄（由WinMain传入）
	wc.hIcon = LoadIcon(0, IDC_ARROW);	//使用默认的应用程序图标
	wc.hCursor = LoadCursor(0, IDC_ARROW);	//使用标准的鼠标指针样式
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//指定了白色背景画刷句柄
	wc.lpszMenuName = 0;	//没有菜单栏
	wc.lpszClassName = L"MainWnd";	//窗口名


	////////////实例////////////////
	//窗口类注册失败
	if (!RegisterClass(&wc))
	{
		//消息框函数，参数1：消息框所属窗口句柄，可为NULL。参数2：消息框显示的文本信息。参数3：标题文本。参数4：消息框样式
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return 0;
	}


	//窗口类注册成功
	RECT R;	//裁剪矩形
	R.left = 0;
	R.top = 0;
	R.right = 1280;
	R.bottom = 720;
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);	//根据窗口的客户区大小计算窗口的大小
	int width = R.right - R.left;
	int hight = R.bottom - R.top;

	//创建窗口,返回布尔值
	mhMainWnd = CreateWindow(L"MainWnd", L"DX12Initialize", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
	//窗口创建失败
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreatWindow Failed", 0, 0);
		return 0;
	}
	//窗口创建成功,则显示并更新窗口
	//为两个函数传入窗口句柄，这样函数才知道需要展示以及更新的窗口是哪个
	ShowWindow(mhMainWnd, nShowCmd);
	UpdateWindow(mhMainWnd);


	return true;//1就是true

}


// 将各种资源设置到渲染流水线上,并最终发出绘制命令
void Draw()
{
	ThrowIfFailed(cmdAllocator->Reset());// 重复使用记录 命令 的相关内存
	ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));// 复用命令列表及其内存

	// 将后台缓冲资源从呈现状态转换到渲染目标状态 准备接收图像渲染
	UINT& ref_mCurrentBackBuffer = mCurrentBackBuffer;
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),// 转换资源为后台缓冲区资源
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));// 从呈现到渲染目标转换

	// 设置视口和裁剪矩形
	cmdList->RSSetViewports(1, &viewPort);
	cmdList->RSSetScissorRects(1, &scissorRect);

	// 清除后台缓冲区和深度缓冲区，并赋值
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), ref_mCurrentBackBuffer, RTVDesSize);
	cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::LightPink, 0, nullptr);// 清除RT背景色为XXX色，并且不设置裁剪矩形
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvHandle,	// DSV描述符句柄
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,	// FLAG
		1.0f,	// 默认深度值
		0,	// 默认模板值
		0,	// 裁剪矩形数量
		nullptr);	// 裁剪矩形指针

	cmdList->OMSetRenderTargets(1,//待绑定的RTV数量
		&rtvHandle,	//指向RTV数组的指针
		true,	//RTV对象在堆内存中是连续存放的
		&dsvHandle);	//指向DSV的指针

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));//从渲染目标到呈现
	//完成命令记的关闭命令列表
	ThrowIfFailed(cmdList->Close());

	// 等CPU将命令都准备好后，需要将待执行的命令列表加入GPU的命令队列
	ID3D12CommandList* commandLists[] = { cmdList.Get() };// 声明并定义命令列表数组
	cmdQueue->ExecuteCommandLists(_countof(commandLists), commandLists);// 将命令从命令列表传至命令队列

	// 交换前后台缓冲区索引
	ThrowIfFailed(swapChain->Present(0, 0));
	// 1变0，0变1，为了让后台缓冲区索引永远为0
	ref_mCurrentBackBuffer = (ref_mCurrentBackBuffer + 1) % 2;

	// 设置fence值，刷新命令队列，使CPU和GPU同步
	FlushCmdQueue();
}





///  使用上面的Draw       第二步是在消息循环中检测消息（循环）

//新建一个Run函数，将之前的消息循环代码复制进去，并将其结构略作调整。因为考虑到我们是游戏程序，所以调整了分支结构，如果没有消息处理，就执行游戏画面和逻辑的计算，这里的Draw函数之后会定义（Draw函数每运行一次，其实就是一帧，所以后期会在它前面加上帧时间的计算）。
//封装消息循环代码	
int Run()
{
	//消息循环
	//定义消息结构体
	MSG msg = { 0 };
	//如果GetMessage函数不返回0，说明没有接受到WM_QUIT
	while (msg.message != WM_QUIT)
	{
		//如果有窗口消息就进行处理
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) //PeekMessage函数会自动填充msg结构体元素
		{
			TranslateMessage(&msg);	//键盘按键转换，将虚拟键消息转换为字符消息
			DispatchMessage(&msg);	//把消息分派给相应的窗口过程
		}
		//否则就执行动画和游戏逻辑
		else
		{
			Draw();
		}
	}
	return (int)msg.wParam;
}








///////第三步是将接收到的消息分派给窗口过程（句柄  窗口过程回调函数）

//窗口过程函数（需要我们自行实现的特殊回调函数）
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//消息处理
	switch (msg)
	{
		//当窗口被销毁时，终止消息循环
	case WM_DESTROY:
		PostQuitMessage(0);	//终止消息循环，并发出WM_QUIT消息
		return 0;
	default:
		break;
	}
	//将上面没有处理的消息转发给默认的窗口过程
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



// 初始化DX
bool InitDirect3D()
{


	//  1、   开启D3D12调试层
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	CreateDevice();
	CreateFence();
	GetDescriptorSize();
	SetMSAA();
	CreateCommandObject();
	CreateSwapChain();
	CreateDescriptorHeap();
	CreateRTV();
	CreateDSV();
	CreateViewPortAndScissorRect();

	return true;
}





//建一个总的Init函数，将InitWindow和InitDirect3D放一起做一个判断，如果两个初始化都正常执行，则判断总的初始化正常执行，返true。
bool Init(HINSTANCE hInstance, int nShowCmd)
{
	if (!InitWindow(hInstance, nShowCmd))
	{
		return false;
	}
	else if (!InitDirect3D())
	{
		return false;
	}
	else
	{
		return true;
	}
}




/// 使用上面的Init   P136    P699
/// 书写WinMain主函数。主函数相当于C++中的Main入口函数。在主函数中首先是一个调试程序的固定写法。
/// <summary>修改WinMain函数，因为要处理抛出异常，所以我们使用try-catch结构。如果初始化成功，则执行Run函数，并通过DxException类捕获异常，返回异常的函数名，以及所在行号。
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try
	{
		if (!Init(hInstance, nShowCmd))
			return 0;

		return Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}





//***************************************************************************************
//  写到这里，我们已经完成了窗口的初始化。直接运行可以看到一个背景纯白的窗口。




//接下来初始化DirectX12。大致步骤如下：
		//1. 开启D3D12调试层
		//2. 创建设备。
		//3. 创建围栏，同步CPU和GPU。
		//4. 获取描述符大小。
		//5. 设置MSAA抗锯齿属性。
		//6. 创建命令队列、命令列表、命令分配器。
		//7. 创建交换链。
		//8. 创建描述符堆。
		//9. 创建描述符。
	   //10. 资源转换。
	   //11. 设置视口和裁剪矩形。
	   //12. 设置围栏刷新命令队列。
	   //13. 将命令从列表传至队列。
//***************************************************************************************

/*

	//////////////////////////////////龙书P136
	//1 、定义一个DxException类 ，统一处理D3D12的异常方式。2 、接下来定义ThrowIfFailed宏。大多数Direct3D函数函数会返回HRESULT错误码，使用ThrowIfFailed宏检测返回的HRESULT值，如果检测失败，则抛出异常，显示调用出错的错误码、函数名、文件名、以及发生错误的行号。（这些代码也可以在头文件d3dUtil.h中找到）3 、注意：这里的ToString函数用到了<comdef.h>头文件，需要在开头初始化窗口时包含这个头文件。它是vs2019自带的头文件 ，系统文件，不要随便使用。

		//AnsiToWString函数（转换成宽字符类型的字符串，wstring）
		//在Windows平台上，我们应该都使用wstring和wchar_t，包括字符串前+L
		inline std::wstring AnsiToWString(const std::string& str)
		{
			WCHAR buffer[512];
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
			return std::wstring(buffer);
		}




	//定义一个DxException类 ，统一处理D3D12的异常方式
	class DxException
	{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);
		std::wstring ToString()const;
		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};
	
	DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}
	std::wstring DxException::ToString()const
	{
		// Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();
		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

	//接下来定义ThrowIfFailed宏。大多数Direct3D函数函数会返回HRESULT错误码，使用ThrowIfFailed宏检测返回的HRESULT值，如果检测失败，则抛出异常，显示调用出错的错误码、函数名、文件名、以及发生错误的行号
	//x是一个函数,hr就是调用函数的结果。L#x 会把这个函数变成Unicode 字符串，也就是函数名本身
	//不定义在头文件中，而是声明在CPP文件里，定义和声明区分开
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
	HRESULT hr__ = (x);                                               \
	std::wstring wfn = AnsiToWString(__FILE__);                       \
	if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

	

	//***************************************************************************************
	*/

	


















//2、 创建设备。/////////////////////////龙书  P89   P102
	// 创建ID3D12Device，此设备代表一个display  adapater（显卡）。先来了解下DXGI API，设计DXGI的基本理念是使，多种图形API中所共有的底层任务，能借助一组通用API来进行处理。而我们的设备即是通过DXGI API的关键接口之一：IDXGIFactory来创建的。所以我们先要CreateDXGIFactory，然后再CreateDevice。
void CreateDevice()
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFac)));

	ThrowIfFailed(D3D12CreateDevice(nullptr,		// 此参数如果设置为nullptr，则使用主适配器
		D3D_FEATURE_LEVEL_12_0,		// 应用程序需要硬件所支持的最低功能级别
		IID_PPV_ARGS(&d3dDevice)));	// 返回所建设备
}

// 创建Fence 为了CPU与GPU进行同步
// 类似与多线程的互斥锁mutex
void CreateFence()
{
	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}


// 获取描述符大小
// 方便之后在地址中做偏移来找到堆中的描述符元素
void GetDescriptorSize()
{
	// 渲染目标缓冲区描述符
	RTVDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 深度模板缓冲区描述符
	DSVDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// 常量缓冲区描述符、着色器资源缓冲描述符和随机访问缓冲描述符
	CbvSrvUavDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// 设置MSAA抗锯齿属性
void SetMSAA()
{
	// 初始化多重采样属性
	// 采样格式
	MSAAQL.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// UNORM是归一化处理的无符号整数
	// 多重采样数量
	MSAAQL.SampleCount = 1;
	// 多重采样模式
	MSAAQL.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	// 多重采样质量
	MSAAQL.NumQualityLevels = 0;

	d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAAQL, sizeof(MSAAQL));
}

// 创建命令队列、命令列表和命令分配器
void CreateCommandObject()
{
	// 初始化命令描述符
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// 创建命令队列
	d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	// 创建命令分配器
	d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	// 创建命令列表
	d3dDevice->CreateCommandList(0, // 掩码值为0，单GPU
		D3D12_COMMAND_LIST_TYPE_DIRECT, // 命令列表类型
		cmdAllocator.Get(),	// 命令分配器接口指针
		nullptr,	// 流水线状态对象PSO，这里不绘制，所以空指针
		IID_PPV_ARGS(&cmdList));	// 返回创建的命令列表
	// 重置命令列表前必须将其关闭
	cmdList->Close();
}

// 创建交换链
// 交换链存着渲染目标资源（后台缓冲区资源）
void CreateSwapChain()
{
	swapChain.Reset();

	// 初始化交换链描述符
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = 1280;	// 缓冲区分辨率的宽度
	swapChainDesc.BufferDesc.Height = 720;	// 缓冲区分辨率的高度
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 缓冲区的显示格式
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;	// 刷新率的分子
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	// 刷新率的分母
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// 逐行扫描VS隔行扫描(未指定的)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// 图像相对屏幕的拉伸（未指定的）
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 将数据渲染至后台缓冲区（即作为渲染目标）
	swapChainDesc.OutputWindow = mhMainWnd;	// 渲染窗口句柄
	swapChainDesc.SampleDesc.Count = 1;	// 多重采样数量
	swapChainDesc.SampleDesc.Quality = 0;	// 多重采样质量
	swapChainDesc.Windowed = true;	// 是否窗口化
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// 固定写法
	swapChainDesc.BufferCount = 2;	// 后台缓冲区数量（双缓冲）
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//自适应窗口模式（自动选择最适于当前窗口尺寸的显示模式）
	// 创建交换链
	DXGIFac->CreateSwapChain(cmdQueue.Get(),	// 命令队列接口指针
		&swapChainDesc,		// 交换链描述符
		swapChain.GetAddressOf());		// 交换链地址
}

// 创建描述符堆
// 描述符堆是存放描述符的一段连续内存空间
void CreateDescriptorHeap()
{
	// 初始化RTV描述符堆描述符
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
	// 双缓冲 所以创建存放2个RTV的RTV堆
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NodeMask = 0;
	// 通过设备创建RTV描述符堆
	d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));

	// 初始化DSV描述符堆描述符
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
	// 深度模板缓存 只有一个
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NodeMask = 0;
	// 通过设备创建DSV描述符堆
	d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));
}

// 创建RTV描述符
void CreateRTV()
{
	// 这个变体类在d3dx12.h头文件中定义，DX库并没有集成
	// 变体类，它的构造函数初始化了D3D12_CPU_DESCRIPTOR_HANDLE结构体中的元素
	// 从RTV堆中 拿到首个RTV句柄
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < 2; i++)
	{
		// 获得存于交换链中的后台缓冲区资源
		swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
		// 创建RTV
		d3dDevice->CreateRenderTargetView(swapChainBuffer[i].Get(),
			nullptr,	// 在交换链创建中已经定义了该资源的数据格式，所以这里指定为空指针
			rtvHeapHandle);	// 描述符句柄结构体（这里是变体，继承自CD3DX12_CPU_DESCRIPTOR_HANDLE）
		// 偏移到描述符堆中的下一个缓冲区
		rtvHeapHandle.Offset(1, RTVDesSize);
	}
}

// 创建DSV描述符
void CreateDSV()
{
	// 在CPU中创建好深度模板数据资源
	// 初始化DSV资源描述符
	D3D12_RESOURCE_DESC dsvResourceDesc;
	dsvResourceDesc.Alignment = 0;	// 指定对齐
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 指定资源维度（类型）为TEXTURE2D
	dsvResourceDesc.DepthOrArraySize = 1;	// 纹理深度为1
	dsvResourceDesc.Width = 1280;	// 资源宽
	dsvResourceDesc.Height = 720;	// 资源高
	dsvResourceDesc.MipLevels = 1;	// MIPMAP层级数量
	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// 指定纹理布局（这里不指定）
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// 深度模板资源的Flag
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
	dsvResourceDesc.SampleDesc.Count = 4;	// 多重采样数量
	dsvResourceDesc.SampleDesc.Quality = MSAAQL.NumQualityLevels - 1;

	CD3DX12_CLEAR_VALUE optClear;	// 清除资源的优化值，提高清除操作的执行速度（CreateCommittedResource函数中传入）
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// 24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
	optClear.DepthStencil.Depth = 1;	// 初始深度值为1
	optClear.DepthStencil.Stencil = 0;	// 初始模板值为0

	// 将深度模板数据提交至GPU显存中
	d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	// 堆类型为默认堆（不能写入）
		D3D12_HEAP_FLAG_NONE,	// Flag
		&dsvResourceDesc,	// 上面定义的DSV资源描述符指针
		D3D12_RESOURCE_STATE_COMMON,	// 资源的状态为初始状态
		&optClear,	// 上面定义的优化值指针
		IID_PPV_ARGS(&depthStencilBuffer));
	// 创建深度模板缓存 资源
	d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
		nullptr,	// D3D12_DEPTH_STENCIL_VIEW_DESC类型指针，可填&dsvDesc（见上注释代码),由于在创建深度模板资源时已经定义深度模板数据属性，所以这里可以指定为空指针
		dsvHeap->GetCPUDescriptorHandleForHeapStart());	// DSV句柄
}



//转换资源
// 标记Ds资源状态
void SetDsStatus()
{
	cmdList->ResourceBarrier(1,	// Barrier屏障个数
		&CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,	// 转换前状态（创建时的状态，即CreateCommittedResource函数中定义的状态）
			D3D12_RESOURCE_STATE_DEPTH_WRITE));	// 转换后状态为可写入的深度图，还有一个D3D12_RESOURCE_STATE_DEPTH_READ是只可读的深度图
}



// 命令从命令列表传入命令队列
// CPU传入GPU
void ExecuteComList()
{
	cmdList->Close();	// 命令添加完后将其关闭
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	// 声明并定义命令列表数组
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);	// 将命令从命令列表传至命令队列
}



// 实现CPU和GPU同步
void FlushCmdQueue()
{
	//设置围栏刷新命令队列

	int mCurrentFence = 0;	// 初始CPU上的Fence值为0
	mCurrentFence++;	// CPU传完命令并关闭后，将当前Fence值+1
	cmdQueue->Signal(fence.Get(), mCurrentFence);	// 当 GPU 处理完 CPU 传入的命令后，将 fence接口中 的fence+1，即fence->GetCompletedValue()+1

	if (fence->GetCompletedValue() < mCurrentFence)	// 如果小于，说明 GPU 没有处理完所有命令
	{
		HANDLE eventHandle = CreateEvent(nullptr, false, false, L"FenceSetDone");	//创建事件
		fence->SetEventOnCompletion(mCurrentFence, eventHandle);// 当fence达到 mCurrentFence值（即执行到Signal（）指令修改了fence值）时触发的eventHandle事件
		WaitForSingleObject(eventHandle, INFINITE);// 等待GPU命中fence，触发事件（阻塞当前线程直到事件触发，注意此Enent需先设置再等待，
							   // 如果没有Set就Wait，就死锁了，Set永远不会调用，所以也就没线程可以唤醒这个线程）
		CloseHandle(eventHandle);
	}
}



// 设置视口和裁剪矩形
void CreateViewPortAndScissorRect()
{
	// 视口设置
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = 1280;
	viewPort.Height = 720;
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;
	// 裁剪矩形设置（矩形外的像素都将被剔除）
	// 前两个为左上点坐标，后两个为右下点坐标
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = 1280;
	scissorRect.bottom = 720;
}
