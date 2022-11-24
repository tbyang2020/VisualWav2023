



// WAVFILE.h
#ifndef _WAVFILE_H_
#define _WAVFILE_H_    //#endif  // _WAVFILE_H_ //


#include "../Interface2D/qTrace.h"



// 结构仅在WAVFILE.CPP中实现
#ifdef  _WAVFILE_CPP_    //#endif  // _WAVFILE_CPP_ //
#define WAVFILE_EXTERN 
#else 
#define WAVFILE_EXTERN extern 
#endif  // _WAVFILE_CPP_ //

#pragma pack(show)        // 显示默认对齐值 8 
#pragma pack(1)           // 紧凑型  


// WAVEFORMATEX ,  但是 少了成员 cbSize. 比 WAVEFORMAT 多一个成员  wBitsPerSample 
struct WAVEFORMATE{

// WAVEFORMATEX ,  但是 少了成员 cbSize. 比 WAVEFORMAT 多一个成员  wBitsPerSample 
    WORD        wFormatTag = WAVE_FORMAT_PCM;         // format type 格式种类.  值为 WAVE_FORMAT_PCM=1 时表示 PCM 线性编码, 不压缩, 没有fact块. typedef unsigned short int uint16; 
    WORD        nChannels = 1;                        // number of channels (i.e. mono, stereo...) 通道数. 单声道为1, 双声道为2.
    DWORD       nSamplesPerSec = 11025;               // sample rate 采样频率. 
    DWORD       nAvgBytesPerSec = 11025;              // for buffer estimation 数据传输率, 每秒字节, Byte率 = nSamplesPerSec*nChannels * wBitsPerSample / 8. 
    WORD        nBlockAlign = 1;                      // block size of data  块对齐字节数 = nChannels * wBitsPerSample / 8.    

    WORD        wBitsPerSample =8;                    // number of bits per sample of mono data 又称量化位数 
//  WORD        cbSize = 0;                           // the count in bytes of the size of extra information (after cbSize) 扩展信息大小  

};


// 确定不变的头部. 便于读写 wav 文件. 默认 little-endian. 
// IFF(Interchange File Format) 标准使用大端.  RIFF使用小端  little-endian (x86) 

struct WAV_HDR {   //  默认值. 但 riffSize 与 dataSize 有关,  = 文件长度-8.

// The RIFF chunk  descriptor 
    char         RIFF[4]={'R','I','F','F',};   // "RIFF",  Resource Interchange File Format 资源交换文件标志. big-endian 
    DWORD        riffSize = 0;                 // 从下个地址开始到文件结尾的字节数.  typedef unsigned int uint32; // 该字段的数值加 8 为当前文件的实际长度. 如果小于文件长度, 则 "data" 后有其他块. 
    char         WAVE[4]={'W','A','V','E',};   // Format "WAVE" 文件标识 big-endian


// The "fmt" sub-chunk. 描述 "data" sub-chunk 中数据的性质 
    char         fmt[4]  = {'f','m','t',' ',};    // SubchunkID, "fmt ", 波形格式标识     big
    DWORD        fmtSize = 0x10;                  // 过滤字节,一般为 16=00000010H, 或18=12H, 若为12H,则有附加信息 cbSize.   

// WAVEFORMATEX ,  但是 少了成员 cbSize. 比 WAVEFORMAT 多一个成员  wBitsPerSample 
    WORD        wFormatTag = WAVE_FORMAT_PCM;         // format type 格式种类.  值为1时表示 PCM 线性编码, 不压缩, 没有fact块. typedef unsigned short int uint16; 
    WORD        nChannels  = 1;                       // number of channels (i.e. mono, stereo...) 通道数. 单声道为1, 双声道为2.
    DWORD       nSamplesPerSec  = 11025;              // sample rate 采样频率. 
    DWORD       nAvgBytesPerSec = 11025;              // for buffer estimation 数据传输率, 每秒字节, Byte率=SampleRate*nChannels * wBitsPerSample / 8. 
    WORD        nBlockAlign = 1;                      // block size of data  块对齐字节数 = nChannels * wBitsPerSample / 8.    

