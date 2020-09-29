//#define  _DEBUG
#include <Windows.h>
#include <tchar.h>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;

// @brief �R���\�[����ʂɃt�H�[�}�b�g�t���̕������\��
// @param format �t�H�[�}�b�g(%d �Ƃ� %f �Ƃ���)
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�O�p�ł��A�f�o�b�O���ɂ������삵�܂���
void DebugOutputFormatString(const char* format, ...) 
{
#ifdef  _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif //  _DEBUG
}

// �ʓ|�����Ǐ����Ȃ���΂Ȃ�Ȃ��֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0); // OS�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // ����̏������s��
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
	// �E�B���h�E�N���X�̐������o�^
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSW);
	w.lpfnWndProc = (WNDPROC)WindowProcedure; // �R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DX12sample");				//	�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(nullptr);			//	�n���h�����̎擾

	RegisterClassEx(&w);	// �A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w���OS�ɓ`����j

	RECT wrc = { 0,0,window_width,window_height };
	//�֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

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
	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
	
	DebugOutputFormatString("Show window test.");
	//getchar();
	return 0;
}
