//***************************************************************************************
//P698
// ����������Ҫ��ͷ�ļ���ʹ��ComPtr�������ռ䣨namespace�������ӿ��ļ�����
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
#include "D:\DirectX 12 3D\Resourse\��������Դ����\Common\d3dx12.h"
#include <comdef.h>
#include "../../../Common/d3dUtil.h"

using namespace Microsoft::WRL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")



//�Ժ����void�ٽ���һ���ϲ��װ

//����ָ��ӿںͱ���
ComPtr<ID3D12Device> d3dDevice;
ComPtr<IDXGIFactory4> DXGIFac;
ComPtr<ID3D12Fence> fence;


// ���������
ComPtr<ID3D12CommandAllocator> cmdAllocator;
// �������
ComPtr<ID3D12CommandQueue> cmdQueue;
// �����б�
ComPtr<ID3D12GraphicsCommandList> cmdList;
// ���ģ�建����Դ
ComPtr<ID3D12Resource> depthStencilBuffer;
ComPtr<ID3D12Resource> swapChainBuffer[2];
// ������
ComPtr<IDXGISwapChain> swapChain;
// RTV��������
ComPtr<ID3D12DescriptorHeap> rtvHeap;
// DSV��������
ComPtr<ID3D12DescriptorHeap> dsvHeap;



// �ӿ�
D3D12_VIEWPORT viewPort;
// �ü�����
D3D12_RECT scissorRect;
// ��ȾĿ�껺����������
UINT RTVDesSize = 0;
// ���ģ�建����������
UINT DSVDesSize = 0;
// ��������������������ɫ����Դ������������������ʻ���������
UINT CbvSrvUavDesSize = 0;
UINT mCurrentBackBuffer = 0;
// MSAA�����ȼ�
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




/*��ʼ��Windows���ڡ���һ��Ҫ���һ��WNDCLASS�Ľṹ�壬Ȼ�����ýṹ��ע��һ��ʵ�����ٸ���ʵ������CreateWindow�����Ϳ��Դ���������ڡ�������Ҫ����һ��ѭ���������ϴ��������ܵ���Ϣ���������Ϣ�Ļص���������Ҳ��Ҫ�����á�
		*/


	   

// ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��
HWND mhMainWnd = 0;	  

//���ڹ��̺���������,HWND�������ھ��
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);

		
	

/////////��һ���Ǵ�������ʾ���ڣ��ṹ��+ʵ��+���ڣ�  (WNDCLASS)

//����һ������ֵ��������ڴ����ɹ��򷵻�true
bool InitWindow(HINSTANCE hInstance, int nShowCmd)
{
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


		////////////ʵ��////////////////
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
		mhMainWnd = CreateWindow(L"MainWnd", L"DX12Initialize", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
		//���ڴ���ʧ��
		if (!mhMainWnd)
		{
			MessageBox(0, L"CreatWindow Failed", 0, 0);
			return 0;
		}
		//���ڴ����ɹ�,����ʾ�����´���
		//Ϊ�����������봰�ھ��������������֪����Ҫչʾ�Լ����µĴ������ĸ�
		ShowWindow(mhMainWnd, nShowCmd);
		UpdateWindow(mhMainWnd);


		return true;//1����true

}





/////�ڶ���������Ϣѭ���м����Ϣ��ѭ����

//�½�һ��Run��������֮ǰ����Ϣѭ�����븴�ƽ�ȥ��������ṹ������������Ϊ���ǵ���������Ϸ�������Ե����˷�֧�ṹ�����û����Ϣ������ִ����Ϸ������߼��ļ��㣬�����Draw����֮��ᶨ�壨Draw����ÿ����һ�Σ���ʵ����һ֡�����Ժ��ڻ�����ǰ�����֡ʱ��ļ��㣩��
//��װ��Ϣѭ������	
int Run()
{
		//��Ϣѭ��
		//������Ϣ�ṹ��
		MSG msg = { 0 };
		//���GetMessage����������0��˵��û�н��ܵ�WM_QUIT
		while (msg.message != WM_QUIT)
		{
			//����д�����Ϣ�ͽ��д���
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) //PeekMessage�������Զ����msg�ṹ��Ԫ��
			{
				TranslateMessage(&msg);	//���̰���ת�������������Ϣת��Ϊ�ַ���Ϣ
				DispatchMessage(&msg);	//����Ϣ���ɸ���Ӧ�Ĵ��ڹ���
			}
			//�����ִ�ж�������Ϸ�߼�
			else
			{
				Draw();
			}
		}
		return (int)msg.wParam;
}




