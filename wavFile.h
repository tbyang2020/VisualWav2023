



// WAVFILE.h
#ifndef _WAVFILE_H_
#define _WAVFILE_H_    //#endif  // _WAVFILE_H_ //


#include "../Interface2D/qTrace.h"



// �ṹ����WAVFILE.CPP��ʵ��
#ifdef  _WAVFILE_CPP_    //#endif  // _WAVFILE_CPP_ //
#define WAVFILE_EXTERN 
#else 
#define WAVFILE_EXTERN extern 
#endif  // _WAVFILE_CPP_ //

#pragma pack(show)        // ��ʾĬ�϶���ֵ 8 
#pragma pack(1)           // ������  


// WAVEFORMATEX ,  ���� ���˳�Ա cbSize. �� WAVEFORMAT ��һ����Ա  wBitsPerSample 
struct WAVEFORMATE{

// WAVEFORMATEX ,  ���� ���˳�Ա cbSize. �� WAVEFORMAT ��һ����Ա  wBitsPerSample 
    WORD        wFormatTag = WAVE_FORMAT_PCM;         // format type ��ʽ����.  ֵΪ WAVE_FORMAT_PCM=1 ʱ��ʾ PCM ���Ա���, ��ѹ��, û��fact��. typedef unsigned short int uint16; 
    WORD        nChannels = 1;                        // number of channels (i.e. mono, stereo...) ͨ����. ������Ϊ1, ˫����Ϊ2.
    DWORD       nSamplesPerSec = 11025;               // sample rate ����Ƶ��. 
    DWORD       nAvgBytesPerSec = 11025;              // for buffer estimation ���ݴ�����, ÿ���ֽ�, Byte�� = nSamplesPerSec*nChannels * wBitsPerSample / 8. 
    WORD        nBlockAlign = 1;                      // block size of data  ������ֽ��� = nChannels * wBitsPerSample / 8.    

    WORD        wBitsPerSample =8;                    // number of bits per sample of mono data �ֳ�����λ�� 
//  WORD        cbSize = 0;                           // the count in bytes of the size of extra information (after cbSize) ��չ��Ϣ��С  

};


// ȷ�������ͷ��. ���ڶ�д wav �ļ�. Ĭ�� little-endian. 
// IFF(Interchange File Format) ��׼ʹ�ô��.  RIFFʹ��С��  little-endian (x86) 

struct WAV_HDR {   //  Ĭ��ֵ. �� riffSize �� dataSize �й�,  = �ļ�����-8.

// The RIFF chunk  descriptor 
    char         RIFF[4]={'R','I','F','F',};   // "RIFF",  Resource Interchange File Format ��Դ�����ļ���־. big-endian 
    DWORD        riffSize = 0;                 // ���¸���ַ��ʼ���ļ���β���ֽ���.  typedef unsigned int uint32; // ���ֶε���ֵ�� 8 Ϊ��ǰ�ļ���ʵ�ʳ���. ���С���ļ�����, �� "data" ����������. 
    char         WAVE[4]={'W','A','V','E',};   // Format "WAVE" �ļ���ʶ big-endian


// The "fmt" sub-chunk. ���� "data" sub-chunk �����ݵ����� 
    char         fmt[4]  = {'f','m','t',' ',};    // SubchunkID, "fmt ", ���θ�ʽ��ʶ     big
    DWORD        fmtSize = 0x10;                  // �����ֽ�,һ��Ϊ 16=00000010H, ��18=12H, ��Ϊ12H,���и�����Ϣ cbSize.   

// WAVEFORMATEX ,  ���� ���˳�Ա cbSize. �� WAVEFORMAT ��һ����Ա  wBitsPerSample 
    WORD        wFormatTag = WAVE_FORMAT_PCM;         // format type ��ʽ����.  ֵΪ1ʱ��ʾ PCM ���Ա���, ��ѹ��, û��fact��. typedef unsigned short int uint16; 
    WORD        nChannels  = 1;                       // number of channels (i.e. mono, stereo...) ͨ����. ������Ϊ1, ˫����Ϊ2.
    DWORD       nSamplesPerSec  = 11025;              // sample rate ����Ƶ��. 
    DWORD       nAvgBytesPerSec = 11025;              // for buffer estimation ���ݴ�����, ÿ���ֽ�, Byte��=SampleRate*nChannels * wBitsPerSample / 8. 
    WORD        nBlockAlign = 1;                      // block size of data  ������ֽ��� = nChannels * wBitsPerSample / 8.    

