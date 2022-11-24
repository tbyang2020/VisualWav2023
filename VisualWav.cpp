

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

// 多次调用 trizq, 秩 r 需要清零, 因此要利用函数控制静态变量 r 
int& getr(){ static int r; return r; }     void incr(){ getr()++; }    void setr(int v){ getr()=v; }



// get_sd, 读取声音样本数据.  
// i 为样本序号, cs 为声道号, 0或1 

// WAVE文件的基本单元是 sample. 44KHz采样,将一秒中得到44000个sample.
// 每个sample可以用8位、24位, 甚至32位表示(数没有限制, 只要是8的整数倍即可). 

// 有一个值得注意的细节, 8位代表无符号的数值, 而16位或16位以上代表有符号的数值. 

// 例如, 如果有一个 10bit 的样本, 由于sample位数要求是8的倍数, 我们就需要把它填充到16位. 
// 16位中 0 - 5位补0,  6 - 15 位是原始的10bit数据. 这就是左补零对齐原则. 

// 如果要处理多声道, 就需要在任意给定时刻给出多个sameple. 例如, 在多声道中,某一时刻需要分辨出哪些 sample 是左声道的, 哪些sample是右声道的. 因此, 我们需要一次读写两个sample.






// drawSeries, 绘制数列. 模板类型T=double, float, int
// 入参n是数列项数, ry是函数值的显示比例.  --- ry可以在函数外部处理, 现在换为 co, 画笔颜色 
// 使用实线画笔, 必须指定co=0x....FF, 即低位的BYTE为 0xFF, 红色分量取最大值     

// 常用标架: 原点在中心, y轴朝上 

void wav_draw(HDC hdc, WavData*wd){  
if(!wd) return;   

HWND hWnd=WindowFromDC(hdc); 
RECT rc = { 0,0,0,0, };  GetClientRect(hWnd, &rc);  // WM_SIZE 无法影响 drawFunc 
int w = rc.right, h = rc.bottom;

// dwSamples= dataSize/BlockAlign =   dataSize / Channles / (BitsPerSample / 8)






//HFONT hFont=(HFONT)SelectObject(hdc,GetStockObject(DEFAULT_GUI_FONT));   // 较小的 Font  
//HPEN hpen;  hpen=(HPEN)SelectObject(hdc,GetStockObject(DC_PEN));  
//SetDCPenColor(hdc,0xFF0000);  // 前景画笔颜色 -- 函数曲线 





// 数据总数  
int n= wd->dataSize*8/ wd->BitsPerSample /wd->Channels;  // dwSamples 
if(n<=0) {qTrace("Sample count %d ", n);  return; }

//SetScrollRange(WindowFromDC(hdc), SB_VERT | SB_HORZ, 0, n, TRUE);


//static int T=GetTickCount();  int t= GetTickCount()-T;
int ta= n/wd->SampleRate;   // t%=ta; 

// int te=t, to= t- (w/100*100.)/wd->SampleRate*1000;  if(to<0) to=0;  //  (float)so/wd->SampleRate*1000;  // 相应毫秒范围
// int so=to * wd->SampleRate / 1000, se=te*wd->SampleRate/1000;  // 样本范围   



int se= GetScrollPos(hWnd, SB_VERT); 
int so=se-w;   if(so<0) so=0;  


int to= (float)so/ wd->SampleRate*1000;
int te= (float)se / wd->SampleRate * 1000;


XFORM xfo={1,0, 0,1,   0,0,};   
XFORM xf; GetWorldTransform(hdc, &xf);
SetWorldTransform(hdc, &xfo);

SetTextColor(hdc,0); 
char szt[128]={0};  
int ip=sprintf(szt,"样本位数 %d    声道数 %d    样本数 %d    时长 %d 秒    采样率 每毫秒 %d ",  wd->BitsPerSample, wd->Channels, n,ta, wd->SampleRate/1000);

TextOut(hdc,4,0,szt,ip);

ip = sprintf(szt, "显示范围 [%d, %d] (%d -- %d 毫秒)", so,se,to, te);

TextOut(hdc,2, 24, szt, ip);

SetWorldTransform(hdc, &xf);

float MD=(1<<wd->BitsPerSample);   // 样本值上界  


float dx = (float)w /n, ry = h/MD*2;  //h /512.;   // x-增量像素值, 一般 <1, 所以, dx 一般指定为 1 px.  y-显示单位, 窗口半高度对应 512.  
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
if(!bok) pf= "儿童乐园.wav";

wav_read(pf,*wd);


// 数据总数  
int n = wd->dataSize * 8 / wd->BitsPerSample / wd->Channels;  // dwSamples 
if (n <= 0) { qTrace("Sample count %d ", n);  return; }

SetScrollRange(WindowFromDC(hdc), SB_VERT | SB_HORZ, 0, n, TRUE);

}  // if(!.) 