// ��������Դ���õ���Ⱦ��ˮ����,�����շ�����������
void Draw()
{
	ThrowIfFailed(cmdAllocator->Reset());// �ظ�ʹ�ü�¼ ���� ������ڴ�
	ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));// ���������б����ڴ�

	// ����̨������Դ�ӳ���״̬ת������ȾĿ��״̬ ׼������ͼ����Ⱦ
	UINT& ref_mCurrentBackBuffer = mCurrentBackBuffer;
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),// ת����ԴΪ��̨��������Դ
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));// �ӳ��ֵ���ȾĿ��ת��

	// �����ӿںͲü�����
	cmdList->RSSetViewports(1, &viewPort);
	cmdList->RSSetScissorRects(1, &scissorRect);

	// �����̨����������Ȼ�����������ֵ
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), ref_mCurrentBackBuffer, RTVDesSize);
	cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::LightPink, 0, nullptr);// ���RT����ɫΪXXXɫ�����Ҳ����òü�����
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvHandle,	// DSV���������
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,	// FLAG
		1.0f,	// Ĭ�����ֵ
		0,	// Ĭ��ģ��ֵ
		0,	// �ü���������
		nullptr);	// �ü�����ָ��

	cmdList->OMSetRenderTargets(1,//���󶨵�RTV����
		&rtvHandle,	//ָ��RTV�����ָ��
		true,	//RTV�����ڶ��ڴ�����������ŵ�
		&dsvHandle);	//ָ��DSV��ָ��

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainBuffer[ref_mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));//����ȾĿ�굽����
	//�������ļ�¼�ر������б�
	ThrowIfFailed(cmdList->Close());

	// ��CPU�����׼���ú���Ҫ����ִ�е������б����GPU���������
	ID3D12CommandList* commandLists[] = { cmdList.Get() };// ���������������б�����
	cmdQueue->ExecuteCommandLists(_countof(commandLists), commandLists);// ������������б����������

	// ����ǰ��̨����������
	ThrowIfFailed(swapChain->Present(0, 0));
	// 1��0��0��1��Ϊ���ú�̨������������ԶΪ0
	ref_mCurrentBackBuffer = (ref_mCurrentBackBuffer + 1) % 2;

	// ����fenceֵ��ˢ��������У�ʹCPU��GPUͬ��
	FlushCmdQueue();

}



///////�������ǽ����յ�����Ϣ���ɸ����ڹ��̣����  ���ڹ��̻ص�������

//���ڹ��̺�������Ҫ��������ʵ�ֵ�����ص�������
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







/// <summary>    P136    P699
/// ��дWinMain���������������൱��C++�е�Main��ں���������������������һ�����Գ���Ĺ̶�д����
/// <summary>�޸�WinMain��������ΪҪ�����׳��쳣����������ʹ��try-catch�ṹ�������ʼ���ɹ�����ִ��Run��������ͨ��DxException�ಶ���쳣�������쳣�ĺ��������Լ������кš�
int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
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
//  д����������Ѿ�����˴��ڵĳ�ʼ����ֱ�����п��Կ���һ���������׵Ĵ��ڡ�




//��������ʼ��DirectX12�����²������£�
		//1. ����D3D12���Բ�
		//2. �����豸��
		//3. ����Χ����ͬ��CPU��GPU��
		//4. ��ȡ��������С��
		//5. ����MSAA��������ԡ�
		//6. ����������С������б������������
		//7. ������������
		//8. �����������ѡ�
		//9. ������������
	   //10. ��Դת����
	   //11. �����ӿںͲü����Ρ�
	   //12. ����Χ��ˢ��������С�
	   //13. ��������б������С�
//***************************************************************************************