    WORD        wBitsPerSample = 8;                   // number of bits per sample of mono data �ֳ�����λ�� 
//  WORD        cbSize = 0;                           // the count in bytes of the size of extra information (after cbSize) ��չ��Ϣ��С  

};   // WAV_HDR


// wav �ļ���Ҫ����.  ��ҪΪ waveOut# ��������:  waveOutOpen ������Ҫ���� WAVEFORMATEX, waveOutWrite ������Ҫ��������  
 
struct DIW{
WAVEFORMATEX  wfx;    // 5+ 2����Ա.  WAVEFORMAT  5����Ա  
DWORD dataSize;
BYTE* pData;  
};







// wav �ļ���������. ֻ�ܴ������, ��Ϊ RIFF (Resource Interchange File Format) ����֯����ܶ಻ͬ�Ŀ�.  
//  IFF(Interchange File Format) ��׼ʹ�ô��.  RIFFʹ��С��  little-endian (x86) 
// wav �ļ��ṹ�� wavRead �����н�һ������ -- qLog 

#if  WAV_FILE_CONTENT|1   // wav�ļ���3��������  RIFF��FORMAT��DATA. RIFF�����������.  
    
struct RIFF_HDR{      // The RIFF chunk  descriptor 

    char           RIFF[4];       // ChunkID, "RIFF",  ��Դ�����ļ���־. big-endian 
    DWORD        ChunkSize;       // ���¸���ַ��ʼ���ļ���β���ֽ���.   typedef unsigned int uint32  // ���ֶε���ֵ�� 8 Ϊ��ǰ�ļ���ʵ�ʳ���.
};    // ����ȫ���� RIFF������  
    

  
struct CHUNK_HDR{      // The RIFF chunk  descriptor 

    char           Name[4];      // ChunkID,   ��Դ�����ļ���־. big-endian 
    DWORD        ChunkSize;       // ���¸���ַ��ʼ���ļ���β���ֽ���.   typedef unsigned int uint32  // ���ֶε���ֵ�� 8 Ϊ��ǰ�ļ���ʵ�ʳ���.
};  



// The "fmt" sub-chunk. ���� "data" sub-chunk �����ݵ����� 
struct FMT_HDR{
    char         WAVE[4];          // Format, "WAVE" �ļ���ʶ  big-endian 
    char         fmt[4];           // SubchunkID, "fmt ", ���θ�ʽ��ʶ     big-endian 
    DWORD        ChunkSize;        // SubCk1Size, �����ֽ�,һ��Ϊ16=00000010H, ��18=12H, ��Ϊ12H, ���и�����Ϣ cbSize=ex_size.   
};


// "fmt " ������  

struct WAVEFMT{   // �� WAVEFORMATEX ����һ����Ա  cbSize.  �� ���˳�Ա  BitsPerSample
    WORD          wFormatTag;      // AudioFmt ��ʽ����.  ֵΪ1ʱ��ʾPCM���Ա���, ��ѹ��, û��fact��.    typedef unsigned short int uint16; 
    WORD          nChannels;       // Channels ͨ����. ������Ϊ1, ˫����Ϊ2.   
    DWORD         nSamplesPerSec;  // SampleRate ����Ƶ��.   
    DWORD         nAvgBytesPerSec; // ByteRate ���ݴ�����, ÿ���ֽڣ�����Ƶ�ʡ�ÿ�������ֽ���.   Byte��=����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8, =SampleRate*channles * bit_samp / 8. 
    WORD          nBlockAlign;     // BlockAlign ������ֽ��� = channles * bit_samp / 8.     

    WORD          BitsPerSample;   // bit_samp, �ֳ�����λ��. 

//  WORD          cbSize;          // ex_size ��չ��Ĵ�С, ����.  the count in bytes of the size of   extra information (after cbSize)  
};



// ��չ�� -- ��ѡ, ��ʹ�� ռλ�� ����  cph=content placeholder  