    WORD        wBitsPerSample = 8;                   // number of bits per sample of mono data 又称量化位数 
//  WORD        cbSize = 0;                           // the count in bytes of the size of extra information (after cbSize) 扩展信息大小  

};   // WAV_HDR


// wav 文件主要内容.  主要为 waveOut# 函数服务:  waveOutOpen 函数需要参数 WAVEFORMATEX, waveOutWrite 函数需要声音数据  
 
struct DIW{
WAVEFORMATEX  wfx;    // 5+ 2个成员.  WAVEFORMAT  5个成员  
DWORD dataSize;
BYTE* pData;  
};







// wav 文件内容描述. 只能大概描述, 因为 RIFF (Resource Interchange File Format) 块组织允许很多不同的块.  
//  IFF(Interchange File Format) 标准使用大端.  RIFF使用小端  little-endian (x86) 
// wav 文件结构在 wavRead 函数中进一步描述 -- qLog 

#if  WAV_FILE_CONTENT|1   // wav文件有3个基本块  RIFF、FORMAT、DATA. RIFF块包含其他块.  
    
struct RIFF_HDR{      // The RIFF chunk  descriptor 

    char           RIFF[4];       // ChunkID, "RIFF",  资源交换文件标志. big-endian 
    DWORD        ChunkSize;       // 从下个地址开始到文件结尾的字节数.   typedef unsigned int uint32  // 该字段的数值加 8 为当前文件的实际长度.
};    // 其余全部是 RIFF块内容  
    

  
struct CHUNK_HDR{      // The RIFF chunk  descriptor 

    char           Name[4];      // ChunkID,   资源交换文件标志. big-endian 
    DWORD        ChunkSize;       // 从下个地址开始到文件结尾的字节数.   typedef unsigned int uint32  // 该字段的数值加 8 为当前文件的实际长度.
};  



// The "fmt" sub-chunk. 描述 "data" sub-chunk 中数据的性质 
struct FMT_HDR{
    char         WAVE[4];          // Format, "WAVE" 文件标识  big-endian 
    char         fmt[4];           // SubchunkID, "fmt ", 波形格式标识     big-endian 
    DWORD        ChunkSize;        // SubCk1Size, 过滤字节,一般为16=00000010H, 或18=12H, 若为12H, 则有附加信息 cbSize=ex_size.   
};


// "fmt " 块内容  

struct WAVEFMT{   // 比 WAVEFORMATEX 少了一个成员  cbSize.  比 多了成员  BitsPerSample
    WORD          wFormatTag;      // AudioFmt 格式种类.  值为1时表示PCM线性编码, 不压缩, 没有fact块.    typedef unsigned short int uint16; 
    WORD          nChannels;       // Channels 通道数. 单声道为1, 双声道为2.   
    DWORD         nSamplesPerSec;  // SampleRate 采样频率.   
    DWORD         nAvgBytesPerSec; // ByteRate 数据传输率, 每秒字节＝采样频率×每个样本字节数.   Byte率=采样频率*音频通道数*每次采样得到的样本位数/8, =SampleRate*channles * bit_samp / 8. 
    WORD          nBlockAlign;     // BlockAlign 块对齐字节数 = channles * bit_samp / 8.     

    WORD          BitsPerSample;   // bit_samp, 又称量化位数. 

//  WORD          cbSize;          // ex_size 扩展块的大小, 可无.  the count in bytes of the size of   extra information (after cbSize)  
};



// 扩展块 -- 可选, 故使用 占位符 函数  cph=content placeholder  

// 有两种方法指明不使用扩展块：
    //    无 ex_size 字段,  但 fmt 块中的 SubCk1Size 字段为 16 = 0x10. 
    //    有 ex_size, (此时 SubCk1Size 字段为 0x12>16.)  设置其值为0.
// 测试发现 ex_size 意义含糊, 不明白什么意思. 倒是 SubCk1Size>0x10 ←→ 有 fact. 


// 测试发现 SubCk1Size >0x10, 并且 cbSize>0,  此时 cbSize 才真正表示  the size of extra information (after cbSize) 



// 只能用于读取数据.  写数据也可以参考该函数.
// int cph_fact_chunk( void*stream, int(* fptr)(void *buf, int si, int ci, void *stream )  ){   // 函数指针 参数示例 FILE*fh,  fptr = fread, or fwrite    //    

