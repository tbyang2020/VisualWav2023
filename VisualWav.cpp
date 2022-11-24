

#pragma warning(disable:4996)



#define  _VISUALMATH_CPP_ 

#define _WIN32_WINNT 0x500  // for DC_PEN 

#include <Windows.h>
#include <WinGdi.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979  // 3.1415926 5358979 323 846    


#include "VisualWav.h"
#include "wavFile/wavFile.h"
#include "Interface2D/qTrace.h"
#include "Interface2D/Interface2D.h"   // GetFileName 
#include "Interface2D/res/resource.h"

#include "Dib/dib.h"   // GetFileName 


BYTE*& GetDIB();    // Interface2D.cpp  

// ��ε��� trizq, �� r ��Ҫ����, ���Ҫ���ú������ƾ�̬���� r 
int& getr(){ static int r; return r; }     void incr(){ getr()++; }    void setr(int v){ getr()=v; }



// get_sd, ��ȡ������������.  
// i Ϊ�������, cs Ϊ������, 0��1 

// WAVE�ļ��Ļ�����Ԫ�� sample. 44KHz����,��һ���еõ�44000��sample.
// ÿ��sample������8λ��24λ, ����32λ��ʾ(��û������, ֻҪ��8������������). 

// ��һ��ֵ��ע���ϸ��, 8λ�����޷��ŵ���ֵ, ��16λ��16λ���ϴ����з��ŵ���ֵ. 

// ����, �����һ�� 10bit ������, ����sampleλ��Ҫ����8�ı���, ���Ǿ���Ҫ������䵽16λ. 
// 16λ�� 0 - 5λ��0,  6 - 15 λ��ԭʼ��10bit����. ������������ԭ��. 

// ���Ҫ���������, ����Ҫ���������ʱ�̸������sameple. ����, �ڶ�������,ĳһʱ����Ҫ�ֱ����Щ sample ����������, ��Щsample����������. ���, ������Ҫһ�ζ�д����sample.






// drawSeries, ��������. ģ������T=double, float, int
// ���n����������, ry�Ǻ���ֵ����ʾ����.  --- ry�����ں����ⲿ����, ���ڻ�Ϊ co, ������ɫ 
// ʹ��ʵ�߻���, ����ָ��co=0x....FF, ����λ��BYTEΪ 0xFF, ��ɫ����ȡ���ֵ     

// ���ñ��: ԭ��������, y�ᳯ�� 