// �����ַ���ָ����ʹ����չ�飺
    //    �� ex_size �ֶ�,  �� fmt ���е� SubCk1Size �ֶ�Ϊ 16 = 0x10. 
    //    �� ex_size, (��ʱ SubCk1Size �ֶ�Ϊ 0x12>16.)  ������ֵΪ0.
// ���Է��� ex_size ���庬��, ������ʲô��˼. ���� SubCk1Size>0x10 ���� �� fact. 


// ���Է��� SubCk1Size >0x10, ���� cbSize>0,  ��ʱ cbSize ��������ʾ  the size of extra information (after cbSize) 



// ֻ�����ڶ�ȡ����.  д����Ҳ���Բο��ú���.
// int cph_fact_chunk( void*stream, int(* fptr)(void *buf, int si, int ci, void *stream )  ){   // ����ָ�� ����ʾ�� FILE*fh,  fptr = fread, or fwrite    //    

// fptr ʾ��  size_t fread( void *buffer, size_t size, size_t count, FILE *stream ) 
//            size_t fwrite( const void *buffer, size_t size, size_t count, FILE *stream ) 

#if FACT_CHUNK|0    

WORD          cbSize;              // ex_size ��չ��Ĵ�С, ����.  the count in bytes of the size of   extra information (after cbSize)  
char          fact[4];             // "fact"  
DWORD         factSize;            // ���ӿ��С 
DWORD         dwSamples;           // �������� (ÿ������)

#endif    // FACT_CHUNK

