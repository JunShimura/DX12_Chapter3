//#define  _DEBUG
#include <Windows.h>
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

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR, int)
{
#endif // DEBUG
	DebugOutputFormatString("Show window test.");
	getchar();
	return 0;
}
