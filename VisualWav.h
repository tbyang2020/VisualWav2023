


#pragma once 
#pragma warning( disable : 4100 4010)  // unreferenced formal parameter, single-line comment contains line-continuation character 

#pragma warning( disable : 4244 4305)  // Possible loss of data. 

#ifndef _VISUALMATH_H_
#define _VISUALMATH_H_  // #endif  // _VISUALMATH_H_ //


// 结构仅在VISUALMATH.CPP中实现
#ifdef  _VISUALMATH_CPP_    // #define  _VISUALMATH_CPP_ //
#define VISUALMATH_EXTERN 
#else 
#define VISUALMATH_EXTERN extern 
#endif  // _VISUALMATH_CPP_ //



int& getr();     void incr();    void setr(int v); 



// bool GetFileName(char* szFile, bool bSave = FALSE, HWND hWnd = 0L);   // Interface2D.cpp 


void t_wav_draw(HDC hdc); 
void t_wav_bm(); 
void t_bm_wav();









#endif  // _VISUALMATH_H_ // 