inline
int cph_fact( FILE*stream, void*buf){   // ����ָ�� ����ʾ�� FILE*fh,  fptr = fread, or fwrite    //    
BYTE*ptr=(BYTE*)buf;  

WORD          cbSize;              // ex_size ��չ��Ĵ�С, ����.  the count in bytes of the size of   extra information (after cbSize)  

int ic= fread(&cbSize, sizeof(cbSize),1,stream);
*(WORD*)ptr=cbSize;   ptr+=sizeof(WORD);  


// The "fact" sub-chunk. ѹ����ʽ��Ҫ��չ��.  
char            fact[4];         // "fact"  
ic+= fread(fact, 4,1,stream);  memcpy(ptr,fact,4);   ptr+=4;    


DWORD          factSize;         // ���ӿ��С 
ic+= fread(&factSize, sizeof(DWORD),1,stream);  *(DWORD*)ptr=factSize;   ptr+=sizeof(DWORD);  
    

DWORD          dwSamples;        // �������� (ÿ������)
ic+= fread(&dwSamples, sizeof(DWORD),1,stream);  *(DWORD*)ptr=dwSamples;   ptr+=sizeof(DWORD);  


// ���滹������. ��С�� cbSize  �й�. 

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

// ��������������  ......  

#endif    // WAV_FILE_CONTENT 













#if  WAV_FILE|1    // wav �ļ��ṹ


typedef
class _FILE_WAV {     // Ĭ�� little-endian  
public:

    
struct WAV_HDR{      // The RIFF chunk  descriptor 

    char            RIFF[4];      // ChunkID, "RIFF",  ��Դ�����ļ���־. big-endian 
    DWORD        ChunkSize;       // ���¸���ַ��ʼ���ļ���β���ֽ���.   typedef unsigned int uint32  // ���ֶε���ֵ�� 8 Ϊ��ǰ�ļ���ʵ�ʳ���.
    char            WAVE[4];      // Format, ��WAVE�� �ļ���ʶ  big-endian 

} wav_hdr;  
    


// The "fmt" sub-chunk. ���� "data" sub-chunk �����ݵ����� 
struct FMT_CHUNK{
    char         fmt[4];           // SubchunkID, "fmt ", ���θ�ʽ��ʶ     big-endian 
    DWORD        ChunkSize;        // SubCk1Size, �����ֽ�,һ��Ϊ16=00000010H, ��18=12H, ��Ϊ12H, ���и�����Ϣ cbSize=ex_size.   

struct WAVEFMT{
    WORD          wFormatTag;      // AudioFmt ��ʽ����.  ֵΪ1ʱ��ʾPCM���Ա���, ��ѹ��, û��fact��.    typedef unsigned short int uint16; 
    WORD          nChannels;       // Channels ͨ����. ������Ϊ1, ˫����Ϊ2.   
    DWORD         nSamplesPerSec;  // SampleRate ����Ƶ��.   
    DWORD         nAvgBytesPerSec; // ByteRate ���ݴ�����, ÿ���ֽڣ�����Ƶ�ʡ�ÿ�������ֽ���.   Byte��=����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8, =SampleRate*channles * bit_samp / 8. 
    WORD          nBlockAlign;     // BlockAlign ������ֽ��� = channles * bit_samp / 8.     

    WORD          BitsPerSample;   // bit_samp, �ֳ�����λ��.    
}wf;
//...��36�ֽ� 
} fmt_chunk;  



// ��չ�� -- ��ѡ, ��ʹ�� ռλ�� ����  cph=content placeholder  

// �����ַ���ָ����ʹ����չ�飺
    //    �� ex_size �ֶ�,  �� fmt ���е� SubCk1Size �ֶ�Ϊ 16 = 0x10. 
    //    �� ex_size, (��ʱ SubCk1Size �ֶ�Ϊ 0x12>16.)  ������ֵΪ0.
// ���Է��� ex_size ���庬��, ������ʲô��˼. ���� SubCk1Size>0x10 ���� �� fact. 



// ֻ�����ڶ�ȡ����.  д����Ҳ���Բο��ú���.
// int cph_fact_chunk( void*stream, int(* fptr)(void *buf, int si, int ci, void *stream )  ){   // ����ָ�� ����ʾ�� FILE*fh,  fptr = fread, or fwrite    //    

// fptr ʾ��  size_t fread( void *buffer, size_t size, size_t count, FILE *stream ) 
//            size_t fwrite( const void *buffer, size_t size, size_t count, FILE *stream ) 

#if FACT_CHUNK|0    

WORD          cbSize;              // ex_size ��չ��Ĵ�С, ����.  the count in bytes of the size of   extra information (after cbSize)  
char          fact[4];             // "fact"  
DWORD         factSize;            // ���ӿ��С 
DWORD         dwSamples;           // �������� (ÿ������)

#endif    // FACT_CHUNK

int cph_fact( FILE*stream, void*buf){   // ����ָ�� ����ʾ�� FILE*fh,  fptr = fread, or fwrite    //    
BYTE*ptr=(BYTE*)buf;  

WORD          cbSize;              // ex_size ��չ��Ĵ�С, ����.  the count in bytes of the size of   extra information (after cbSize)  

int ic= fread(&cbSize, sizeof(cbSize),1,stream);
*(WORD*)ptr=cbSize;   ptr+=sizeof(WORD);  


// The "fact" sub-chunk. ѹ����ʽ��Ҫ��չ��.  
char            fact[4];         // "fact"  
ic+= fread(fact, 4,1,stream);  memcpy(ptr,fact,4);   ptr+=4;    


DWORD          factSize;         // ���ӿ��С 
ic+= fread(&factSize, sizeof(DWORD),1,stream);  *(DWORD*)ptr=factSize;   ptr+=sizeof(DWORD);  
    

DWORD          dwSamples;        // �������� (ÿ������)
ic+= fread(&dwSamples, sizeof(DWORD),1,stream);  *(DWORD*)ptr=dwSamples;   ptr+=sizeof(DWORD);  


// ���滹������. ��С�� cbSize  �й�. 

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











// template ������ class һ������, �����! 

// drawSeries, ���ƺ�������. �� drawFunc ����, ֻ�Ǻ���������Ϊ����, ����dx,dyһ���������ȱ���һ��, �μ� drawAxis  
// fptr, ����ָ�� double (*FUNCPTR)(double*,int iv=1).   drawDiscreate ��Ӱ������: MapMode�Լ�WorldTransform.  
//template<class T> void drawDiscrete(HDC hdc, T fptr, double dx=1,  double dy=1, RECT*pr=0L);

// drawFunc, ���ƺ�������. ��initGdiӰ����drawAxisһ��.
// fptr, ����ָ�� double (*FUNCPTR)(double*,int iv=1).   drawFunc ��Ӱ������: MapMode�Լ�WorldTransform.  

template<class T> void drawSeries(HDC hdc, T fptr, double dx, double dy, RECT*prc){  // =1 =1 =0L
if(!fptr) return;  if(dx==0) dx=1;  if(dy==0) dy=1; 

T f=(T)fptr;  // typedef double (*FUNCPTR)(double); // FUNCPTR f=(FUNCPTR)fptr;   

HFONT hFont=(HFONT)SelectObject(hdc,GetStockObject(DEFAULT_GUI_FONT));  // ��С�� Font  

static HPEN hpen;  hpen=(HPEN)SelectObject(hdc,GetStockObject(DC_PEN));  
SetDCPenColor(hdc, 0xFF0000);  // ǰ��������ɫ -- �������� 


static RECT rc={0,0,0,0,};  if(prc) rc=*prc; 
if(rc.right==0) GetClientRect(WindowFromDC(hdc),&rc); 

int w=rc.right, h=rc.bottom; 
int cx=w/2, cy=h/2, B=40;   // (xo,yo)�Ǵ����е�, B ��Ĭ�ϱ߽�ֵ.  
int iw=w-2*B, ih=h-2*B;  
dx*=ih/10, dy*=ih/10;  // ��������, Ĭ�� 1/10 ���ڸ߶�, ��������߽� --- drawFuncҪ����ͬ����λֵ 

POINT p[5]={ {B,cy},{w-B, cy},   {cx,B},{cx,h-B}, {cx,cy}, };  DPtoLP(hdc,p,5);  // ���ڷ�Χ, ת��������ռ� 

int xo=p[0].x, xr=p[1].x;   

int x=xo, xR=xr; if(x>xR) x=xR,xR=xo;  x-=fmod(x,dx); 
double x0=x, y0=f(x0/dx), y; 

/*
while(x<=xR){ //   ϸ��: ÿһ������ 5 ��:  x+=dx ==>  x+=dx/5; 
y=f(x/dx);   SetPixel(hdc, x, y*dy, 0x0000FF);   // ÿ������ 100 ������:  x+=dx ==>  x+=dx/100; 
// MoveToEx(hdc, x0,y0*dy,0L);  LineTo(hdc, x, y*dy); 
x0=x; y0=y; 
x+=dx/10;   // ÿһ������ 5 ��:  x+=dx ==>  x+=dx/5; 
}  // while(x<=X) 
*/

SetDCPenColor(hdc, 0x00BB00);   // SelectObject(hdc,hBkPen);  // ��ע���� .   
SetBkMode(hdc,TRANSPARENT); SetTextColor(hdc,0x8080B0); 

static char sz[128]; int ic;  
x=xo, xR=xr; if(x>xR) x=xR,xR=xo;  x-=fmod(x,dx); 

 
while(x<=xR){  
y=f(x/dx);   SetPixel(hdc, x, y*dy, 0x0000FF);   // ÿһ������ 100 ������:  x+=dx ==>  x+=dx/100; 
ic=sprintf(sz,"%.2f",y); TextOut(hdc,x,y*dy,sz,ic);  // ����ֵ��ʾʱ�����ű���dy 
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








// ȷ�������ͷ��. ���ڶ�д wav �ļ�.

typedef class WavHeader {   // 36 �ֽ�

public:

    // The RIFF chunk  descriptor 
    char            RIFF[4];      // ChunkID, "RIFF",  ��Դ�����ļ���־. big 
    DWORD        ChunkSize;       // ���¸���ַ��ʼ���ļ���β���ֽ���. little.  typedef unsigned int uint32;
                                  // ���ֶε���ֵ�� 8 Ϊ��ǰ�ļ���ʵ�ʳ���. ���С���ļ�����, �� "data" ����������. 
    char            WAVE[4];      // Format, ��WAVE�� �ļ���ʶ big


// The "fmt" sub-chunk. ���� "data" sub-chunk �����ݵ����� 
    char         fmt[4];          // SubchunkID, "fmt ", ���θ�ʽ��ʶ     big
    DWORD        SubCk1Size;      // �����ֽ�,һ��Ϊ 16=00000010H, ��18=12H, ��Ϊ12H,���и�����Ϣ ex_size.  little

    WORD         AudioFmt;        // ��ʽ����.  ֵΪ1ʱ��ʾPCM���Ա���, ��ѹ��, û��fact��.  little. typedef unsigned short int uint16; 
    WORD         Channels;        // ͨ����. ������Ϊ1, ˫����Ϊ2.  little.
    DWORD        SampleRate;      // ����Ƶ��. little. 
    DWORD        ByteRate;        // ���ݴ�����, ÿ���ֽ�. little.  Byte��=SampleRate*channles * bit_samp / 8. 
    WORD         BlockAlign;      // ������ֽ��� = channles * bit_samp / 8. little.   
    WORD         BitsPerSample;   // bit_samp, �ֳ�����λ��. little.   


public:

    
WavHeader(){

    memset(this, 0, sizeof(WavHeader));  memcpy(RIFF, "RIFF", 4); 
    memcpy(WAVE, "WAVE", 4);             memcpy(fmt, "fmt ", 4);  
    

SubCk1Size = 0x10;
    AudioFmt = 1;
    Channels=1;  

    SampleRate=0x1F40;    // =8000, �Ժ���λ��λ, ��  SampleRate = 8  

    BitsPerSample =8;     // Ҳ�� 10 λ�� 

// ���� ......
    ByteRate = SampleRate * Channels * BitsPerSample / 8;  
    BlockAlign = Channels * BitsPerSample / 8;  
} 

~WavHeader() {}  

} WAVHDR;








// �������йص���������

// (1)����Ƶ��: �ֳ�ȡ��Ƶ��, �ǵ�λʱ���ڵĲ�������. ���ݿ��ز�������, Ҫ�Ӳ�������ȫ�ָ�ԭʼ�źŵĲ���, ����Ƶ��Ҫ�������������Ƶ�ʵ��������˶���������������Ƶ�ʷ�Χ����16Hz - 20kHz֮�䡣���, Ҫ��������ԭ������ʵ�ػ�ԭ����, ����Ƶ�ʱ������4 0k H z �����õĲ���Ƶ����8 k H z ��1 1 . 02 5 k H z ��22.05kHz��44.1kHz��48kHz�ȼ��֡�22.05KHz�൱����ͨFM�㲥������, 44.1KHz�����ϿɴﵽCD������. ���ڸ���48KHz�Ĳ���Ƶ���˶����ѷֱ�, û��ʵ������.
// (2)����λ��: Ҳ������λ��(��λ ����), �Ǵ洢ÿ������ֵ���õĶ�����λ��. ���õ�����λ����4��8��12��16��24. ����λ����������λ���ͱ����й�.�������PCM����ͬʱʹ��8 λ����, �ɽ���Ƶ�źŷ��ȴ����޵����޻��ֳ�256�������ȼ�, ȡֵ��ΧΪ0 - 255; ʹ��16λ����, �ɽ���Ƶ�źŷ��Ȼ��ֳ���64K�������ȼ�, ȡֵ��ΧΪ - 32768��32767��
// (3)������: ��ʹ�õ�����ͨ���ĸ���, Ҳ�ǲ���ʱ���������������εĸ���. ������, ÿ�β���һ�����ε�����, ˫����, ÿ�β����������ε�����, ��ռ�Ĵ洢�ռ�����һ��.



typedef
class WavData {  // WAV�ļ�����Ҫ����������  

public:
WORD          Channels;          // ͨ����. ������Ϊ1, ˫����Ϊ2.  little.
DWORD         SampleRate;        // freq, SamplePerSecond,  frequency, ����Ƶ��
// DWORD      ByteRate;          // bps, Byte��, ByteRate, BytesPerSecond, ���ݴ�����, ÿ���ֽڣ�����Ƶ�ʡ�ÿ�������ֽ���
// WORD       BlockAlign;        // ������ֽ��� = channles * bit_samp / 8. little.   
WORD          BitsPerSample;     // bit_samp, sample_num_bit, BitsPerSample, һ��������λ��, �ֳ�����λ��.     
DWORD         dataSize;          // SubCK2Size, ���ݴ�С, =dwSamples*Channels*BitsPerSample/8. 
BYTE*         ps;     // sound data, Ƶ���� ,����Ҫ����ʲô�Ϳ�����λ����, ������ֻ�ǵ����ĸ�������

// ͨ�� malloc��realloc��free �����ڴ� ps.

public:
WavData(){ Channels=1; SampleRate=8000; BitsPerSample=8;  ps=0; }
~WavData(){if(ps){ free(ps); ps=0; }  } 

}WAVDATA;








bool wav_read(const char*szFile, WAVDATA&wd);



int get_sd(int i, WavData*wd, int cs = 0); 



#if CSDN_CODE   

// https://blog.csdn.net/Hamlee67/article/details/82969170 

// https://blog.csdn.net/meicheng777/article/details/52449443  WAV�ļ��Ķ�д 

// https://blog.csdn.n et/ininw/article/details/70195934  WAV�ļ�����  

// https://blog.csdn.net/vacu_um/article/details/70195761  WAV�ļ���ʽ����  




// WAV �ļ��ṹ
// RIFF��
// WAVE FOURCC
// fmt ��
// fact �飨��ѡ��
// data�飨����PCM���ݣ�




#if FOR_REFERENCE






// RIFFͷchunk��ʾ���� 

struct RIFF_HEADER{
TCHAR szRiffID[4];        // 'R','I','F','F'
DWORD dwRiffSize;
TCHAR szRiffFormat[4];    // 'W','A','V','E'
};


// �ڶ�������fmt chunk, ����������WAVE�ļ�������, ��������ʡ�������  
// fmt�����������.  

struct WAVE_FORMAT {
WORD wFormatTag;
WORD wChannels;
DWORD dwSamplesPerSec;
DWORD dwAvgBytesPerSec;
WORD wBlockAlign;
WORD wBitsPerSample;

 WORD ex_size;         // uint16_t, ��չ��Ĵ�С, ���ӿ�Ĵ�С. �����ַ��������ò�ʹ����չ��.һ��������fmt�е�size�ֶ�Ϊ16����ex_size�ֶΣ������ߣ���ex_size,������ֵΪ0.�ڱ����У�ʹ�õڶ��ַ���������ex_size��ֵΪ0����ʹ����չ�顣

};



//  ��������ʵ���������ݵ� data chunk 

struct FMT_BLOCK{
TCHAR szFmtID[4];    // 'f','m','t',' ' please note the  pace character at the fourth location.
DWORD dwFmtSize;
WAVE_FORMAT wavFormat;
};


// ���Ͼ���һ��WAV�ļ��������������chunk
// Ҳ�����кܶ��ѡchunkλ��fmt block��data block֮��
// ������һ����ѡchunk������ note chunk 

struct NOTE_CHUNK{
TCHAR ID[4];    // 'note'
long chunkSize;
long dwIdentifier;
TCHAR dwText[];
};







typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
 
// WAV����RIFF(Resource Interchange File Format)�ļ��淶.
// RIFF �ļ�ͷһϵ�����ݿ�(data chunks).
// WAVE�ļ�ͨ������һ������WAVE���ݿ��RIFF�ļ�.
// �����WAVE���ݿ�(chunk)�����������ݿ�(sub-chunks)���, "fmt" chunk ˵�����ݵĽṹ,��"data" chunk ������������Ƶ����. 
// wav�ļ�ȥ��ͷ����snd�ļ���



typedef struct WaveHeader{ 

//  RIFF_HEADER ......

BYTE  RIFF[4];                 // ChunkID,      ="RIFF", ��Դ�����ļ���־ 
DWORD RiffSize;                // ChunkSize,    ���¸���ַ��ʼ���ļ���β���ֽ���; 
BYTE  WAVE[4];                 // Format,       ="WAVE", wave�ļ���ʶ; 

// The "fmt" SubChunk ......

BYTE  fmt[4];              // SubChunk1ID,        ="fmt", ���θ�ʽ��ʶ 
DWORD fmtSize;             // SubChunk1Size,      �����ֽ�,һ��Ϊ00000010H, ��Ϊ00000012H��˵������ͷЯ��������Ϣ
WORD  fmtFlag;             // AudioFormat,        ��ʽ����,  ֵΪ1ʱ,  ��ʾPCM���Ա���  
WORD  channels;            // NumChannels,        ͨ����, ������Ϊ1, ˫����Ϊ2  
		uint32 samp_freq;           // SampleRate,         ����Ƶ��  
		uint32 byte_rate;           // ByteRate,           ���ݴ����� (ÿ���ֽڣ�����Ƶ�ʡ�ÿ�������ֽ���)  
		uint16 block_align;         // BlockAlign,         ������ֽ��� = channles * bit_samp / 8  
		uint16 bit_samp;            // BitsPerSample,      bits per sample (�ֳ�����λ��)  


// The Data SubChunk ......  

// SubChunk2ID;             //  ="data"    
// SubChunk2Size;           //  data���������ֽڳ��ȣ�Ҳ��ʣ�����ݵĴ�С
// Data;                    // ��ʵ����Ƶ���ݣ�actual sound data�� 

	
} wave_header_t; 
 


typedef struct WaveStruct { 
		FILE *fp;                    // file pointer  


		wave_header_t* pHeader;      // header  

// 
		uint8 data_flag[4];          // ���ݱ�ʶ�� ;
		uint32 length;               // ������������ ;
		uint32 *pData;               // data ;
} wave_t; 


 



#endif  // FOR_REFERENCE








#endif  // CSDN_CODE






typedef
class _WAVFILE {     // Ĭ�� little-endian  
public:

    
// The RIFF chunk  descriptor 
    char            RIFF[4];      // ChunkID, "RIFF",  ��Դ�����ļ���־. big-endian 
    DWORD        ChunkSize;       // ���¸���ַ��ʼ���ļ���β���ֽ���. little.  typedef unsigned int uint32;
                                  // ���ֶε���ֵ�� 8 Ϊ��ǰ�ļ���ʵ�ʳ���.
    char            WAVE[4];      // Format, ��WAVE�� �ļ���ʶ big

    
// The "fmt" sub-chunk. ���� "data" sub-chunk �����ݵ����� 
    char            fmt[4];       // SubchunkID, "fmt ", ���θ�ʽ��ʶ     big
    DWORD        SubCk1Size;      // �����ֽ�,һ��Ϊ16=00000010H, ��18=12H, ��Ϊ12H,���и�����Ϣex_size.  little
    WORD          AudioFmt;       // ��ʽ����.  ֵΪ1ʱ��ʾPCM���Ա���, ��ѹ��, û��fact��.  little. typedef unsigned short int uint16; 
    WORD          Channels;       // ͨ����. ������Ϊ1, ˫����Ϊ2.  little.

    DWORD        SampleRate;      // ����Ƶ��. little. 
 
    DWORD        ByteRate;        // ���ݴ�����,ÿ���ֽڣ�����Ƶ�ʡ�ÿ�������ֽ���. little.  
                                  // Byte��=����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8, =SampleRate*channles * bit_samp / 8. 


    WORD          BlockAlign;     // ������ֽ��� = channles * bit_samp / 8. little.   
    WORD          BitsPerSample;  // bit_samp, �ֳ�����λ��. little.   

//...��36�ֽ� 


    WORD          ex_size;         //  ��չ��Ĵ�С, ����.  




    // �����ַ���ָ����ʹ����չ�飺
    //    �� ex_size �ֶ�,  �� fmt ���е� SubCk1Size �ֶ�Ϊ 16 = 0x10. 
    //    �� ex_size, (��ʱ SubCk1Size �ֶ�Ϊ 0x12>16.)  ������ֵΪ0.
// ���Է��� ex_size ���庬��, ������ʲô��˼. ���� SubCk1Size>0x10 ���� �� fact. 



    
// The "fact" sub-chunk. ѹ����ʽ��Ҫ��չ��.  
    char            fact[4];         // "fact"  
    DWORD          factSize;         // ���ӿ��С 
    DWORD          dwSamples;        // �������� (ÿ������)
    





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
void t_write(const char*wavf = "��ͯ��԰.wav");


void t_wavcx(const char*wavf = "��ͯ��԰.wav");







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



void t_rw(const char*wavf = "wavFile/�Ұ����й�.wav");


void t_mix();


#endif  // _WAVFILE_H_ //