/*

	//////////////////////////////////����P136 


	//1 ������һ��DxException�� ��ͳһ����D3D12���쳣��ʽ��2 ������������ThrowIfFailed�ꡣ�����Direct3D���������᷵��HRESULT�����룬ʹ��ThrowIfFailed���ⷵ�ص�HRESULTֵ��������ʧ�ܣ����׳��쳣����ʾ���ó���Ĵ����롢���������ļ������Լ�����������кš�����Щ����Ҳ������ͷ�ļ�d3dUtil.h���ҵ���3 ��ע�⣺�����ToString�����õ���<comdef.h>ͷ�ļ�����Ҫ�ڿ�ͷ��ʼ������ʱ�������ͷ�ļ�������vs2019�Դ���ͷ�ļ� ��ϵͳ�ļ�����Ҫ���ʹ�á�


		//AnsiToWString������ת���ɿ��ַ����͵��ַ�����wstring��
		//��Windowsƽ̨�ϣ�����Ӧ�ö�ʹ��wstring��wchar_t�������ַ���ǰ+L
		inline std::wstring AnsiToWString(const std::string& str)
		{
			WCHAR buffer[512];
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
			return std::wstring(buffer);


		}
		
	//����һ��DxException�� ��ͳһ����D3D12���쳣��ʽ
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




	
	//����������ThrowIfFailed�ꡣ�����Direct3D���������᷵��HRESULT�����룬ʹ��ThrowIfFailed���ⷵ�ص�HRESULTֵ��������ʧ�ܣ����׳��쳣����ʾ���ó���Ĵ����롢���������ļ������Լ�����������к�
	//x��һ������,hr���ǵ��ú����Ľ����L#x �������������Unicode �ַ�����Ҳ���Ǻ���������
	//��������ͷ�ļ��У�����������CPP�ļ��������������ֿ�
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif
	
	*/

//***************************************************************************************


	
	