void wav_draw(HDC hdc, WavData*wd){  
if(!wd) return;   

HWND hWnd=WindowFromDC(hdc); 
RECT rc = { 0,0,0,0, };  GetClientRect(hWnd, &rc);  // WM_SIZE �޷�Ӱ�� drawFunc 
int w = rc.right, h = rc.bottom;

// dwSamples= dataSize/BlockAlign =   dataSize / Channles / (BitsPerSample / 8)






//HFONT hFont=(HFONT)SelectObject(hdc,GetStockObject(DEFAULT_GUI_FONT));   // ��С�� Font  
//HPEN hpen;  hpen=(HPEN)SelectObject(hdc,GetStockObject(DC_PEN));  
//SetDCPenColor(hdc,0xFF0000);  // ǰ��������ɫ -- �������� 





// ��������  
int n= wd->dataSize*8/ wd->BitsPerSample /wd->Channels;  // dwSamples 
if(n<=0) {qTrace("Sample count %d ", n);  return; }

//SetScrollRange(WindowFromDC(hdc), SB_VERT | SB_HORZ, 0, n, TRUE);


//static int T=GetTickCount();  int t= GetTickCount()-T;
int ta= n/wd->SampleRate;   // t%=ta; 

// int te=t, to= t- (w/100*100.)/wd->SampleRate*1000;  if(to<0) to=0;  //  (float)so/wd->SampleRate*1000;  // ��Ӧ���뷶Χ
// int so=to * wd->SampleRate / 1000, se=te*wd->SampleRate/1000;  // ������Χ   



int se= GetScrollPos(hWnd, SB_VERT); 
int so=se-w;   if(so<0) so=0;  


int to= (float)so/ wd->SampleRate*1000;
int te= (float)se / wd->SampleRate * 1000;


XFORM xfo={1,0, 0,1,   0,0,};   
XFORM xf; GetWorldTransform(hdc, &xf);
SetWorldTransform(hdc, &xfo);

SetTextColor(hdc,0); 
char szt[128]={0};  
int ip=sprintf(szt,"����λ�� %d    ������ %d    ������ %d    ʱ�� %d ��    ������ ÿ���� %d ",  wd->BitsPerSample, wd->Channels, n,ta, wd->SampleRate/1000);

TextOut(hdc,4,0,szt,ip);

ip = sprintf(szt, "��ʾ��Χ [%d, %d] (%d -- %d ����)", so,se,to, te);

TextOut(hdc,2, 24, szt, ip);

SetWorldTransform(hdc, &xf);

float MD=(1<<wd->BitsPerSample);   // ����ֵ�Ͻ�  


float dx = (float)w /n, ry = h/MD*2;  //h /512.;   // x-��������ֵ, һ�� <1, ����, dx һ��ָ��Ϊ 1 px.  y-��ʾ��λ, ���ڰ�߶ȶ�Ӧ 512.  
if(dx<1) dx=1; 


dx=5; 

int i; 

float x=0, y=0; 

MoveToEx(hdc, x, 0, 0L);


for (i = so; i < se; i++) { 
x+=dx; 
y=get_sd(i,wd,0); 
y*=ry; 

//sLog("%.2f\t", y);

LineTo(hdc, x, y);  
// SetPixel(hdc, x, y,0x0000FF); 
 
}  // for  



//SelectObject(hdc,hpen); 
//SelectObject(hdc,hFont);
}



void t_wav_draw(HDC hdc){

static WavData *wd=0; 
if(!wd) { 
wd=new WavData; 

char szf[256]; 
const char*pf=szf; 
bool bok=GetFileName(szf);  
if(!bok) pf= "��ͯ��԰.wav";

wav_read(pf,*wd);


// ��������  
int n = wd->dataSize * 8 / wd->BitsPerSample / wd->Channels;  // dwSamples 
if (n <= 0) { qTrace("Sample count %d ", n);  return; }

SetScrollRange(WindowFromDC(hdc), SB_VERT | SB_HORZ, 0, n, TRUE);

}  // if(!.) 




wav_draw(hdc, wd); 

//sLog((const char*)-1); 
//sLog(0); 

}










// wav_bm, wav ����ת��Ϊ 8 λ�Ҷ�ͼ 

// ��ת��ʱ,��Ҫ�²���������λ�������Ƶ��, �ο�ָ���� biHeight�� biClrImportant
// ����λ��Ĭ�� 8 λ, biClrImportant = 8*k ʱ�϶�Ϊ 8*k. 
// SampleRate = biHeight*100. 