// fptr 示例  size_t fread( void *buffer, size_t size, size_t count, FILE *stream ) 
//            size_t fwrite( const void *buffer, size_t size, size_t count, FILE *stream ) 

#if FACT_CHUNK|0    

WORD          cbSize;              // ex_size 扩展块的大小, 可无.  the count in bytes of the size of   extra information (after cbSize)  
char          fact[4];             // "fact"  
DWORD         factSize;            // 附加块大小 
DWORD         dwSamples;           // 采样总数 (每个声道)

#endif    // FACT_CHUNK

inline
int cph_fact( FILE*stream, void*buf){   // 函数指针 参数示例 FILE*fh,  fptr = fread, or fwrite    //    
BYTE*ptr=(BYTE*)buf;  

WORD          cbSize;              // ex_size 扩展块的大小, 可无.  the count in bytes of the size of   extra information (after cbSize)  

int ic= fread(&cbSize, sizeof(cbSize),1,stream);
*(WORD*)ptr=cbSize;   ptr+=sizeof(WORD);  


// The "fact" sub-chunk. 压缩格式需要扩展块.  
char            fact[4];         // "fact"  
ic+= fread(fact, 4,1,stream);  memcpy(ptr,fact,4);   ptr+=4;    


DWORD          factSize;         // 附加块大小 
ic+= fread(&factSize, sizeof(DWORD),1,stream);  *(DWORD*)ptr=factSize;   ptr+=sizeof(DWORD);  
    

DWORD          dwSamples;        // 采样总数 (每个声道)
ic+= fread(&dwSamples, sizeof(DWORD),1,stream);  *(DWORD*)ptr=dwSamples;   ptr+=sizeof(DWORD);  


// 后面还有数据. 大小与 cbSize  有关. 

// ...... 


return ic; 
}
 
// The "data" sub-chunk. raw sound data  
struct DATA_CHUNK{
    char             data[4];          // Subchunk2ID, "data". big 
    DWORD          chunkSize;          //SubCk2Size,  =dwSamples*Channels*BitsPerSample/8.   little. 
};


//  BYTE*          DATA;                // of  SubCk2Size 
inline 
int cph_data( FILE*stream, int chunkSize, void*buf){ return fread(buf, chunkSize,1,stream); }

// 还可能有其他块  ......  

#endif    // WAV_FILE_CONTENT 













#if  WAV_FILE|1    // wav 文件结构


typedef
class _FILE_WAV {     // 默认 little-endian  
public:

    
struct WAV_HDR{      // The RIFF chunk  descriptor 

    char            RIFF[4];      // ChunkID, "RIFF",  资源交换文件标志. big-endian 
    DWORD        ChunkSize;       // 从下个地址开始到文件结尾的字节数.   typedef unsigned int uint32  // 该字段的数值加 8 为当前文件的实际长度.
    char            WAVE[4];      // Format, “WAVE” 文件标识  big-endian 

} wav_hdr;  
    


// The "fmt" sub-chunk. 描述 "data" sub-chunk 中数据的性质 
struct FMT_CHUNK{
    char         fmt[4];           // SubchunkID, "fmt ", 波形格式标识     big-endian 
    DWORD        ChunkSize;        // SubCk1Size, 过滤字节,一般为16=00000010H, 或18=12H, 若为12H, 则有附加信息 cbSize=ex_size.   

struct WAVEFMT{
    WORD          wFormatTag;      // AudioFmt 格式种类.  值为1时表示PCM线性编码, 不压缩, 没有fact块.    typedef unsigned short int uint16; 
    WORD          nChannels;       // Channels 通道数. 单声道为1, 双声道为2.   
    DWORD         nSamplesPerSec;  // SampleRate 采样频率.   
    DWORD         nAvgBytesPerSec; // ByteRate 数据传输率, 每秒字节＝采样频率×每个样本字节数.   Byte率=采样频率*音频通道数*每次采样得到的样本位数/8, =SampleRate*channles * bit_samp / 8. 
    WORD          nBlockAlign;     // BlockAlign 块对齐字节数 = channles * bit_samp / 8.     