// ��ʼ��DX
	bool InitDirect3D()
	{


		//  1��   ����D3D12���Բ�
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


//��һ���ܵ�Init��������InitWindow��InitDirect3D��һ����һ���жϣ����������ʼ��������ִ�У����ж��ܵĳ�ʼ������ִ�У���true��
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













	

//2�� �����豸��/////////////////////////����  P89   P102
	// ����ID3D12Device�����豸����һ��display  adapater���Կ����������˽���DXGI API�����DXGI�Ļ���������ʹ������ͼ��API�������еĵײ������ܽ���һ��ͨ��API�����д��������ǵ��豸����ͨ��DXGI API�Ĺؼ��ӿ�֮һ��IDXGIFactory�������ġ�����������ҪCreateDXGIFactory��Ȼ����CreateDevice��
	void CreateDevice()
	{
		ComPtr<IDXGIFactory4> dxgiFactory;
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
		ComPtr<ID3D12Device> d3dDevice;
		ThrowIfFailed(D3D12CreateDevice(nullptr, //�˲����������Ϊnullptr����ʹ����������
			D3D_FEATURE_LEVEL_12_0,		//Ӧ�ó�����ҪӲ����֧�ֵ���͹��ܼ���
			IID_PPV_ARGS(&d3dDevice)));	//���������豸

	}

	// ����Fence Ϊ��CPU��GPU����ͬ��
    // ��������̵߳Ļ�����mutex
	void CreateFence()
	{
		ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	}

	// ��ȡ��������С
    // ����֮���ڵ�ַ����ƫ�����ҵ����е�������Ԫ��
	void GetDescriptorSize()
	{
		// ��ȾĿ�껺����������
		RTVDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// ���ģ�建����������
		DSVDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		// ��������������������ɫ����Դ������������������ʻ���������
		CbvSrvUavDesSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// ����MSAA���������
	void SetMSAA()
	{
		// ��ʼ�����ز�������
		// ������ʽ
		MSAAQL.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// UNORM�ǹ�һ��������޷�������
		// ���ز�������
		MSAAQL.SampleCount = 1;
		// ���ز���ģʽ
		MSAAQL.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		// ���ز�������
		MSAAQL.NumQualityLevels = 0;

		d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAAQL, sizeof(MSAAQL));
	}

	// ����������С������б�����������
	void CreateCommandObject()
	{
		// ��ʼ������������
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		// �����������
		d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
		// �������������
		d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
		// ���������б�
		d3dDevice->CreateCommandList(0, // ����ֵΪ0����GPU
			D3D12_COMMAND_LIST_TYPE_DIRECT, // �����б�����
			cmdAllocator.Get(),	// ����������ӿ�ָ��
			nullptr,	// ��ˮ��״̬����PSO�����ﲻ���ƣ����Կ�ָ��
			IID_PPV_ARGS(&cmdList));	// ���ش����������б�
		// ���������б�ǰ���뽫��ر�
		cmdList->Close();
	}

	// ����������
    // ������������ȾĿ����Դ����̨��������Դ��
	void CreateSwapChain()
	{
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
		swapChainDesc.Windowed = true;	// �Ƿ񴰿ڻ�
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �̶�д��
		swapChainDesc.BufferCount = 2;	// ��̨������������˫���壩
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//����Ӧ����ģʽ���Զ�ѡ�������ڵ�ǰ���ڳߴ����ʾģʽ��
		// ����������
		DXGIFac->CreateSwapChain(cmdQueue.Get(),	// ������нӿ�ָ��
			&swapChainDesc,		// ������������
			swapChain.GetAddressOf());		// ��������ַ
	}

	// ������������
    // ���������Ǵ����������һ�������ڴ�ռ�
	void CreateDescriptorHeap()
	{
		// ��ʼ��RTV��������������
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
		// ˫���� ���Դ������2��RTV��RTV��
		rtvDescriptorHeapDesc.NumDescriptors = 2;
		rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescriptorHeapDesc.NodeMask = 0;
		// ͨ���豸����RTV��������
		d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvHeap));

		// ��ʼ��DSV��������������
		D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
		// ���ģ�建�� ֻ��һ��
		dsvDescriptorHeapDesc.NumDescriptors = 1;
		dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDescriptorHeapDesc.NodeMask = 0;
		// ͨ���豸����DSV��������
		d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeap));
	}

	// ����RTV������
	void CreateRTV()
	{
		// �����������d3dx12.hͷ�ļ��ж��壬DX�Ⲣû�м���
		// �����࣬���Ĺ��캯����ʼ����D3D12_CPU_DESCRIPTOR_HANDLE�ṹ���е�Ԫ��
		// ��RTV���� �õ��׸�RTV���
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

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

	// ����DSV������
	void CreateDSV()
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

		// �����ģ�������ύ��GPU�Դ���
		d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	// ������ΪĬ�϶ѣ�����д�룩
			D3D12_HEAP_FLAG_NONE,	// Flag
			&dsvResourceDesc,	// ���涨���DSV��Դ������ָ��
			D3D12_RESOURCE_STATE_COMMON,	// ��Դ��״̬Ϊ��ʼ״̬
			&optClear,	// ���涨����Ż�ֵָ��
			IID_PPV_ARGS(&depthStencilBuffer));
		// �������ģ�建�� ��Դ
		d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
			nullptr,	// D3D12_DEPTH_STENCIL_VIEW_DESC����ָ�룬����&dsvDesc������ע�ʹ���),�����ڴ������ģ����Դʱ�Ѿ��������ģ���������ԣ������������ָ��Ϊ��ָ��
			dsvHeap->GetCPUDescriptorHandleForHeapStart());	// DSV���
	}



	//ת����Դ
    // ���Ds��Դ״̬
	void SetDsStatus()
	{
		cmdList->ResourceBarrier(1,	// Barrier���ϸ���
			&CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON,	// ת��ǰ״̬������ʱ��״̬����CreateCommittedResource�����ж����״̬��
				D3D12_RESOURCE_STATE_DEPTH_WRITE));	// ת����״̬Ϊ��д������ͼ������һ��D3D12_RESOURCE_STATE_DEPTH_READ��ֻ�ɶ������ͼ
	}



	// ����������б����������
    // CPU����GPU
	void ExecuteComList()
	{
		cmdList->Close();	// ������������ر�
		ID3D12CommandList* cmdLists[] = { cmdList.Get() };	// ���������������б�����
		cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);	// ������������б����������
	}


	
	// ʵ��CPU��GPUͬ��
	void FlushCmdQueue()
	{
		//����Χ��ˢ���������

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
	


	// �����ӿںͲü�����
	void CreateViewPortAndScissorRect()
	{
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












  


