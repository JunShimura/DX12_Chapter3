//#define  _DEBUG
#include <Windows.h>
#include <tchar.h>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;

// @brief コンソール画面にフォーマット付きの文字列を表示
// @param format フォーマット(%d とか %f とかの)
// @param 可変長引数
// @remarks この関数はデバッグ用です、デバッグ時にしか動作しません
void DebugOutputFormatString(const char* format, ...) 
{
#ifdef  _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif //  _DEBUG
}

// 面倒だけど書かなければならない関数
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0); // OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 既定の処理を行う
}

const UINT window_width = 1280;
const UINT window_height = 640;


#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR, int)
{
#endif // DEBUG
	// ウィンドウクラスの生成＆登録
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSW);
	w.lpfnWndProc = (WNDPROC)WindowProcedure; // コールバック関数の指定
	w.lpszClassName = _T("DX12sample");				//	アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);			//	ハンドル名の取得

	RegisterClassEx(&w);	// アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）

	RECT wrc = { 0,0,window_width,window_height };
	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

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
	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
	
	DebugOutputFormatString("Show window test.");
	//getchar();
	return 0;
}
