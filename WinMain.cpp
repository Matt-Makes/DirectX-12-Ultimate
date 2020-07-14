
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
#include <comdef.h>
#include "D:\DirectX 12 3D\Resourse\��������Դ����\Common\d3dx12.h"

using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

//ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��
HWND mhMainWnd = 0;
//���ڹ��̺���������,HWND�������ھ��
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//���ڳ�ʼ�������ṹ��(WNDCLASS)
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;	//����������߸ı䣬�����»��ƴ���
	wc.lpfnWndProc = MainWndProc;	//ָ�����ڹ���
	wc.cbClsExtra = 0;	//�����������ֶ���Ϊ��ǰӦ�÷��������ڴ�ռ䣨���ﲻ���䣬������0��
	wc.cbWndExtra = 0;	//�����������ֶ���Ϊ��ǰӦ�÷��������ڴ�ռ䣨���ﲻ���䣬������0��
	wc.hInstance = hInstance;	//Ӧ�ó���ʵ���������WinMain���룩
	wc.hIcon = LoadIcon(0, IDC_ARROW);	//ʹ��Ĭ�ϵ�Ӧ�ó���ͼ��
	wc.hCursor = LoadCursor(0, IDC_ARROW);	//ʹ�ñ�׼�����ָ����ʽ
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//ָ���˰�ɫ������ˢ���
	wc.lpszMenuName = 0;	//û�в˵���
	wc.lpszClassName = L"MainWnd";	//������
	//������ע��ʧ��
	if (!RegisterClass(&wc))
	{
		//��Ϣ����������1����Ϣ���������ھ������ΪNULL������2����Ϣ����ʾ���ı���Ϣ������3�������ı�������4����Ϣ����ʽ
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return 0;
	}

	//������ע��ɹ�
	RECT R;	//�ü�����
	R.left = 0;
	R.top = 0;
	R.right = 1280;
	R.bottom = 720;
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);	//���ݴ��ڵĿͻ�����С���㴰�ڵĴ�С
	int width = R.right - R.left;
	int hight = R.bottom - R.top;

	//��������,���ز���ֵ
	mhMainWnd = CreateWindow(L"MainWnd", L"DirectXMeow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
	//���ڴ���ʧ��
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreatWindow Failed", 0, 0);
		return 0;
	}
	//���ڴ����ɹ�,����ʾ�����´���
	ShowWindow(mhMainWnd, nShowCmd);
	UpdateWindow(mhMainWnd);

	//��Ϣѭ��
	//������Ϣ�ṹ��
	MSG msg = { 0 };
	BOOL bRet = 0;
	//���GetMessage����������0��˵��û�н��ܵ�WM_QUIT
	while (bRet = GetMessage(&msg, 0, 0, 0) != 0)
	{
		//�������-1��˵��GetMessage���������ˣ����������
		if (bRet == -1)
		{
			MessageBox(0, L"GetMessage Failed", L"Errow", MB_OK);
		}
		//�����������ֵ��˵�����յ�����Ϣ
		else
		{
			TranslateMessage(&msg);	//���̰���ת�������������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);	//����Ϣ���ɸ���Ӧ�Ĵ��ڹ���
		}
	}
	return (int)msg.wParam;
}

/// <summary>
/// ���ڹ��̺���
/// </summary>
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//��Ϣ����
	switch (msg)
	{
		//�����ڱ�����ʱ����ֹ��Ϣѭ��
	case WM_DESTROY:
		PostQuitMessage(0);	//��ֹ��Ϣѭ����������WM_QUIT��Ϣ
		return 0;
	default:
		break;
	}
	//������û�д������Ϣת����Ĭ�ϵĴ��ڹ���
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/// <summary>
/// �����豸
/// </summary>
void CreateDevice()
{
	ComPtr<IDXGIFactory4> DXGIFac;
	CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFac));

	ComPtr<ID3D12Device> d3dDevice;
	D3D12CreateDevice(nullptr,		//�˲����������Ϊnullptr����ʹ����������
		D3D_FEATURE_LEVEL_12_0,		//Ӧ�ó�����ҪӲ����֧�ֵ���͹��ܼ���
		IID_PPV_ARGS(&d3dDevice));	//���������豸
}

