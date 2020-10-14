//�E�B���h�E�\����DirectX������
#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>

#ifdef _DEBUG
#include <iostream>
#endif

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//using namespace std;


// @brief �R���\�[����ʂɃt�H�[�}�b�g�t���̕������\��
// @param format �t�H�[�}�b�g(%d �Ƃ� %f �Ƃ���)
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�O�p�ł��A�f�o�b�O���ɂ������삵�܂���
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}
// �ʓ|�����Ǐ����Ȃ���΂Ȃ�Ȃ��֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//�E�B���h�E���j�����ꂽ��Ă΂�܂�
		PostQuitMessage(0);//OS�ɑ΂��āu�������̃A�v���͏I�����v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}

const unsigned int window_width = 1280;
const unsigned int window_height = 720;

ID3D12Device* _dev = nullptr;
IDXGIFactory6* _dxgiFactory = nullptr;
IDXGISwapChain4* _swapchain = nullptr;
ID3D12CommandAllocator* _cmdAllocator = nullptr;	//P72
ID3D12GraphicsCommandList* _cmdList = nullptr;		//P72
ID3D12CommandQueue* _cmdQueue = nullptr;			//P74

void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(
		IID_PPV_ARGS(&debugLayer));
	if (result == S_OK) {
		debugLayer->EnableDebugLayer();	// �f�o�b�O���C���[��L��������
		debugLayer->Release();					// �L����������C���^�[�t�F�C�X���������
	}
}

#ifdef _DEBUG
int main()
{
#else
#include<Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif // DEBUG
#include <vector>
	DebugOutputFormatString("Show window test.");
	HINSTANCE hInst = GetModuleHandle(nullptr);
	// �E�B���h�E�N���X�̐������o�^
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DirectXTest");//�A�v���P�[�V�����N���X��(�K���ł����ł�)
	w.hInstance = GetModuleHandle(0);//�n���h���̎擾
	RegisterClassEx(&w);//�A�v���P�[�V�����N���X(���������̍�邩���낵������OS�ɗ\������)

	RECT wrc = { 0,0, window_width, window_height };//�E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�E�B���h�E�̃T�C�Y�͂�����Ɩʓ|�Ȃ̂Ŋ֐����g���ĕ␳����

	// �E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,	// �N���X���w��
		_T("DX12�e�X�g"),					// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	// �^�C�g���o�[�Ƌ����������E�B���h�E
		CW_USEDEFAULT,					// �\��x���W��OS�ɂ��C��
		CW_USEDEFAULT,					// �\��y���W��OS�ɂ��C��
		wrc.right - wrc.left,					// �E�B���h�E��
		wrc.bottom - wrc.top,				// �E�B���h�E��
		nullptr,									// �e�E�B���h�E�n���h��
		nullptr,									// ���j���[�n���h��
		w.hInstance,							// �Ăяo���A�v���P�[�V�����n���h��
		nullptr);									// �ǉ��p�����[�^�[

#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif

	// DirectX12����̏�����
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif
	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateDXGIFactory");
		return -1;
	}

	//�A�_�v�^�[�̗񋓗p
	std::vector<IDXGIAdapter*> adapters;

	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	//	�A�_�v�^�[�̐����I�u�W�F�N�g�擾

		std::wstring	strDesc = adesc.Description;

		// �T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;	// �����\�ȃo�[�W���������������烋�[�v��ł��؂�
		}
	}

	//Direct3D�f�o�C�X�̏������@P68
	D3D_FEATURE_LEVEL featureLevel;
	for (auto lv : levels) {
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
			featureLevel = lv;
			break;	//�����\�ȃo�[�W���������t��������ł��؂�
		}
	}
	if (_dev == nullptr) {
		DebugOutputFormatString("FAILED D3D12CreateDevice");
		return -1;
	}

	// �R�}���h���X�g�̍쐬�ƃR�}���h�A���P�[�^�[
	// P73
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateCommandAllocator");
		return -1;
	}
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateCommandList");
		return -1;
	}

	// �R�}���h�L���[
	// P74
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	// �^�C���A�E�g����
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// �A�_�v�^�[��������g��Ȃ��Ƃ��͂O�ōD��
	cmdQueueDesc.NodeMask = 0;

	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	// �R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// �L���[����
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));//�R�}���h�L���[����

	// �X���b�v�`�F�[������
	// P79
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;  // DXGI.h
	swapchainDesc.BufferCount = 2;// �o�b�N�o�b�t�@�[�͐L�яk�݉\
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;// �t���b�v��͑��₩�ɔj��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;// ���Ɏw��Ȃ�
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;// �E�B���h�E�̃t���X�N���[���؂�ւ��\
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain
	);

	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateSwapChainForHwnd");
		return -1;
	}

	// �f�X�N���v�^�[�̐ݒ�@P82
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//�����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; //  �\���� 2 ��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //  ���Ɏw��Ȃ�
	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateDescriptorHeap");
		return -1;
	}

	// �X���b�v�`�F�[���Ƃ̕R�Â��@P83
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);
	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < swcDesc.BufferCount; ++i)
	{
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		_dev->CreateRenderTargetView(_backBuffers[i], nullptr, handle);
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	// 3.4.2�@�t�F���X�@P92
	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateFence");
		return -1;
	}

	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT)
		{
			break;
		}
		//////////////////////////////////////////////
		// DirectX�̏���
		// 3.3.6 �X���b�v�`�F�[���𓮍삳����

		// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
		
		// ���\�[�X�o���A�̐ݒ�@3.4.3
		D3D12_RESOURCE_BARRIER BarrierDesc = {};
		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx];
		BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		_cmdList->ResourceBarrier(1, &BarrierDesc);
		
		// �f�B�X�N���v�^�[�q�[�v�̐擪�A�h���X���Z�o
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		//�����_�[�^�[�Q�b�g�̐ݒ�
		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		//�����_�[�^�[�Q�b�g�̃N���A
		//��ʃN���A
		float clearColor[] = { 1.0f,1.0f,0.0f,1.0f };//���F
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		_cmdList->ResourceBarrier(1, &BarrierDesc);


		// ���߂Ă��������߂����s����
		// ���߂̃N���[�Y
		_cmdList->Close();
		// �R�}���h���X�g�̎��s
		ID3D12CommandList* cmdlists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdlists);

		// 3.4.2�@�t�F���X�œ���҂��@P93
		_cmdQueue->Signal(_fence, ++_fenceVal);
		while (_fence->GetCompletedValue() != _fenceVal) 
		{
			// �C�x���g�n���h���̎擾
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event);
			//�C�x���g����������܂ő҂�������(INFINITE)
			WaitForSingleObject(event, INFINITE);
			//�C�x���g�n���h�������
			CloseHandle(event);
		}

		_cmdAllocator->Reset();		// �L���[���N���A
		_cmdList->Reset(_cmdAllocator, nullptr);	// �ĂуR�}���h���X�g�����߂鏀��


													
													
		// �t���b�v
		_swapchain->Present(1, 0);
		/////////////////////////////

	}

	//�������̃N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(w.lpszClassName, w.hInstance);


	//getchar();
	return 0;
}