    WORD          BitsPerSample;   // bit_samp, 又称量化位数.    
}wf;
//...共36字节 
} fmt_chunk;  



// 扩展块 -- 可选, 故使用 占位符 函数  cph=content placeholder  

// 有两种方法指明不使用扩展块：
    //    无 ex_size 字段,  但 fmt 块中的 SubCk1Size 字段为 16 = 0x10. 
    //    有 ex_size, (此时 SubCk1Size 字段为 0x12>16.)  设置其值为0.
// 测试发现 ex_size 意义含糊, 不明白什么意思. 倒是 SubCk1Size>0x10 ←→ 有 fact. 



// 只能用于读取数据.  写数据也可以参考该函数.
// int cph_fact_chunk( void*stream, int(* fptr)(void *buf, int si, int ci, void *stream )  ){   // 函数指针 参数示例 FILE*fh,  fptr = fread, or fwrite    //    

// fptr 示例  size_t fread( void *buffer, size_t size, size_t count, FILE *stream ) 
//            size_t fwrite( const void *buffer, size_t size, size_t count, FILE *stream ) 

#if FACT_CHUNK|0    

WORD          cbSize;              // ex_size 扩展块的大小, 可无.  the count in bytes of the size of   extra information (after cbSize)  
char          fact[4];             // "fact"  
DWORD         factSize;            // 附加块大小 
DWORD         dwSamples;           // 采样总数 (每个声道)

#endif    // FACT_CHUNK

int cph_fact( FILE*stream, void*buf){   // 函数指针 参数示例 FILE*fh,  fptr = fread, or fwrite    //    
BYTE*ptr=(BYTE*)buf;  

WORD          cbSize;              // ex_size 扩展块的大小, 可无.  the count in bytes of the size of   extra information (after cbSize)  

int ic= fread(&cbSize, sizeof(cbSize),1,stream);
*(WORD*)ptr=cbSize;   ptr+=sizeof(WORD);  


// The "fact" sub-chunk. 压缩格式需要扩展块.  
char            fact[4];         // "fact"  
ic+= fread(fact, 4,1,stream);  memcpy(ptr,fact,4);   ptr+=4;    


DWORD          factSize;         // 附加块大小 
ic+= fread(&factSize, sizeof(DWORD),1,stream);  *(DWORD*)ptr=factSize;   ptr+=sizeof(DWORD);  
    

DWORD          dwSamples;        // 采样总数 (每个声道)
ic+= fread(&dwSamples, sizeof(DWORD),1,stream);  *(DWORD*)ptr=dwSamples;   ptr+=sizeof(DWORD);  


// 后面还有数据. 大小与 cbSize  有关. 

// ...... 


return ic; 
}
 
// The "data" sub-chunk. raw sound data  
struct DATA_CHUNK{
    char             data[4];          // Subchunk2ID, "data". big 
    DWORD          chunkSize;          //SubCk2Size,  =dwSamples*Channels*BitsPerSample/8.   little. 
} data_chunk;


//  BYTE*          DATA;                // of  SubCk2Size 
int cph_data( FILE*stream, void*buf){ return fread(buf, data_chunk.chunkSize,1,stream); }


}FILE_WAV;   //  class FILE_WAV  



#endif    //  WAV_FILE











// template 必须象 class 一样定义, 很奇怪! 

// drawSeries, 绘制函数曲线. 与 drawFunc 类似, 只是函数定义域为整数, 步长dx,dy一般与网格宽度保持一致, 参见 drawAxis  
// fptr, 函数指针 double (*FUNCPTR)(double*,int iv=1).   drawDiscreate 的影响因素: MapMode以及WorldTransform.  
//template<class T> void drawDiscrete(HDC hdc, T fptr, double dx=1,  double dy=1, RECT*pr=0L);

// drawFunc, 绘制函数曲线. 受initGdi影响与drawAxis一致.
// fptr, 函数指针 double (*FUNCPTR)(double*,int iv=1).   drawFunc 的影响因素: MapMode以及WorldTransform.  