/// <summary>
/// ����Fence Ϊ��CPU��GPU����ͬ��
/// ��������̵߳Ļ�����mutex
/// </summary>
void CreateFence(ComPtr<ID3D12Device>& d3dDevice)
{
	ComPtr<ID3D12Fence> fence;
	d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

/// <summary>
/// ��ȡ��������С
/// ����֮���ڵ�ַ����ƫ�����ҵ����е�������Ԫ��
/// </summary>
void GetDescriptorSize(ComPtr<ID3D12Device>& d3dDevice)
{
	// ��ȾĿ�껺����������
	UINT RTVDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// ���ģ�建����������
	UINT DSVDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// ��������������������ɫ����Դ������������������ʻ���������
	UINT CbvSrvUavDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

/// <summary>
/// ����MSAA���������
/// </summary>
void SetMSAA(ComPtr<ID3D12Device>& d3dDevice)
{
	// MSAA�����ȼ�
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQL;

	// ��ʼ�����ز�������
	// ������ʽ
	MSAAQL.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//UNORM�ǹ�һ��������޷�������
	// ���ز�������
	MSAAQL.SampleCount = 1;
	// ���ز���ģʽ
	MSAAQL.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	// ���ز�������
	MSAAQL.NumQualityLevels = 0;

	d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAAQL, sizeof(MSAAQL));
}

/// <summary>
/// ����������С������б�����������
/// </summary>
void CreateCommandObject(ComPtr<ID3D12Device>& d3dDevice)
{
	// ��ʼ������������
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �����������
	ComPtr<ID3D12CommandQueue> cmdQueue;
	d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	// �������������
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	// ���������б�
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	d3dDevice->CreateCommandList(0, //����ֵΪ0����GPU
		D3D12_COMMAND_LIST_TYPE_DIRECT, //�����б�����
		cmdAllocator.Get(),	//����������ӿ�ָ��
		nullptr,	//��ˮ��״̬����PSO�����ﲻ���ƣ����Կ�ָ��
		IID_PPV_ARGS(&cmdList));	//���ش����������б�
	// ���������б�ǰ���뽫��ر�
	cmdList->Close();
}

/// <summary>
/// ����������
/// ������������ȾĿ����Դ����̨��������Դ��
/// </summary>
void CreateSwapChain(ComPtr<IDXGIFactory4>& DXGIFac, ComPtr<ID3D12CommandQueue>& cmdQueue)
{
	ComPtr<IDXGISwapChain> swapChain;
	swapChain.Reset();

	// ��ʼ��������������
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = 1280;	// �������ֱ��ʵĿ��
	swapChainDesc.BufferDesc.Height = 720;	// �������ֱ��ʵĸ߶�
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ����������ʾ��ʽ
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;	// ˢ���ʵķ���
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	// ˢ���ʵķ�ĸ
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// ����ɨ��VS����ɨ��(δָ����)
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// ͼ�������Ļ�����죨δָ���ģ�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// ��������Ⱦ����̨������������Ϊ��ȾĿ�꣩
	swapChainDesc.OutputWindow = mhMainWnd;	// ��Ⱦ���ھ��
	swapChainDesc.SampleDesc.Count = 1;	// ���ز�������
	swapChainDesc.SampleDesc.Quality = 0;	// ���ز�������
	swapChainDesc.Windowed = true;	//�Ƿ񴰿ڻ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �̶�д��
	swapChainDesc.BufferCount = 2;	// ��̨������������˫���壩
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//����Ӧ����ģʽ���Զ�ѡ�������ڵ�ǰ���ڳߴ����ʾģʽ��
	// ����������
	DXGIFac->CreateSwapChain(cmdQueue.Get(),	// ������нӿ�ָ��
		&swapChainDesc,		// ������������
		swapChain.GetAddressOf());		// ��������ַ
}

/// <summary>
/// ������������
/// ���������Ǵ����������һ�������ڴ�ռ�
/// </summary>
void CreateDescriptorHeap(ComPtr<ID3D12Device>& d3dDevice)
{
	// ��ʼ��RTV��������������
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
	// ˫���� ���Դ������2��RTV��RTV��
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NodeMask = 0;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	// ͨ���豸����RTV��������
	d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));

	// ��ʼ��DSV��������������
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
	// ���ģ�建�� ֻ��һ��
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.NodeMask = 0;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;

	// ͨ���豸����DSV��������
	d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));
}

/// <summary>
/// ����RTV������
/// </summary>
void CreateRTV(ComPtr<ID3D12DescriptorHeap>& rtvHeap, ComPtr<IDXGISwapChain>& swapChain, ComPtr<ID3D12Device>& d3dDevice, UINT RTVDesSize)
{
	// �����������d3dx12.hͷ�ļ��ж��壬DX�Ⲣû�м���
	// �����࣬���Ĺ��캯����ʼ����D3D12_CPU_DESCRIPTOR_HANDLE�ṹ���е�Ԫ��
	// ��RTV���� �õ��׸�RTV���
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

	ComPtr<ID3D12Resource> swapChainBuffer[2];
	for (int i = 0; i < 2; i++)
	{
		// ��ô��ڽ������еĺ�̨��������Դ
		swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
		// ����RTV
		d3dDevice->CreateRenderTargetView(swapChainBuffer[i].Get(),
			nullptr,	// �ڽ������������Ѿ������˸���Դ�����ݸ�ʽ����������ָ��Ϊ��ָ��
			rtvHeapHandle);	// ����������ṹ�壨�����Ǳ��壬�̳���CD3DX12_CPU_DESCRIPTOR_HANDLE��
		// ƫ�Ƶ����������е���һ��������
		rtvHeapHandle.Offset(1, RTVDesSize);
	}
}