bool wav_bm(WAVDATA*wd, const char*szFile){

if (!wd) return false;
if (!szFile) return false;

FILE*fptr = fopen(szFile, "wb");  if (!fptr) { MessageBox(0, szFile, "δ�ܴ�", 0); return false; }


// ������������  
int BS= wd->BitsPerSample/8;
if(8*BS<wd->BitsPerSample) BS++;

// qTrace("BS=%d", BS);
    
int n = wd->dataSize/ wd->Channels / BS ;  // dwSamples 
if (n <= 0) { qTrace("Sample count %d ", n);  return false; }

int ta = n / wd->SampleRate;   // ��ʱ�䳤��


BITMAPFILEHEADER fh = { 0x4D42, 0,  0,0,  0, };  // bfSize,  bfOffBits 
BITMAPINFOHEADER bi = { sizeof(bi),  0,0,0,0,0,  0,0,0,0,0, };
bi.biPlanes = 1;
bi.biCompression = BI_RGB;  // ���� 
bi.biBitCount = 8;

bi.biClrUsed = 0;
bi.biClrImportant = BS*8;   // or  = biClrUsed  // �����Ա�ʾwav������λ��. =8��16��24��... 0 Ҳ��ʾ 8 λ. ���� 8*K ʱȡ8. 


#if VERT_VIEW|0 

bi.biWidth = wd->SampleRate / 100;      // �涨λͼ���, ÿ�� 10 ���� 
bi.biHeight = wd->dataSize / bi.biWidth; 
if(bi.biWidth*bi.biHeight< wd->dataSize)  bi.biHeight++; 

#else  

bi.biHeight = wd->SampleRate / 100;     // �涨λͼ�߶�, ÿ�� 10 ����. 
bi.biWidth = wd->dataSize / bi.biHeight;
if(bi.biWidth*bi.biHeight< wd->dataSize)  bi.biWidth++;

#endif  // VERT_VIEW 


int wb = ((bi.biWidth * bi.biBitCount + 31) & ~31) >> 3 ;  // = (((bits) + 31) / 32 * 4),  =((ih.biWidth*ih.biBitCount / 8 + 3) / 4) * 4;   // 4-bytes aligning   
sLog("\r\n%d,%d��%d\r\n", bi.biWidth, wb, bi.biHeight);


bi.biSizeImage = 0;    //  ����BI_RGB, ������Ϊ0. wb * ih.biHeight;    //wd->dataSize;  // ����Ϊ 0


fh.bfOffBits = sizeof(fh) + sizeof(bi) + 0 * 0;  // �޵�ɫ��  
fh.bfSize = fh.bfOffBits + wb * bi.biHeight;



#if USE_PAL|1

    bi.biClrUsed = 1 << 8;  // ĳЩ GDI ���� ��Ҫ��ɫ��? 
int pc= sizeof(RGBQUAD)*bi.biClrUsed;
    fh.bfOffBits += pc;
    fh.bfSize += pc;

#endif    //  USE_PAL



float MD = (1 << wd->BitsPerSample);   // ����ֵ�Ͻ� 


int ir = fwrite(&fh, sizeof(fh), 1, fptr);  assert(ir== 1);
ir = fwrite(&bi, sizeof(bi), 1, fptr);   assert(ir == 1);


#if USE_PAL|1

RGBQUAD qd = {0,0,0,0};
for (int i = 0; i < bi.biClrUsed; i++) { qd.rgbBlue=qd.rgbGreen=qd.rgbRed=i; fwrite(&qd,sizeof(RGBQUAD),1,fptr);  }

#endif  //  USE_PAL 



#if WRITE_DATA|1

int v;  // BYTE c; 
for(int i=0; i<bi.biHeight; i++)
for(int j=0; j<wb; j++){

v=get_sd(i*bi.biWidth+j, wd,0);  // 0-����   

//c=1*v/MD*255;   // ���� 8 λ�������ļ�, v����Сֵ����Ϊ�� 

// sLog("%d\t",c);

ir = fwrite(&v, BS, 1, fptr);  // assert(ir==1);   // qTrace("ir=%d, %d",ir, (fh.bfSize-sizeof(fh))/1024); 

}  // for i,j 

// qTrace("ip=%d, fh.bfSize=%d", ftell(fp), fh.bfSize)

// fwrite(&c, 1, ih.biSizeImage-ih.biWidth*ih.biHeight, fp);  // ����, biWidthӦ����Ϊ wb.


#endif  //  WRITE_DATA


    
fclose(fptr);
return true;
}



// bm_wav, 8λ�Ҷ�ͼ����Ϊ wav �ļ� 


// ��Ҫ�²���������λ��, �ο�ָ���� biHeight�� biClrImportant
// Ĭ�� 8 λ, biHeight��8*k*10ʱ�϶�Ϊ 8*k. ����, biClrImportant = 8*k ʱ�϶�Ϊ 8*k. 