template<class T> void drawSeries(HDC hdc, T fptr, double dx, double dy, RECT*prc){  // =1 =1 =0L
if(!fptr) return;  if(dx==0) dx=1;  if(dy==0) dy=1; 

T f=(T)fptr;  // typedef double (*FUNCPTR)(double); // FUNCPTR f=(FUNCPTR)fptr;   

HFONT hFont=(HFONT)SelectObject(hdc,GetStockObject(DEFAULT_GUI_FONT));  // 较小的 Font  

static HPEN hpen;  hpen=(HPEN)SelectObject(hdc,GetStockObject(DC_PEN));  
SetDCPenColor(hdc, 0xFF0000);  // 前景画笔颜色 -- 函数曲线 


static RECT rc={0,0,0,0,};  if(prc) rc=*prc; 
if(rc.right==0) GetClientRect(WindowFromDC(hdc),&rc); 

int w=rc.right, h=rc.bottom; 
int cx=w/2, cy=h/2, B=40;   // (xo,yo)是窗口中点, B 是默认边界值.  
int iw=w-2*B, ih=h-2*B;  
dx*=ih/10, dy*=ih/10;  // 绘制网格, 默认 1/10 窗口高度, 保留少许边界 --- drawFunc要保持同样单位值 

POINT p[5]={ {B,cy},{w-B, cy},   {cx,B},{cx,h-B}, {cx,cy}, };  DPtoLP(hdc,p,5);  // 窗口范围, 转化到世界空间 

int xo=p[0].x, xr=p[1].x;   

int x=xo, xR=xr; if(x>xR) x=xR,xR=xo;  x-=fmod(x,dx); 
double x0=x, y0=f(x0/dx), y; 

/*
while(x<=xR){ //   细化: 每一步绘制 5 段:  x+=dx ==>  x+=dx/5; 
y=f(x/dx);   SetPixel(hdc, x, y*dy, 0x0000FF);   // 每步绘制 100 个像素:  x+=dx ==>  x+=dx/100; 
// MoveToEx(hdc, x0,y0*dy,0L);  LineTo(hdc, x, y*dy); 
x0=x; y0=y; 
x+=dx/10;   // 每一步绘制 5 段:  x+=dx ==>  x+=dx/5; 
}  // while(x<=X) 
*/

SetDCPenColor(hdc, 0x00BB00);   // SelectObject(hdc,hBkPen);  // 标注文字 .   
SetBkMode(hdc,TRANSPARENT); SetTextColor(hdc,0x8080B0); 

static char sz[128]; int ic;  
x=xo, xR=xr; if(x>xR) x=xR,xR=xo;  x-=fmod(x,dx); 

 
while(x<=xR){  
y=f(x/dx);   SetPixel(hdc, x, y*dy, 0x0000FF);   // 每一步绘制 100 个像素:  x+=dx ==>  x+=dx/100; 
ic=sprintf(sz,"%.2f",y); TextOut(hdc,x,y*dy,sz,ic);  // 函数值显示时的缩放比例dy 
ic=sprintf(sz,"%.0f",x/dx); TextOut(hdc,x,0,sz,ic);
Rectangle(hdc, x, y*dy, x+3, y*dy+3); // Ellipse(hdc, x, y*dy, x+4, y*dy+4);  // SetPixel(hdc, x, y*dy+0.5, 0x0000FF);   
x+=dx;  
}  // while(x<=xR) 
 
SelectObject(hdc,hpen); 
SelectObject(hdc,hFont);
}




typedef struct ChunkHdr {
char         ckID[4];   
DWORD        ckSize; 
// BYTE*  ckData;        // ckSize Bytes.   
} CHUNKHDR;








// 确定不变的头部. 便于读写 wav 文件.