wav_draw(hdc, wd); 

//sLog((const char*)-1); 
//sLog(0); 

}










// wav_bm, wav 数据转存为 8 位灰度图 

// 反转换时,需要猜测声音采样位数与采样频率, 参考指标是 biHeight、 biClrImportant
// 采样位数默认 8 位, biClrImportant = 8*k 时认定为 8*k. 
// SampleRate = biHeight*100. 



bool wav_bm(WAVDATA*wd, const char*szFile){

if (!wd) return false;
if (!szFile) return false;

FILE*fptr = fopen(szFile, "wb");  if (!fptr) { MessageBox(0, szFile, "未能打开", 0); return false; }


// 数据样本总数  
int BS= wd->BitsPerSample/8;
if(8*BS<wd->BitsPerSample) BS++;

// qTrace("BS=%d", BS);
    
int n = wd->dataSize/ wd->Channels / BS ;  // dwSamples 
if (n <= 0) { qTrace("Sample count %d ", n);  return false; }

int ta = n / wd->SampleRate;   // 总时间长度


BITMAPFILEHEADER fh = { 0x4D42, 0,  0,0,  0, };  // bfSize,  bfOffBits 
BITMAPINFOHEADER bi = { sizeof(bi),  0,0,0,0,0,  0,0,0,0,0, };
bi.biPlanes = 1;
bi.biCompression = BI_RGB;  // 必须 
bi.biBitCount = 8;

bi.biClrUsed = 0;
bi.biClrImportant = BS*8;   // or  = biClrUsed  // 借用以表示wav的样本位数. =8、16、24、... 0 也表示 8 位. 不是 8*K 时取8. 


#if VERT_VIEW|0 

bi.biWidth = wd->SampleRate / 100;      // 规定位图宽度, 每行 10 毫秒 
bi.biHeight = wd->dataSize / bi.biWidth; 
if(bi.biWidth*bi.biHeight< wd->dataSize)  bi.biHeight++; 

#else  

bi.biHeight = wd->SampleRate / 100;     // 规定位图高度, 每行 10 毫秒. 
bi.biWidth = wd->dataSize / bi.biHeight;
if(bi.biWidth*bi.biHeight< wd->dataSize)  bi.biWidth++;

#endif  // VERT_VIEW 


int wb = ((bi.biWidth * bi.biBitCount + 31) & ~31) >> 3 ;  // = (((bits) + 31) / 32 * 4),  =((ih.biWidth*ih.biBitCount / 8 + 3) / 4) * 4;   // 4-bytes aligning   
sLog("\r\n%d,%d×%d\r\n", bi.biWidth, wb, bi.biHeight);


bi.biSizeImage = 0;    //  对于BI_RGB, 必须设为0. wb * ih.biHeight;    //wd->dataSize;  // 可以为 0


fh.bfOffBits = sizeof(fh) + sizeof(bi) + 0 * 0;  // 无调色板  
fh.bfSize = fh.bfOffBits + wb * bi.biHeight;



#if USE_PAL|1

    bi.biClrUsed = 1 << 8;  // 某些 GDI 函数 需要调色板? 
int pc= sizeof(RGBQUAD)*bi.biClrUsed;
    fh.bfOffBits += pc;
    fh.bfSize += pc;

#endif    //  USE_PAL



float MD = (1 << wd->BitsPerSample);   // 样本值上界 


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

v=get_sd(i*bi.biWidth+j, wd,0);  // 0-声道   

//c=1*v/MD*255;   // 高于 8 位的声音文件, v的最小值可能为负 

// sLog("%d\t",c);

ir = fwrite(&v, BS, 1, fptr);  // assert(ir==1);   // qTrace("ir=%d, %d",ir, (fh.bfSize-sizeof(fh))/1024); 

}  // for i,j 

// qTrace("ip=%d, fh.bfSize=%d", ftell(fp), fh.bfSize)