bool bm_wav(BYTE*pDib, const char*szFile) {

if (!pDib) return false;
if (!szFile) return false;

BITMAPINFOHEADER* bi = (BITMAPINFOHEADER*)pDib;

if (bi->biBitCount != 8) { MessageBox(0, "��ǰ�汾��֧��8λλͼ", "��֧�ֵ��ļ���ʽ", 0); return false; }

FILE*fp = fopen(szFile, "wb");  if (!fp) { MessageBox(0, szFile, "δ�ܴ��ļ�", 0); return false; }


WavHeader wt;  // 3����Ҫָ�����ֶ�  // wt.AudioFmt  = 1 

wt.Channels = 1;
wt.SampleRate = bi->biHeight * 100;  // ÿ�� 10 ����  

wt.BitsPerSample = 8; 
if( bi->biClrImportant %8 == 0 ) wt.BitsPerSample = bi->biClrImportant; 

if(wt.BitsPerSample <= 0 ) wt.BitsPerSample = 8;

int BS= wt.BitsPerSample/8;  // if( BS*8< wt.BitsPerSample) BS++; 

//qTrace("biClrImportant=%d", bi->biClrImportant);


// 3����Ҫ������ֶ� ChunkSize ...  

DWORD dataSize = bi->biWidth*bi->biHeight;  // =BS*dwSamples



    // ������������     
    //int n =  dataSize * 8 / wt.BitsPerSample / wt.Channels;  // =dwSamples 
    //int ta = n / wt.SampleRate;   // ��ʱ�䳤��

    wt.ByteRate = wt.SampleRate*wt.Channels * BS;
    wt.BlockAlign = wt.Channels * BS;
    wt.ChunkSize = 0x24 + dataSize;   // ����ǰ�� 36+8 -8 =36 =0x24 �ֽ�. ����� fact, һ���� 50=0x32, ���12+2=14�ֽ�. 

    // qTrace("ChunkSize=%X, ByteRate=%X, BlockAlign=%X", wt.ChunkSize, wt.ByteRate, wt.BlockAlign);

    int ir = fwrite(&wt, sizeof(wt), 1, fp);   assert(ir == 1);

    // д data �� 
    char ckName[4] = { 'd','a','t','a' };
    ir = fwrite(ckName, 1, 4, fp);  assert(ir == 4);
    ir = fwrite(&dataSize, 1, 4, fp);  assert(ir == 4);



    BYTE*pi = LocBits(pDib);

    ir = fwrite(pi, 1, dataSize, fp);  assert(ir == dataSize);


#if WRITE_DATA|0

    int v;  BYTE c;
    for (int i = 0; i < bi->biHeight; i++)
        for (int j = 0; j < bi->biWidth; j++) {

            v = pi[i*bi->biWidth + j];    // 0-����  
            ir = fwrite(&v, 1, 1, fp);    // qTrace("ir=%d, %d",ir, (fh.bfSize-sizeof(fh))/1024); 

        }  // for i,j 


#endif  //  WRITE_DATA


    fclose(fp);
    return true;
}




 










void t_wav_bm() {


    WavData *wd = new WavData;
    char szf[256];
    const char*pf = szf;
    bool bok = GetFileName(szf);
    if (!bok){ return;  pf = "��ͯ��԰.wav";  }

    wav_read(pf, *wd);



    wav_bm(wd, "t_wavbm.bmp");


// ��  t_wavbm.bmp 
if(GetDIB()!=0){ free(GetDIB()); GetDIB()=0; }
GetDIB()=dib_read("t_wavbm.bmp"); 
SendMessage(GetWnd(), WM_COMMAND, IDV_BITMAP,0);  
InvalidateRect(GetWnd(),0,1); 
 

qLog(0);
}




void t_bm_wav() {

    char szf[256];
    const char*pf = szf;
    bool bok = GetFileName(szf);    //  , true  // bm_wav(GetDIB(), pf);   // ����λͼ�� wav  

    
if(!bok) return;  
// if (!bok) pf = "t_wavbm.bmp";


BYTE*pDib = dib_read(pf);   \
bm_wav(pDib, "t_bmwav.wav");

// �� .wav
PlaySound("t_bmwav.wav",0, SND_FILENAME);


//sLog((const char*)-1); 
sLog(0);
}











































 

 
