typedef class WavHeader {   // 36 字节

public:

    // The RIFF chunk  descriptor 
    char            RIFF[4];      // ChunkID, "RIFF",  资源交换文件标志. big 
    DWORD        ChunkSize;       // 从下个地址开始到文件结尾的字节数. little.  typedef unsigned int uint32;
                                  // 该字段的数值加 8 为当前文件的实际长度. 如果小于文件长度, 则 "data" 后有其他块. 
    char            WAVE[4];      // Format, “WAVE” 文件标识 big


// The "fmt" sub-chunk. 描述 "data" sub-chunk 中数据的性质 
    char         fmt[4];          // SubchunkID, "fmt ", 波形格式标识     big
    DWORD        SubCk1Size;      // 过滤字节,一般为 16=00000010H, 或18=12H, 若为12H,则有附加信息 ex_size.  little

    WORD         AudioFmt;        // 格式种类.  值为1时表示PCM线性编码, 不压缩, 没有fact块.  little. typedef unsigned short int uint16; 
    WORD         Channels;        // 通道数. 单声道为1, 双声道为2.  little.
    DWORD        SampleRate;      // 采样频率. little. 
    DWORD        ByteRate;        // 数据传输率, 每秒字节. little.  Byte率=SampleRate*channles * bit_samp / 8. 
    WORD         BlockAlign;      // 块对齐字节数 = channles * bit_samp / 8. little.   
    WORD         BitsPerSample;   // bit_samp, 又称量化位数. little.   


public:

    
WavHeader(){

    memset(this, 0, sizeof(WavHeader));  memcpy(RIFF, "RIFF", 4); 
    memcpy(WAVE, "WAVE", 4);             memcpy(fmt, "fmt ", 4);  
    

SubCk1Size = 0x10;
    AudioFmt = 1;
    Channels=1;  

    SampleRate=0x1F40;    // =8000, 以毫秒位单位, 则  SampleRate = 8  

    BitsPerSample =8;     // 也有 10 位的 

// 计算 ......
    ByteRate = SampleRate * Channels * BitsPerSample / 8;  
    BlockAlign = Channels * BitsPerSample / 8;  
} 

~WavHeader() {}  

} WAVHDR;








// 与声音有关的三个参数

// (1)采样频率: 又称取样频率, 是单位时间内的采样次数. 根据奎特采样定理, 要从采样中完全恢复原始信号的波形, 采样频率要高于声音中最高频率的两倍。人耳可听到的声音的频率范围是在16Hz - 20kHz之间。因此, 要将听到的原声音真实地还原出来, 采样频率必须大于4 0k H z 。常用的采样频率有8 k H z 、1 1 . 02 5 k H z 、22.05kHz、44.1kHz、48kHz等几种。22.05KHz相当于普通FM广播的音质, 44.1KHz理论上可达到CD的音质. 对于高于48KHz的采样频率人耳很难分辨, 没有实际意义.
// (2)采样位数: 也叫量化位数(单位 比特), 是存储每个采样值所用的二进制位数. 常用的量化位数有4、8、12、16、24. 量化位数与声卡的位数和编码有关.如果采用PCM编码同时使用8 位声卡, 可将音频信号幅度从上限到下限化分成256个音量等级, 取值范围为0 - 255; 使用16位声卡, 可将音频信号幅度划分成了64K个音量等级, 取值范围为 - 32768至32767。
// (3)声道数: 是使用的声音通道的个数, 也是采样时所产生的声音波形的个数. 单声道, 每次产生一个波形的数据, 双声道, 每次产生两个波形的数据, 所占的存储空间增加一倍.



typedef
class WavData {  // WAV文件最重要的三个部分  

public:
WORD          Channels;          // 通道数. 单声道为1, 双声道为2.  little.
DWORD         SampleRate;        // freq, SamplePerSecond,  frequency, 采样频率
// DWORD      ByteRate;          // bps, Byte率, ByteRate, BytesPerSecond, 数据传输率, 每秒字节＝采样频率×每个样本字节数
// WORD       BlockAlign;        // 块对齐字节数 = channles * bit_samp / 8. little.   
WORD          BitsPerSample;     // bit_samp, sample_num_bit, BitsPerSample, 一个样本的位数, 又称量化位数.     
DWORD         dataSize;          // SubCK2Size, 数据大小, =dwSamples*Channels*BitsPerSample/8. 
BYTE*         ps;     // sound data, 频数据 ,这里要定义什么就看样本位数了, 我这里只是单纯的复制数据

// 通过 malloc、realloc、free 管理内存 ps.

public:
WavData(){ Channels=1; SampleRate=8000; BitsPerSample=8;  ps=0; }
~WavData(){if(ps){ free(ps); ps=0; }  } 

}WAVDATA;








bool wav_read(const char*szFile, WAVDATA&wd);



int get_sd(int i, WavData*wd, int cs = 0); 



#if CSDN_CODE   

// https://blog.csdn.net/Hamlee67/article/details/82969170 

// https://blog.csdn.net/meicheng777/article/details/52449443  WAV文件的读写 