// fwrite(&c, 1, ih.biSizeImage-ih.biWidth*ih.biHeight, fp);  // 否则, biWidth应当改为 wb.


#endif  //  WRITE_DATA


    
fclose(fptr);
return true;
}



// bm_wav, 8位灰度图保存为 wav 文件 


// 需要猜测声音采样位数, 参考指标是 biHeight、 biClrImportant
// 默认 8 位, biHeight是8*k*10时认定为 8*k. 否则, biClrImportant = 8*k 时认定为 8*k. 

bool bm_wav(BYTE*pDib, const char*szFile) {

if (!pDib) return false;
if (!szFile) return false;

BITMAPINFOHEADER* bi = (BITMAPINFOHEADER*)pDib;

if (bi->biBitCount != 8) { MessageBox(0, "当前版本仅支持8位位图", "不支持的文件格式", 0); return false; }

FILE*fp = fopen(szFile, "wb");  if (!fp) { MessageBox(0, szFile, "未能打开文件", 0); return false; }


WavHeader wt;  // 3个需要指明的字段  // wt.AudioFmt  = 1 

wt.Channels = 1;
wt.SampleRate = bi->biHeight * 100;  // 每行 10 毫秒  

wt.BitsPerSample = 8; 
if( bi->biClrImportant %8 == 0 ) wt.BitsPerSample = bi->biClrImportant; 

if(wt.BitsPerSample <= 0 ) wt.BitsPerSample = 8;

int BS= wt.BitsPerSample/8;  // if( BS*8< wt.BitsPerSample) BS++; 

//qTrace("biClrImportant=%d", bi->biClrImportant);


// 3个需要计算的字段 ChunkSize ...  

DWORD dataSize = bi->biWidth*bi->biHeight;  // =BS*dwSamples



    // 数据样本总数     
    //int n =  dataSize * 8 / wt.BitsPerSample / wt.Channels;  // =dwSamples 
    //int ta = n / wt.SampleRate;   // 总时间长度

    wt.ByteRate = wt.SampleRate*wt.Channels * BS;
    wt.BlockAlign = wt.Channels * BS;
    wt.ChunkSize = 0x24 + dataSize;   // 包括前面 36+8 -8 =36 =0x24 字节. 如果有 fact, 一般是 50=0x32, 多出12+2=14字节. 

    // qTrace("ChunkSize=%X, ByteRate=%X, BlockAlign=%X", wt.ChunkSize, wt.ByteRate, wt.BlockAlign);

    int ir = fwrite(&wt, sizeof(wt), 1, fp);   assert(ir == 1);

    // 写 data 块 
    char ckName[4] = { 'd','a','t','a' };
    ir = fwrite(ckName, 1, 4, fp);  assert(ir == 4);
    ir = fwrite(&dataSize, 1, 4, fp);  assert(ir == 4);



    BYTE*pi = LocBits(pDib);

    ir = fwrite(pi, 1, dataSize, fp);  assert(ir == dataSize);


#if WRITE_DATA|0

    int v;  BYTE c;
    for (int i = 0; i < bi->biHeight; i++)
        for (int j = 0; j < bi->biWidth; j++) {

            v = pi[i*bi->biWidth + j];    // 0-声道  
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
    if (!bok){ return;  pf = "儿童乐园.wav";  }

    wav_read(pf, *wd);



    wav_bm(wd, "t_wavbm.bmp");


// 打开  t_wavbm.bmp 
if(GetDIB()!=0){ free(GetDIB()); GetDIB()=0; }
GetDIB()=dib_read("t_wavbm.bmp"); 
SendMessage(GetWnd(), WM_COMMAND, IDV_BITMAP,0);  
InvalidateRect(GetWnd(),0,1); 
 

qLog(0);
}




void t_bm_wav() {

    char szf[256];
    const char*pf = szf;
    bool bok = GetFileName(szf);    //  , true  // bm_wav(GetDIB(), pf);   // 屏显位图变 wav  

    
if(!bok) return;  
// if (!bok) pf = "t_wavbm.bmp";


BYTE*pDib = dib_read(pf);   \
bm_wav(pDib, "t_bmwav.wav");

// 打开 .wav
PlaySound("t_bmwav.wav",0, SND_FILENAME);


//sLog((const char*)-1); 
sLog(0);
}











































 

 
















