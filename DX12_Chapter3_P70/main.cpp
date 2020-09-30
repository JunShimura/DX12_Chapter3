//#define  _DEBUG
#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include<vector>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#ifdef _DEBUG
#include <iostream>
#endif

//using namespace std;


// @brief コンソール画面にフォーマット付きの文字列を表示
// @param format フォーマット(%d とか %f とかの)
// @param 可変長引数
// @remarks この関数はデバッグ用です、デバッグ時にしか動作しません
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}
// 面倒だけど書かなければならない関数
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//ウィンドウが破棄されたら呼ばれます
		PostQuitMessage(0);//OSに対して「もうこのアプリは終わるんや」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}

const unsigned int window_width = 1280;
const unsigned int window_height = 720;

ID3D12Device* _dev = nullptr;
IDXGIFactory6* _dxgiFactory = nullptr;
IDXGISwapChain4* _swapchain = nullptr;
ID3D12CommandAllocator* _cmdAllocator = nullptr;
ID3D12GraphicsCommandList* _cmdList = nullptr;

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
	// ウィンドウクラスの生成＆登録
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバック関数の指定
	w.lpszClassName = _T("DirectXTest");//アプリケーションクラス名(適当でいいです)
	w.hInstance = GetModuleHandle(0);//ハンドルの取得
	RegisterClassEx(&w);//アプリケーションクラス(こういうの作るからよろしくってOSに予告する)

	RECT wrc = { 0,0, window_width, window_height };//ウィンドウサイズを決める
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//ウィンドウのサイズはちょっと面倒なので関数を使って補正する

	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName,	// クラス名指定
		_T("DX12テスト"),					// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// タイトルバーと居回線があるウィンドウ
		CW_USEDEFAULT,					// 表示x座標はOSにお任せ
		CW_USEDEFAULT,					// 表示y座標はOSにお任せ
		wrc.right - wrc.left,					// ウィンドウ幅
		wrc.bottom - wrc.top,				// ウィンドウ高
		nullptr,									// 親ウィンドウハンドル
		nullptr,									// メニューハンドル
		w.hInstance,							// 呼び出しアプリケーションハンドル
		nullptr);									// 追加パラメーター

	// DirectX12周りの初期化
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	//Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;
	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
			featureLevel = lv;
			break;	//生成可能なバージョンが見付かったら打ち切り
		}
	}
	if (_dev == nullptr) {
		DebugOutputFormatString("FAILED D3D12CreateDevice");
		return -1;
	}
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
	if (result != S_OK) {
		DebugOutputFormatString("FAILED CreateDXGIFactory1");
		return -1;
	}

	//アダプターの列挙用
	std::vector<IDXGIAdapter*> adapters;

	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	//	アダプターの説明オブジェクト取得

		std::wstring	strDesc = adesc.Description;

		// 探したいアダプターの名前を確認
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;	// 生成可能なバージョンが見つかったらループを打ち切り
		}
	}

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
	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//アプリケーションが終わるときにmessageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	//もうこのクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);


	//getchar();
	return 0;
}