// https://blog.csdn.n et/ininw/article/details/70195934  WAV文件分析  

// https://blog.csdn.net/vacu_um/article/details/70195761  WAV文件格式解析  




// WAV 文件结构
// RIFF块
// WAVE FOURCC
// fmt 块
// fact 块（可选）
// data块（包含PCM数据）




#if FOR_REFERENCE






// RIFF头chunk表示如下 

struct RIFF_HEADER{
TCHAR szRiffID[4];        // 'R','I','F','F'
DWORD dwRiffSize;
TCHAR szRiffFormat[4];    // 'W','A','V','E'
};


// 第二个块是fmt chunk, 它用来描述WAVE文件的特性, 例如比特率、声道数  
// fmt块的主题数据.  

struct WAVE_FORMAT {
WORD wFormatTag;
WORD wChannels;
DWORD dwSamplesPerSec;
DWORD dwAvgBytesPerSec;
WORD wBlockAlign;
WORD wBitsPerSample;

 WORD ex_size;         // uint16_t, 扩展块的大小, 附加块的大小. 有两种方法来设置不使用扩展块.一种是设置fmt中的size字段为16（无ex_size字段）；或者，有ex_size,设置其值为0.在本文中，使用第二种方法，设置ex_size的值为0，不使用扩展块。

};



//  描述包含实际声音数据的 data chunk 

struct FMT_BLOCK{
TCHAR szFmtID[4];    // 'f','m','t',' ' please note the  pace character at the fourth location.
DWORD dwFmtSize;
WAVE_FORMAT wavFormat;
};


// 以上就是一个WAV文件的三个最基本的chunk
// 也可以有很多可选chunk位于fmt block和data block之间
// 下面是一个可选chunk的例子 note chunk 

struct NOTE_CHUNK{
TCHAR ID[4];    // 'note'
long chunkSize;
long dwIdentifier;
TCHAR dwText[];
};







typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
 
// WAV符合RIFF(Resource Interchange File Format)文件规范.
// RIFF 文件头一系列数据块(data chunks).
// WAVE文件通常就是一个包含WAVE数据块的RIFF文件.
// 而这个WAVE数据块(chunk)由两个子数据块(sub-chunks)组成, "fmt" chunk 说明数据的结构,和"data" chunk 包含真正的音频数据. 
// wav文件去掉头就是snd文件了



typedef struct WaveHeader{ 

//  RIFF_HEADER ......

BYTE  RIFF[4];                 // ChunkID,      ="RIFF", 资源交换文件标志 
DWORD RiffSize;                // ChunkSize,    从下个地址开始到文件结尾的字节数; 
BYTE  WAVE[4];                 // Format,       ="WAVE", wave文件标识; 

// The "fmt" SubChunk ......

BYTE  fmt[4];              // SubChunk1ID,        ="fmt", 波形格式标识 
DWORD fmtSize;             // SubChunk1Size,      过滤字节,一般为00000010H, 若为00000012H则说明数据头携带附加信息
WORD  fmtFlag;             // AudioFormat,        格式种类,  值为1时,  表示PCM线性编码  
WORD  channels;            // NumChannels,        通道数, 单声道为1, 双声道为2  
		uint32 samp_freq;           // SampleRate,         采样频率  
		uint32 byte_rate;           // ByteRate,           数据传输率 (每秒字节＝采样频率×每个样本字节数)  
		uint16 block_align;         // BlockAlign,         块对齐字节数 = channles * bit_samp / 8  
		uint16 bit_samp;            // BitsPerSample,      bits per sample (又称量化位数)  


// The Data SubChunk ......  

// SubChunk2ID;             //  ="data"    
// SubChunk2Size;           //  data的总数据字节长度，也是剩余数据的大小
// Data;                    // 真实的音频数据（actual sound data） 

	
} wave_header_t; 
 


typedef struct WaveStruct { 
		FILE *fp;                    // file pointer  


		wave_header_t* pHeader;      // header  

// 
		uint8 data_flag[4];          // 数据标识符 ;
		uint32 length;               // 采样数据总数 ;
		uint32 *pData;               // data ;
} wave_t; 


 



#endif  // FOR_REFERENCE








#endif  // CSDN_CODE






