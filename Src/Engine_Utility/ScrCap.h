// 스크린 캡쳐를 하는 lib 해더파일 입니다.

#pragma once
#pragma comment(lib, "ScrCap.lib")

#ifdef _AFXDLL
	#define DLLFunction  __declspec(dllexport)
#else
	#define DLLFunction  __declspec(dllimport)
#endif



extern "C" {

DLLFunction BOOL __stdcall ScrCaptureToBMP(HDC hDC, LPCTSTR lpszFileName);
DLLFunction BOOL __stdcall ScrCaptureToGIF(HDC hDC, LPCTSTR lpszFileName);
DLLFunction BOOL __stdcall ScrCaptureToPNG(HDC hDC, LPCTSTR lpszFileName);
DLLFunction BOOL __stdcall ScrCaptureToJPG(HDC hDC, LPCTSTR lpszFileName, ULONG lQuality);

DLLFunction BOOL __stdcall DCToBMP(HDC hDC, int nWidth, int nHeight, LPCTSTR lpszFileName);
DLLFunction BOOL __stdcall DCToGIF(HDC hDC, int nWidth, int nHeight, LPCTSTR lpszFileName);
DLLFunction BOOL __stdcall DCToPNG(HDC hDC, int nWidth, int nHeight, LPCTSTR lpszFileName);
DLLFunction BOOL __stdcall DCToJPG(HDC hDC, int nWidth, int nHeight, LPCTSTR lpszFileName, ULONG lQuality);

}