/// <summary>
/// ����DSV������
/// </summary>
void CreateDSV(ComPtr<ID3D12DescriptorHeap>& dsvHeap, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQL, ComPtr<ID3D12Device>& d3dDevice)
{
	// ��CPU�д��������ģ��������Դ
	// ��ʼ��DSV��Դ������
	D3D12_RESOURCE_DESC dsvResourceDesc;
	dsvResourceDesc.Alignment = 0;	// ָ������
	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// ָ����Դά�ȣ����ͣ�ΪTEXTURE2D
	dsvResourceDesc.DepthOrArraySize = 1;	// �������Ϊ1
	dsvResourceDesc.Width = 1280;	// ��Դ��
	dsvResourceDesc.Height = 720;	// ��Դ��
	dsvResourceDesc.MipLevels = 1;	// MIPMAP�㼶����
	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// ָ�������֣����ﲻָ����
	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// ���ģ����Դ��Flag
	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24λ��ȣ�8λģ��,���и������͵ĸ�ʽDXGI_FORMAT_R24G8_TYPELESSҲ����ʹ��
	dsvResourceDesc.SampleDesc.Count = 4;	// ���ز�������
	dsvResourceDesc.SampleDesc.Quality = MSAAQL.NumQualityLevels - 1;

	CD3DX12_CLEAR_VALUE optClear;	// �����Դ���Ż�ֵ��������������ִ���ٶȣ�CreateCommittedResource�����д��룩
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// 24λ��ȣ�8λģ��,���и������͵ĸ�ʽDXGI_FORMAT_R24G8_TYPELESSҲ����ʹ��
	optClear.DepthStencil.Depth = 1;	// ��ʼ���ֵΪ1
	optClear.DepthStencil.Stencil = 0;	// ��ʼģ��ֵΪ0

	// �������ģ�建�� ��Դ
	ComPtr<ID3D12Resource> depthStencilBuffer;
	// �����ģ�������ύ��GPU�Դ���
	d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	// ������ΪĬ�϶ѣ�����д�룩
		D3D12_HEAP_FLAG_NONE,	// Flag
		&dsvResourceDesc,	// ���涨���DSV��Դ������ָ��
		D3D12_RESOURCE_STATE_COMMON,	// ��Դ��״̬Ϊ��ʼ״̬
		&optClear,	// ���涨����Ż�ֵָ��
		IID_PPV_ARGS(&depthStencilBuffer));

	d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
		nullptr,	// D3D12_DEPTH_STENCIL_VIEW_DESC����ָ�룬����&dsvDesc������ע�ʹ���),�����ڴ������ģ����Դʱ�Ѿ��������ģ���������ԣ������������ָ��Ϊ��ָ��
		dsvHeap->GetCPUDescriptorHandleForHeapStart());	// DSV���
}

/// <summary>
/// ���Ds��Դ״̬
/// </summary>
void SetDsStatus(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12Resource> depthStencilBuffer)
{
	cmdList->ResourceBarrier(1,	// Barrier���ϸ���
		&CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,	// ת��ǰ״̬������ʱ��״̬����CreateCommittedResource�����ж����״̬��
			D3D12_RESOURCE_STATE_DEPTH_WRITE));	// ת����״̬Ϊ��д������ͼ������һ��D3D12_RESOURCE_STATE_DEPTH_READ��ֻ�ɶ������ͼ
}

/// <summary>
/// ����������б����������
/// CPU����GPU
/// </summary>
void ExecuteComList(ComPtr<ID3D12GraphicsCommandList>& cmdList, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	cmdList->Close();	// ������������ر�
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	// ���������������б�����
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);	// ������������б����������
}

/// <summary>
/// ʵ��CPU��GPUͬ��
/// </summary>
void FlushCmdQueue(ComPtr<ID3D12CommandQueue> cmdQueue, ComPtr<ID3D12Fence>& fence)
{
	int mCurrentFence = 0;	// ��ʼCPU�ϵ�FenceֵΪ0
	mCurrentFence++;	// CPU��������رպ󣬽���ǰFenceֵ+1
	cmdQueue->Signal(fence.Get(), mCurrentFence);	// �� GPU ������ CPU ���������󣬽� fence�ӿ��� ��fence+1����fence->GetCompletedValue()+1

	if (fence->GetCompletedValue() < mCurrentFence)	// ���С�ڣ�˵�� GPU û�д�������������
	{
		HANDLE eventHandle = CreateEvent(nullptr, false, false, L"FenceSetDone");	//�����¼�
		fence->SetEventOnCompletion(mCurrentFence, eventHandle);// ��fence�ﵽ mCurrentFenceֵ����ִ�е�Signal����ָ���޸���fenceֵ��ʱ������eventHandle�¼�
		WaitForSingleObject(eventHandle, INFINITE);// �ȴ�GPU����fence�������¼���������ǰ�߳�ֱ���¼�������ע���Enent���������ٵȴ���
							   // ���û��Set��Wait���������ˣ�Set��Զ������ã�����Ҳ��û�߳̿��Ի�������̣߳�
		CloseHandle(eventHandle);
	}
}

/// <summary>
/// �����ӿںͲü�����
/// </summary>
void CreateViewPortAndScissorRect()
{
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;
	// �ӿ�����
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = 1280;
	viewPort.Height = 720;
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;
	// �ü��������ã�����������ض������޳���
	// ǰ����Ϊ���ϵ����꣬������Ϊ���µ�����
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = 1280;
	scissorRect.bottom = 720;
}