typedef
class _WAVFILE {     // 默认 little-endian  
public:

    
// The RIFF chunk  descriptor 
    char            RIFF[4];      // ChunkID, "RIFF",  资源交换文件标志. big-endian 
    DWORD        ChunkSize;       // 从下个地址开始到文件结尾的字节数. little.  typedef unsigned int uint32;
                                  // 该字段的数值加 8 为当前文件的实际长度.
    char            WAVE[4];      // Format, “WAVE” 文件标识 big

    
// The "fmt" sub-chunk. 描述 "data" sub-chunk 中数据的性质 
    char            fmt[4];       // SubchunkID, "fmt ", 波形格式标识     big
    DWORD        SubCk1Size;      // 过滤字节,一般为16=00000010H, 或18=12H, 若为12H,则有附加信息ex_size.  little
    WORD          AudioFmt;       // 格式种类.  值为1时表示PCM线性编码, 不压缩, 没有fact块.  little. typedef unsigned short int uint16; 
    WORD          Channels;       // 通道数. 单声道为1, 双声道为2.  little.

    DWORD        SampleRate;      // 采样频率. little. 
 
    DWORD        ByteRate;        // 数据传输率,每秒字节＝采样频率×每个样本字节数. little.  
                                  // Byte率=采样频率*音频通道数*每次采样得到的样本位数/8, =SampleRate*channles * bit_samp / 8. 


    WORD          BlockAlign;     // 块对齐字节数 = channles * bit_samp / 8. little.   
    WORD          BitsPerSample;  // bit_samp, 又称量化位数. little.   

//...共36字节 


    WORD          ex_size;         //  扩展块的大小, 可无.  




    // 有两种方法指明不使用扩展块：
    //    无 ex_size 字段,  但 fmt 块中的 SubCk1Size 字段为 16 = 0x10. 
    //    有 ex_size, (此时 SubCk1Size 字段为 0x12>16.)  设置其值为0.
// 测试发现 ex_size 意义含糊, 不明白什么意思. 倒是 SubCk1Size>0x10 ←→ 有 fact. 



    
// The "fact" sub-chunk. 压缩格式需要扩展块.  
    char            fact[4];         // "fact"  
    DWORD          factSize;         // 附加块大小 
    DWORD          dwSamples;        // 采样总数 (每个声道)
    





// The "data" sub-chunk. raw sound data  

    char             data[4];          // Subchunk2ID, "data". big 
    DWORD          SubCk2Size;         // =dwSamples*Channels*BitsPerSample/8.   little. 


    BYTE*          DATA;                // of  SubCk2Size 


public:

_WAVFILE() {
memset(this, 0, sizeof(_WAVFILE));  memcpy(RIFF, "RIFF", 4); memcpy(WAVE, "WAVE", 4);
memcpy(fmt, "fmt ", 4);  memcpy(data, "data", 4);
DATA=0;  
SubCk1Size=16; 
AudioFmt = 1;
ex_size = 0;
}

~_WAVFILE() {
if(DATA){ delete[] DATA; DATA=0; }
}


} WavFile, wave_t;   //  class WAVFILE  




void t_read(const char*wavf="M1F1-Alaw-AFsp.wav");
void t_write(const char*wavf = "儿童乐园.wav");


void t_wavcx(const char*wavf = "儿童乐园.wav");







BOOL PlayWav(char* soundFileName); 











#if WAV_TESTS|1

inline 
void t_wav(){

qTrace( "sizeof(WAVEFORMATE) = 0x%X\r\n",  sizeof(WAVEFORMATE) );    // 0x10   
qTrace("sizeof(WAVEFORMATEX) = 0x%X\r\n%d -- %d    %d    %d",  sizeof(WAVEFORMATEX),   sizeof(_FILE_WAV), sizeof(_WAVFILE),    sizeof(WAVEFMT),  sizeof(WORD) );  


}



#endif   //  WAV_TESTS   






void* wavRead(const char*szFile, void*diw=0);  
int   wavWrite(DIW*pdiw, const char*szFile=0, bool DefaultFmt=false);  


int wavMix(const char* f1, const char* f2,const char* f3=0); 



void t_rw(const char*wavf = "wavFile/我爱你中国.wav");


void t_mix();


#endif  // _WAVFILE_H_ //




