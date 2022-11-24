

// https://blog.csdn.net/meicheng777/article/details/52449443  WAV文件的读写 



#pragma warning(disable:4996)


#include <Windows.h> 
#include <stdio.h> 
#include <io.h>        // filelength  
//#include <math.h> 

#define _WAVFILE_CPP_ 
#include "WAVFILE.h" 


#include "../Interface2D/qTrace.h"


// wavRead, 读取 wav 文件内容.   返回读取的字节总数.  
// waveOutOpen 函数需要参数 WAVEFORMATEX,   waveOutWrite 函数需要声音数据  
// 检验、测试、描述 wav 文件结构!

// 返回 DIW*. 测试完美通过 

void* wavRead(const char*szFile, void*pdiw) {   // DIW diw 

FILE*fp = fopen(szFile, "rb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return 0; }


DIW*diw=(DIW*)pdiw;  
if(!diw) diw = (DIW*)malloc( sizeof(DIW) );  





int Len = filelength(fileno(fp));
qLog("\r\n%s\t  %04X Bytes\t   %.1f K\r\n", szFile,Len, Len/1024.);



int ip = 0,  ic = 0;    // ip 文件指针, =ftell. ic 块内偏移(或块的字节总数)   ipo 是临时变量, 用于检验读取字节数.
char st[5]={0};
RIFF_HDR rh;  
int ir=0;


CHUNK_HDR ckh;       // "RIFF"、"fmt "、"data" ...


















while(!feof(fp)){    // 会有很多 Chunk  ...  


// "...." chunk  
ir=fread(&ckh,1, sizeof(ckh), fp);       
memcpy(st,ckh.Name,4);  
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X) \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=ir;  



if( *st==0) { 
qLog("\t  块名[]错误! Header (%s, %04X) 为误读. 已退 0x%X 字节, 并尝试进 1 字节.\r\n", st, ckh.ChunkSize, sizeof(ckh)); 
qTrace("块名[%s]错误! 应当终止, 但针对当前实验文件, 可尝试文件指针+1. ",  st+1);  fseek(fp, 1-sizeof(ckh), SEEK_CUR);   ip-=(sizeof(ckh)-1);  
continue; 
}  





#if  READ_MAJOR|1


if(strnicmp(st,"RIFF",4)==0 ){     // RIFF 块比较特殊: chunkSize 是总体内容, 到下一块只需跳过 "WAVE"标识.    
qLog("\t   验证块大小 %04X= %04X+%04X =%04X \r\n", Len, ip, ckh.ChunkSize, ip+ckh.ChunkSize  );     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 

ir=fread(st,1, 4, fp);         
qLog("[%04X, %04X) %X\t WAVE Tag (%s) \r\n", ip, ip+ ir, ir,  st);   
ip+=ir; 

continue;  
}






if(strnicmp(st,"fmt ",4)==0 ){     // "fmt " 块: .    


#if   READ_FMT|1     // 读取内容大小必须严格等于 ckh.ChunkSize  

if(ckh.ChunkSize==0x10) qLog("\t ChunkSize-0x10 = %02X-10=%02X, 没有 cbSize 没有 fact 块.  \r\n",   ckh.ChunkSize,  ckh.ChunkSize- 0x10);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
if(ckh.ChunkSize> 0x10) qLog("\t ChunkSize-0x10 = %02X-10=%02X, 有 cbSize. cbSize>0 时有扩展信息, 无 fact 块.  实验发现 ChunkSize>0x10 ←→  有 fact.\r\n",   ckh.ChunkSize,  ckh.ChunkSize- 0x10);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
 

// 但是测试发现 ckh.ChunkSize >0x10 ←→  有 fact. ?   实验发现有扩展信息, 照样有 fact 块.  

ir=fread(&diw->wfx,1, sizeof(diw->wfx), fp);  // sizeof( WAVEFORMATEX ) = 0x12,  如果对于 ChunkSize, 则需要 seek back.     
qLog("[%04X, %04X) %X\t fmt  Content (..., %X) \t 实际大小是 %X \r\n", ip, ip+ ir, ir,     diw->wfx.cbSize, ckh.ChunkSize);    
ip+=ir;  


// 结论:  --- 是的, 要以 ChunkSize 为准. 
// 1、 如果  ckh.ChunkSize = 0x10, 没有 cbSize 没有 fact 块, 则需要退  2 字节, sizeof(wfx.cbSize)   
// 2、 如果  ckh.ChunkSize > 0x10, 则有 cbSize 
//     此时如果  cbSize=0, 则接下来是 fact 块, 如果  cbSize> 0, 则没有fact, 但有 cbSize 大小的扩展信息.   


if(ckh.ChunkSize==0x10){   //  没有 cbSize 没有 fact 块, 则需要退  sizeof(wfx.cbSize)=2 字节,   并且可以返回主循环.  
fseek(fp,-2, SEEK_CUR);  ip-=2;  
qLog("[%04X, %04X) 2\t  ChunkSize=0x10, 误读 cbSize=%X, 已退 2 字节并返回主循环. 注意, 接下来应该没有 fact 块. \r\n", ip, ip+2, diw->wfx.cbSize); 
// ip+=2;    
continue;  
}


if(ckh.ChunkSize>0x10){

if(diw->wfx.cbSize == 0){ 
qLog("\t ChunkSize=%X, 已读取%X.   cbSize=0 无扩展信息, 直接退回主循环. 注意, 接下来应该有 fact 块. \r\n", 
ckh.ChunkSize, sizeof(diw->wfx) ); 

continue;     // 直接返回主循. 接下来应该是  "fact" chunk.  ChunkSize>0x10, 并且 cbSize=0.  
}


if(diw->wfx.cbSize > 0) {              // 如果 ChunkSize>0x10, 并且 cbSize> 0.  则无fact块, 但有附加信息 cbSize 字节.
fseek(fp, diw->wfx.cbSize, SEEK_CUR);  // 此时还应当验证   ChunkSize=  0x12 + cbSize 
qLog("[%04X, %04X) %X\t 扩展信息(...)\t 验证  %02X= %02X+%02X = %02X.  注意, 接下据说无 fact 块, 但实验发现只要 ChunkSize>0x10 就有 fact 块. \r\n", ip, ip+ diw->wfx.cbSize, diw->wfx.cbSize,
ckh.ChunkSize, sizeof(diw->wfx), diw->wfx.cbSize,  sizeof(diw->wfx)+ diw->wfx.cbSize );     
ip+=diw->wfx.cbSize;  

continue; 
}    

}     // if(ckh.ChunkSize>0x10)  


// wfx.cbSize=0;   // WAVEFORMATEX 的 cbSize 总是 0 ?   参见 MSDN:   
//  extra format information appended to the end of the structure. 
// for non-PCM formats to store extra attributes for the wFormatTag. 
// Note that for WAVE_FORMAT_PCM=1 formats (and only WAVE_FORMAT_PCM formats), this member is ignored. 



continue;  // 返回主循 -- 多余语句 
#endif     

}


if(strnicmp(st,"data",4)==0 ){


diw->dataSize= ckh.ChunkSize;
diw->pData= (BYTE*)malloc( ckh.ChunkSize ); 


ir=fread(diw->pData,1, ckh.ChunkSize, fp);       
qLog("[%04X, %04X) %X\t data Content (%04X bytes)\t  ftell=%04X  \r\n", ip, ip+ ir, ir, ir, ftell(fp));     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 


// "data" 有可能是最后一块. 
if( ip+ckh.ChunkSize>=Len   )  {  // && strnicmp(st,"RIFF",4)!=0     // 为啥 feof() 没有反应?!  
qLog("\r\n%04X + %04X = %04X >= %04X\r\n", ip, ckh.ChunkSize, ip+ ckh.ChunkSize,  Len); 
break; 
}   

ip+=ir;  
continue; 
}


#endif    // READ_MAJOR 






if(ip+ckh.ChunkSize > Len){
qTrace("块名[%s] 或大小错误! 文件指针越界:  %04X + %04X =%04X >= %04X! 退出.",  st, ip, ckh.ChunkSize,  ip+ckh.ChunkSize, Len);  
break; 
} 




// 暂时跳过 chunk content
fseek(fp, ckh.ChunkSize, SEEK_CUR);    // 有些 ChunkSize 记录是错的 ... 测试 +1

qLog("[%04X, %04X) %X\t %s Content( %04X ). \r\n", ip, ip+ ckh.ChunkSize,  ckh.ChunkSize, st,   ckh.ChunkSize);     


// if(ip>=Len)  {   if(0)qTrace("%04X >= %04X", ip, Len);   break;   }   // 为啥 feof() 没有反应?! 
if( ip+ckh.ChunkSize>=Len   )  {  // && strnicmp(st,"RIFF",4)!=0     // 为啥 feof() 没有反应?!  
qLog("\r\n%04X + %04X = %04X >= %04X\r\n", ip, ckh.ChunkSize, ip+ ckh.ChunkSize,  Len); 
break; 
}   


ip+=ckh.ChunkSize; 
}       // while  


fclose(fp); 
  
if(0)qLog();  

return diw;
}






// 写(规范)声音文件. 不压缩, 无 fact 等扩展块.  bCopy = 不更改 pdiw 内容.  

int wavWrite(DIW*diw, const char*szFile, bool DefaultFmt){  // =0L  =false

if(!diw) return false;   if( diw->dataSize<=0) return false;  
if(szFile==0L)  szFile="wavWrite.wav";  


FILE*fp = fopen(szFile, "wb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return false; }


// 计算文件长度  



CHUNK_HDR ckh={{'R','I','F','F'}, 0, };       // "RIFF"、"fmt "、"data" ...
int Len= sizeof(ckh)                 //  RIFF  块头 
    + 4                              // "WAVE" 标记   
    + sizeof(ckh)                    // "fmt " 块头
    + 0x10                           // "fmt " Size,  0x10 =sizeof(WAVEFORMATE) 仅 适合于 WAVE_FORMAT_PCM =1 格式, 无 扩展信息 c=bSize, 无 "fact" 块. 
    + sizeof(ckh)                    // "data" 块头  
    + diw->dataSize                  // 数据长度  
;                                    // = 8 + 0x24+diw->dataSize ,   8+4+8+0x10+8 = 8 + 0x24 = 0x2C  

ckh.ChunkSize=Len-sizeof(ckh);             // = 0x24 + diw->dataSize
// ckh.ChunkSize  = 0x24+ diw->dataSize;   // = Len - 8.  diw->dataSize 包括前面 0x24+8 -8 =36 字节. 但如果有扩展信息、fact 块, 则要 > 0x24.  



qLog("\r\n%s\t  %04X Bytes\t   %.1f K\r\n", szFile,Len, Len/1024.);

char st[5]={0};  memcpy(st,ckh.Name,4);  

// 写 RIFF 块头   
int ip=0, ir = fwrite(&ckh, 1, sizeof(ckh), fp);     assert( ir==sizeof(ckh) );  
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X)\t RIFF块实际数据仅4字节,\"WAVE\" \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=sizeof(ckh);  

char WAVE[5]={'W','A','V','E',0};  
// 写 "WAVE" 文件标识 big-endian 
ir = fwrite(WAVE,1,  4, fp);      assert( ir==4 ); 
qLog("[%04X, %04X) %X\t %s \r\n", ip, ip+ ir, ir,  WAVE);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=4;


// 写 "fmt " 块   
memcpy( ckh.Name,"fmt ",4);  memcpy(st,ckh.Name,4);  
ckh.ChunkSize=0x10;    // 仅支持  WAVE_FORMAT_PCM 格式, 无 cbSize, 无扩展信息, 无 fact 块.   
ir = fwrite(&ckh, 1, sizeof(ckh), fp);    assert( ir==sizeof(ckh) ); 
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X)\t RIFF块实际数据仅4字节,\"WAVE\" \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=ir;


// 写 fmt  Content, WAVEFORMATE 数据, 0x10 字节  比 WAVEFORMAT 多一个 wBitsPerSample,  比 WAVEFORMATEX 少一个 cbSize 
WAVEFORMATE wfe;   // 已经初始化为 default 
if(!DefaultFmt) wfe=*(WAVEFORMATE*)&diw->wfx;   

// 3 个需要指明的字段 -- 上面的等式已经实现了
// wfe.wFormatTag = WAVE_FORMAT_PCM;  //   = 1;   
// wfe.nChannels = diw->wfx.nChannels;  
// wfe.wBitsPerSample  = diw->wfx.wBitsPerSample;
// wfe.nSamplesPerSec = diw->wfx.nSamplesPerSec;  

// 2 个需要计算的字段, 以免出错   
wfe.nAvgBytesPerSec = wfe.nChannels*wfe.nSamplesPerSec* wfe.wBitsPerSample/8; 
wfe.nBlockAlign= wfe.nChannels*wfe.wBitsPerSample/8;  

ir = fwrite(&wfe, 1, sizeof(wfe), fp);    assert( ir==sizeof(wfe) );   
qLog( "[%04X, %04X) %X\t %s  Content ( %04X ) \r\n", ip, ip+ ir, ir,  st,  sizeof(wfe) );     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=ir;

// 写 data 块头 
memcpy( ckh.Name,"data",4);  memcpy(st,ckh.Name,4);  
ckh.ChunkSize=diw->dataSize;       
ir = fwrite(&ckh,  1, sizeof(ckh),fp);    assert( ir==sizeof(ckh) ); 
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X) \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=ir;

// 写 data Content 
ir= fwrite(diw->pData, 1, diw->dataSize, fp);  assert(ir == diw->dataSize);
qLog( "[%04X, %04X) %X\t %s  Content ( %04X ) \r\n", ip, ip+ ir, ir,  st,  diw->dataSize );     //   如果 ChunkSize>0x10, 则有附加信息 cbSize 字节. 
ip+=ir;


fclose(fp); qLog();  

return ip;  
}





void t_rw(const char*wavf){
DIW diw;  
DIW* pdiw=(DIW*)wavRead( wavf, &diw);  if(!pdiw) return;  
wavWrite(&diw,  "wavFile/t_我爱你中国.wav"); 
}



















bool wav_read(const char*szFile, WAVDATA&wd) {

FILE*fp = fopen(szFile, "rb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return false; }

int fLen = filelength(fileno(fp));
sLog("\r\n%s\r\n%X / %d  BYTES\r\n", szFile, fLen, fLen);


    
int ip = 0, ic = 0;    // ip 文件指针, =ftell. ic 块内偏移(或块的字节总数) 

WavHeader wt;  
int ir=fread(&wt,sizeof(wt),1, fp);   assert(ir==1); 
char* p = (char*)wt.RIFF;   char c = *(p + 4);    *(p + 4) = 0;
ic= wt.ChunkSize +8;    // +8, 包括Chunk头的8字节. 
sLog("\r\n[%04X, %04X) %X\t [%s]   \r\n", ip, ip+ ic, ic, 
wt.RIFF);    *(p + 4) = c;
ip+= sizeof(wt);   

sLog("\r\nChunkSize\t%X\r\nAudioFmt\t%X\r\nChannels\t%X\r\nSampleRate\t%X\r\nByteRate\t%X\r\nBlockAlign\t%X\r\nBitsPerSample\t%X\r\n", 
 wt.ChunkSize, wt.AudioFmt, wt.Channels,  wt.SampleRate, wt.ByteRate, wt.BlockAlign, wt.BitsPerSample);


sLog("\r\n验证 BlockAlign\t %d = %d\r\n", wt.BlockAlign, wt.Channels * wt.BitsPerSample / 8);
sLog("验证 ByteRate\t %d = %d\r\n", wt.ByteRate, wt.SampleRate*wt.Channels * wt.BitsPerSample / 8);
// sLog("\r\n验证 SubCk2Size %X = %X\r\n", wav->SubCk2Size, wav->dwSamples*wav->Channels * wav->BitsPerSample / 8);




//WavFile wf; 


char ckName[5]; ckName[4] = 0;  // 尾部始终为 \0
DWORD ckSize;


p = (char*)wt.fmt;   c = *(p + 4);   
ic =  8+ wt.SubCk1Size;   *(p + 4) = 0;
sLog("\r\n[%04X, %04X)\t%X\t [%s]  \r\n", 12, 12 + ic, ic,
wt.fmt);    *(p + 4) = c;




if( wt.SubCk1Size > 0x10) {  // 需要读取  ex_size. 也可以跳动文件指针  wt.SubCk1Size - 0x10 

WORD ex_size=0;   
ir = fread(&ex_size, sizeof(ex_size), 1, fp);  assert(ir == 1);
    sLog("\r\n[%04X]\t ex_size  %X\r\n", ip, ex_size);
    ip += sizeof(ex_size);

// if (wf.ex_size > 0) { fseek(fp, wf.ex_size, SEEK_CUR); ip += wf.ex_size; }  // <--- 并不清楚 ex_size 的含义. 

if(wt.SubCk1Size > 0x12) { fseek(fp, wt.SubCk1Size -0x12, SEEK_CUR); ip += wt.SubCk1Size - 0x12; MessageBox(0,"?",0,0); }  // "fmt " 是不是最多多出一个 WORD ex_size, 尚不肯定


DWORD dwSamples=0;   // 注意, sizeof(ckName) 多了1. 
ir = fread(ckName, 4, 1, fp);  assert(ir == 1);  // "fact"  
ir = fread(&ckSize, sizeof(ckSize), 1, fp);  assert(ir == 1);
ir = fread(&dwSamples, sizeof(dwSamples), 1, fp);  assert(ir == 1);

ic= 4+ sizeof(ckSize)+ sizeof(dwSamples);

sLog("\r\n[%04X, %04X)\t%X\t [%s]\r\n", ip, ip+ic, ic, ckName);  
ip += ic;

if(ckSize>4){ fseek(fp, ckSize - 4, SEEK_CUR); ip += ckSize - 4; }    // 应该不会发生  

}  // if 



// "data" 块之后可能还有其他块  

// WavData wd={0}; 

wd.Channels=wt.Channels;  
wd.SampleRate=wt.SampleRate; 

wd.BitsPerSample=wt.BitsPerSample;  

// wd.dataSize=0;  BYTE*ps=0;   // sound data  

while (!feof(fp)){

ir = fread(ckName, 1, 4, fp); if(ir<4){ qTrace( "feof=%d, ftell=%X, ir=%d", feof(fp), ftell(fp), ir );   };
ir = fread(&ckSize, 1, sizeof(ckSize), fp);  if (ir < sizeof(ckSize)) { qTrace("feof=%d, ftell=%X, ir=%d", feof(fp), ftell(fp), ir); };

sLog("\r\n[%04X, %04X)\t%X\t [%s]  \r\n", ip, ip+8 + ckSize, 8+ckSize, ckName);  

ip+=8+ ckSize;
if(ip>fLen) { qTrace("feof=%d, ftell=%X, fLen=%X, ip=%X", feof(fp), ftell(fp), fLen,ip);  break;  }



if(memcmp(ckName,"data",4)==0){  // 只关心声音数据  

wd.dataSize= ckSize;
wd.ps=(BYTE*)realloc(wd.ps, ckSize);  // new BYTE[ckSize];  
fread(wd.ps, ckSize,1,fp);

}else fseek(fp, ckSize,SEEK_CUR);



// 容错处理. 有些文件的 ChunkSize 是错的. 
#if FAULT_TOLERANT|1

char c=0;   // 0x20 – 0x7E 
while (!feof(fp) && !isprint(c)) fread(&c, 1, 1, fp);

if(isprint(c)) fseek(fp,-1,SEEK_CUR);  // 因为至少读一次 

#endif   // FAULT_TOLERANT 

}  // while  



//sLog("\r\n验证 SubCk2Size %X = %X\r\n", wd.dataSize, dwSamples*wd.Channels * wd.BitsPerSample / 8);  // 可能没有 "fact"

// ir = ftell(fp);  sLog("验证 FileLength = %X\t  ir=%X\t  △=%X   \r\n", fLen, ir, fLen - ir);



_EXIT_:
    fclose(fp);
    return true;
}









#if  SIMPLE_PLAY|1

//  ReadFile,  loadAudioBlock, see   https://blog.csdn.net/xbk123123/article/details/53787287

// 全部读取 -- 没有用处 
LPSTR ReadFile(const char*filename, DWORD* blockSize){
	HANDLE hFile= INVALID_HANDLE_VALUE;
	DWORD size = 0;
	DWORD readBytes = 0;
	void* block = NULL;
	if((hFile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE){ MessageBox(0,filename,0,0); return 0; }
	
	do 
	{
		if((size = GetFileSize(hFile, NULL)) ==0) break;
		 
		if((block = HeapAlloc(GetProcessHeap(),0, size)) == NULL) break;
		 
		ReadFile(hFile, block, size,&readBytes, NULL);
	} while(0);

	CloseHandle(hFile);
	*blockSize = size;
	
return (LPSTR)block;
}

void writeWavBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size){
	WAVEHDR hdr;
	ZeroMemory(&hdr, sizeof(WAVEHDR));
	hdr.dwBufferLength = size;
	hdr.lpData = block;
	waveOutPrepareHeader(hWaveOut, &hdr, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &hdr, sizeof(WAVEHDR));
	Sleep(500);
	while(waveOutUnprepareHeader(hWaveOut,&hdr,sizeof(WAVEHDR)) ==WAVERR_STILLPLAYING)
	Sleep(100);
}



// 还有一些 bug: 播放完毕不退出?  

BOOL PlayWav(char* soundFileName){     if (soundFileName == NULL)return FALSE;


DIW diw; wavRead(soundFileName,&diw); 

#if DEFAULT_WFX|0





// static  
// WAVEFORMATEX waveForm={WAVE_FORMAT_PCM, 2,   44100,0L,  0L, 16, 0 };  // BOOL CIP_PHONEDlg::initAudioDevice()
WAVEFORMATEX waveForm={WAVE_FORMAT_PCM, 1,   11025,0L,  0L, 8, 0 };  // BOOL CIP_PHONEDlg::initAudioDevice()
	
//		waveForm.nSamplesPerSec = 44100; // sample rate  
//		waveForm.wBitsPerSample = 16;    // sample size  
//		waveForm.nChannels= 2;           // channels 
//		waveForm.cbSize = 0;             // size of _extra_ info  
//waveForm.wFormatTag = WAVE_FORMAT_PCM;
		
waveForm.nBlockAlign = (waveForm.wBitsPerSample * waveForm.nChannels) >> 3;   //  16*2/8 = 4  
waveForm.nAvgBytesPerSec = waveForm.nBlockAlign * waveForm.nSamplesPerSec;    //  4* 44100     每秒  声道数 × 样本数 × 样本字节数   
		


#endif    // DEFAULT_WFX
	
	
HWAVEOUT hWaveOut; 
//if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveForm, 0, 0, CALLBACK_NULL) !=MMSYSERR_NOERROR)	{ MessageBox(0, "音频设备打卡失败", 0, MB_OK); return FALSE; }
if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &diw.wfx, 0, 0, CALLBACK_NULL) !=MMSYSERR_NOERROR)	{ MessageBox(0, "音频设备打卡失败", 0, MB_OK); return FALSE; }
	



// LPSTR block;  DWORD blockSize;    \
if((block = ReadFile(soundFileName, &blockSize)) ==NULL){ MessageBox(0,"无法打开音频文件",0, MB_OK); return FALSE; }






// writeWavBlock(hWaveOut, block, blockSize); 
writeWavBlock(hWaveOut, (LPSTR)diw.pData, diw.dataSize); 
	

waveOutClose(hWaveOut);
	
return TRUE;
}




#endif    // SIMPLE_PLAY 
























// uint8 转 int8, -128~127    

// b+~b&0xFF= 0xFF 

int b_int8(BYTE b) {  // BYTE 0~255  

    int i = b & (1 << 7);
    if (i == 0) return b;

    return -(1 + ~b & 0xFF);   // 取反 + 1 


    int r = b;
    if (r > 127) r -= 256;
    return r;
}




// w_int16, WORD 转换为 16 位有符号整数. 

// 为何 16 位 int 类型范围是 [-32768, 32767] 

// w+~w&0xFFFF= 0xFFFF 

int w_int16(WORD w) {

    int i = w & (1 << 15);

    if (i == 0) return w;

    //  3 的码值是 0000 0011, －3 则为 11111100＋1 ＝ 11111101

    //  32767 是 0111 1111  1111 1111 (15个1), -32767是 1000 0000  0000 0000 + 1 = 1000 0000  0000 0001  (取反加1)

    // -32768 是 0111 1111  1111 1111 + 1 = 1000 0000  0000 0000

    return -(1 + ~w & 0xFFFF);   // 取反 + 1 
};


// W+~W&0xFFFFFFFF= 0xFFFFFFFF 

int dw_int32(DWORD W) {
    return (int)W;
    if ((0 == (1 << 31)&W)) return W;
    //return -( 1 + ~W&0xFFFFFFFF );  
    return (1 << 32) - W;
}







// _wav_read, 逐项读取 wav 文件内容. 可能读不到某些块.  

bool _wav_read(const char*szFile, wave_t*wav=0){

if (!wav)  wav=new wave_t; 



//qTrace("ChunkSize=%d, SubCk1Size=%d,  RIFF=%s, data=%s",  \
wav->ChunkSize, wav->SubCk1Size, wav->RIFF, wav->data );

FILE*fp=fopen(szFile,"rb");
if(!fp) { qTrace("%s\r\n%s", strerror(errno),szFile);  return false; }

int fLen= filelength( fileno(fp) );
sLog("%s\t %X / %d  BYTES\r\n", szFile, fLen, fLen);


int ip=0, ic=0;    // 文件指针、块内偏移 

int ir=fread(wav->RIFF,sizeof(wav->RIFF),1,fp);  assert(ir==1); 
char* p=  (char*) wav->RIFF;   char c=*(p+4);    *(p + 4) = 0;
sLog("\r\n[%04X]\t \"%s\"\r\n", ip,  wav->RIFF);  *(p + 4) = c;
ip+= sizeof(wav->RIFF);  


ir = fread(&wav->ChunkSize, sizeof(wav->ChunkSize), 1, fp);  assert(ir == 1);
sLog("[%04X]\t ChunkSize  %X\r\n", ip, wav->ChunkSize );  // + 8 为文件大小 
ip+= sizeof(wav->ChunkSize);
 
ir = fread(wav->WAVE, sizeof(wav->WAVE), 1, fp);  assert(ir == 1);
p=(char*)wav->WAVE;  c = *(p + 4);  *(p + 4) = 0; 
sLog("\r\n[%04X]\t [%s]\r\n", ip, wav->WAVE);    *(p + 4) = c; 
ip+= sizeof(wav->WAVE);

ir = fread(wav->fmt, sizeof(wav->fmt), 1, fp);  assert(ir == 1);
p=(char*)wav->fmt;  c = *(p + 4);  *(p + 4)  = 0;
sLog("\r\n[%04X]\t [%s]\r\n", ip, wav->fmt);    *(p + 4) = c; 
ip+= sizeof(wav->fmt);



ir = fread(&wav->SubCk1Size, sizeof(wav->SubCk1Size), 1, fp);  assert(ir == 1);
sLog("\r\n[%04X]\t SubCk1Size  %X\r\n", ip, wav->SubCk1Size);
ip+= sizeof(wav->SubCk1Size);



ir = fread(&wav->AudioFmt, sizeof(wav->AudioFmt), 1, fp);  assert(ir == 1);
sLog("[%04X]\t AudioFmt  %X\r\n", ip, wav->AudioFmt);
ip+= sizeof(wav->AudioFmt);


ir = fread(&wav->Channels, sizeof(wav->Channels), 1, fp);  assert(ir == 1);
sLog("[%04X]\t Channels  %X\r\n", ip, wav->Channels);
ip+= sizeof(wav->Channels);



ir = fread(&wav->SampleRate, sizeof(wav->SampleRate), 1, fp);  assert(ir == 1);
sLog("[%04X]\t SampleRate  %X\r\n", ip, wav->SampleRate);
ip+= sizeof(wav->SampleRate);



ir = fread(&wav->ByteRate, sizeof(wav->ByteRate), 1, fp);  assert(ir == 1);
sLog("[%04X]\t ByteRate  %X\r\n", ip, wav->ByteRate);
ip+= sizeof(wav->ByteRate);


ir = fread(&wav->BlockAlign, sizeof(wav->BlockAlign), 1, fp);  assert(ir == 1);
sLog("[%04X]\t BlockAlign  %X\r\n", ip, wav->BlockAlign);
ip+= sizeof(wav->BlockAlign);


ir = fread(&wav->BitsPerSample, sizeof(wav->BitsPerSample), 1, fp);  assert(ir == 1);
sLog("[%04X]\t BitsPerSample  %X\r\n", ip, wav->BitsPerSample);
ip+= sizeof(wav->BitsPerSample);


sLog("\r\n验证 BlockAlign %d = %d ... \r\n", wav->BlockAlign, wav->Channels * wav->BitsPerSample / 8);
sLog("验证 ByteRate %d = %d\r\n", wav->ByteRate, wav->SampleRate*wav->Channels * wav->BitsPerSample / 8);





// fseek(fp,2,SEEK_CUR); 
// ir = fread(wav->data, sizeof(wav->data), 1, fp);  assert(ir == 1);
// sLog("\r\ndata=%s\r\n", wav->data);


if(wav->SubCk1Size!=16){    // 需要扩展块... 为什么 ex_size一定存在?  

ir = fread(&wav->ex_size, sizeof(wav->ex_size), 1, fp);  assert(ir == 1);
sLog("\r\n[%04X]\t ex_size  %X\r\n", ip,wav->ex_size);
ip+=sizeof(wav->ex_size); 

if(wav->ex_size>0){ fseek(fp,wav->ex_size,SEEK_CUR); ip+= wav->ex_size;  }



ir = fread(wav->fact, sizeof(wav->fact), 1, fp);  assert(ir == 1);
p = (char*)wav->fact;  c = *(p + 4);  *(p + 4) = 0;
sLog("\r\n[%04X]\t [%s]\r\n", ip, wav->fact); *(p + 4) = c;
ip+=sizeof(wav->fact);


if( 0!=memcmp(wav->fact,"fact",4) ){  fseek(fp, wav->ex_size-4,SEEK_CUR); ip += wav->ex_size-4;  }  // 跳过扩展块 
else{  // 读 fact 块  

ir = fread(&wav->factSize, sizeof(wav->factSize), 1, fp);  assert(ir == 1);
sLog("[%04X]\t factSize  %X\r\n", ip, wav->factSize);
ip+=sizeof(wav->factSize);

ir = fread(&wav->dwSamples, sizeof(wav->dwSamples), 1, fp);  assert(ir == 1);
sLog("[%04X]\t dwSamples  %X\r\n", ip, wav->dwSamples);
ip+=sizeof(wav->dwSamples);

}

}  // if  !=16  


ir = fread(wav->data, sizeof(wav->data), 1, fp);  assert(ir == 1);
p = (char*)wav->data;  c = *(p + 4);  *(p + 4) = 0;
sLog("\r\n[%04X]\t [☆%s]\r\n", ip,wav->data); *(p + 4) = c;
ip+=sizeof(wav->data);


if (0 != memcmp(wav->data, "data", 4)) {  qTrace("读取错误!没有正确定位到'data'块.");   goto _EXIT_; }
   

ir = fread(&wav->SubCk2Size, sizeof(wav->SubCk2Size), 1, fp);  assert(ir == 1);
sLog("[%04X]\t SubCk2Size  %X\r\n", ip, wav->SubCk2Size);   // +0x2c ? 为文件大小 
ip+=sizeof(wav->SubCk2Size);

wav->DATA=new BYTE[wav->SubCk2Size];

ir = fread(wav->DATA, 1,  wav->SubCk2Size, fp);  assert(ir == wav->SubCk2Size);
ip+= wav->SubCk2Size; 



if(wav->dwSamples<=0)   wav->dwSamples= wav->SubCk2Size / wav->Channels / (wav->BitsPerSample / 8);

sLog("\r\n验证 SubCk2Size %X = %X\r\n", wav->SubCk2Size, wav->dwSamples*wav->Channels * wav->BitsPerSample / 8);


ir=ftell(fp); 

sLog("验证 FileLength = %X\t  ir=%X, ip=%X\t  △=%X   \r\n", fLen,  ir, ip, fLen-ip);






_EXIT_:
fclose(fp);  
return true; 
}






void t_read(const char*wavf) {   // = "M1F1-Alaw-AFsp.wav"

_wav_read(wavf);

   
//   WavData wd;  wav_read(wavf, wd);  \
sLog("\r\nChannels\t %X\r\nSampleRate\t %X\r\nBitsPerSample\t %X\r\ndataSize\t %X",   \
wd.Channels, wd.SampleRate, wd.BitsPerSample, wd.dataSize); 

sLog(0);

}




// 写(规范)声音文件. 不压缩, 无 fact 等扩展块 
  

bool wav_write(const char*szFile, WAVDATA*wd, bool bCopy=false){

if(!wd) return false; 

FILE*fp = fopen(szFile, "wb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return false; }

WavHeader wt;

// 3个需要指明的字段
// wt.AudioFmt  = 1 
wt.Channels=wd->Channels;  
wt.BitsPerSample = wd->BitsPerSample;
wt.SampleRate    = wd->SampleRate; 

// 3个需要计算的字段 ChunkSize ...  

wt.ByteRate   = wd->SampleRate*wd->Channels * wd->BitsPerSample / 8;
wt.BlockAlign = wd->Channels * wd->BitsPerSample / 8;

wt.ChunkSize  = 0x24+ wd->dataSize;   // 包括前面 36+8 -8 =36 =0x24 字节. 如果有 fact, 一般是 50=0x32, 多出12+2=14字节. 

// qTrace("ChunkSize=%X, ByteRate=%X, BlockAlign=%X", wt.ChunkSize, wt.ByteRate, wt.BlockAlign);


int ir = fwrite(&wt, sizeof(wt), 1, fp);   assert(ir == 1);



// 写 data 块 
char ckName[4]={'d','a','t','a'};   
ir = fwrite(ckName, 1, 4, fp);  assert(ir==4);

ir= fwrite(&wd->dataSize, 1, 4, fp);  assert(ir == 4);

ir= fwrite(wd->ps, 1, wd->dataSize, fp);  assert(ir == wd->dataSize);

fclose(fp);
return true; 
}



void t_write(const char*wavf){



WavData wd;  wav_read(wavf, wd);  \
    sLog("\r\nChannels\t %X\r\nSampleRate\t %X\r\nBitsPerSample\t %X\r\ndataSize\t %X",   \
    wd.Channels, wd.SampleRate, wd.BitsPerSample, wd.dataSize); 

wav_write("t_儿童乐园.wav",&wd); 


    
sLog(0);
}







// 位图宽=时间(毫秒), 高 对应于总样本数/每毫秒样本数 

int get_sd(int i, WavData*wd, int cs ) {

    if (!wd) return 0;

    static int n = wd->dataSize * 8 / wd->BitsPerSample / wd->Channels;  // dwSamples 

    if (i<0 || i >= n) return 0;

    BYTE*p = wd->ps;
    //BYTE* q= p + 2 * i * wd->Channels + cs;    // q >>= (wd->BitsPerSample-8);  // 考虑左补0规则 ...... 

    if (wd->BitsPerSample <= 8) { BYTE v = *(p + i * wd->Channels + cs);   v >>= (8 - wd->BitsPerSample);   return v; }


    if (8 < wd->BitsPerSample && wd->BitsPerSample <= 16) { WORD v = *(WORD*)(p + 2 * i * wd->Channels + cs);   v >>= (16 - wd->BitsPerSample);   return w_int16(v); }

    if (16 < wd->BitsPerSample && wd->BitsPerSample <= 24) { DWORD v = *(DWORD*)(p + 3 * i * wd->Channels + cs);  v &= 0x00FFFFFF;  v >>= (24 - wd->BitsPerSample);   return w_int16(v); }

    if (24 < wd->BitsPerSample && wd->BitsPerSample <= 32) { DWORD v = *(DWORD*)(p + 4 * i * wd->Channels + cs);  v >>= (32 - wd->BitsPerSample);   return dw_int32(v); }

    MessageBox(0, "样本bit值过高!", 0, 0);
    return 0;
}










// wav_cx, 更改 wav 文件的声道数  2-->1, [或相反--tbc].  返回原wav的 Channels, 或 错误值 0.   


int wav_cx(WAVDATA&wd, const char*zf) {

FILE*fp = fopen(zf, "wb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), zf);  return false; }

if(wd.dataSize<=0) return 0;
if(wd.Channels <=0) return 0;
if(wd.BitsPerSample <=0) return 0;

int BS= wd.BitsPerSample / 8;    // BytesPerSample  
if(BS*8<wd.BitsPerSample) BS++; 


DWORD dwSamples = wd.dataSize / BS / wd.Channels; 
 
int Channels=1;  // 减少声道数 
DWORD dataSize= dwSamples* BS*1;   // 重新计算  

//qTrace("wd.dataSize=%d, dataSize=%d", wd.dataSize, dataSize);

WavHeader wt;

// 3个需要指明的字段  // wt.AudioFmt  = 1 
wt.Channels = 1;
wt.BitsPerSample = wd.BitsPerSample;
wt.SampleRate = wd.SampleRate;

// 3个需要计算的字段 ChunkSize ...  

wt.ByteRate = wd.SampleRate* Channels * BS;
wt.BlockAlign = Channels * BS;

wt.ChunkSize = 0x24 + dataSize;   // 包括前面 36+8 -8 =36 =0x24 字节. 如果有 fact, 一般是 50=0x32, 多出12+2=14字节. 

// qTrace("ChunkSize=%X, ByteRate=%X, BlockAlign=%X", wt.ChunkSize, wt.ByteRate, wt.BlockAlign);


int ir = fwrite(&wt, sizeof(wt), 1, fp);   assert(ir == 1);



// 写 data 块 
char ckName[4] = { 'd','a','t','a' };
ir = fwrite(ckName, 1, 4, fp);  assert(ir == 4);

ir= fwrite(&dataSize, 1, 4, fp);  assert(ir == 4);



#if WRITE_DATA|1

int cs=0;  // 0 声道  
BYTE*p= wd.ps+cs*BS;  

int v=0;
//DWORD v=0; // DWORD msk=(1<<BS*8);  

assert(BS==sizeof(WORD));

for(int i=0; i< dwSamples; i++){
// v=get_sd(i,&wd,0);   // 0-声道数据 
// p =  wd.ps + i*BS*wd.Channels; 
// DWORD v = *(DWORD*)p;    v&= msk;  // v >>= (32 - wd.BitsPerSample);
v=*(WORD*)p;  
// v=get_sd(i,&wd,0);
fwrite(&v, BS, 1, fp);  // assert(ir == 4);

p+=BS*wd.Channels;  
}  // for i 

#endif  // 


fclose(fp);

return true;
}




void t_wavcx(const char*wavf) {



    WavData wd;  wav_read(wavf, wd);  \
        sLog("\r\nChannels\t %X\r\nSampleRate\t %X\r\nBitsPerSample\t %X\r\ndataSize\t %X", \
            wd.Channels, wd.SampleRate, wd.BitsPerSample, wd.dataSize);

    wav_cx(wd,"t_儿童乐园cx.wav");



    sLog(0);
}
















int wavSave(WAVEFORMATEX& waveform, BYTE* pSaveBuffer, DWORD dwDataLength, const char*szF="t_audio.wav") {

    // 简单录放音、保存为wav文件程序: https://blog.csdn.net/benny5609/article/details/1915135

    // ⑵编写消息处理函数，其中，MM_WIM_DATA 函数是本程序的核心。其主要作用是将输入数据另行保存在一缓冲区内（pSaveBuffer），该缓冲区的长度将随着已录入数据的大小而增加，从而实现保存输入话音数据的功能。同时，可将缓冲区内数据保存为wav文件。其具体实现如下：
    FILE* fp = fopen(szF, "wb");    //保存路径
    DWORD m_WaveHeaderSize = 38;
    DWORD m_WaveFormatSize = 18;

    fwrite("RIFF", 4, 1, fp);

    unsigned int Sec = (sizeof pSaveBuffer + m_WaveHeaderSize);

    fwrite(&Sec, sizeof(Sec), 1, fp);    //  
    fwrite("WAVE", 4, 1, fp);
    fwrite("fmt ", 4, 1, fp);
    fwrite(&m_WaveFormatSize, sizeof(m_WaveFormatSize), 1, fp);
    fwrite(&waveform.wFormatTag, sizeof(waveform.wFormatTag), 1, fp);
    fwrite(&waveform.nChannels, sizeof(waveform.nChannels), 1, fp);
    fwrite(&waveform.nSamplesPerSec, sizeof(waveform.nSamplesPerSec), 1, fp);
    fwrite(&waveform.nAvgBytesPerSec, sizeof(waveform.nAvgBytesPerSec), 1, fp);
    fwrite(&waveform.nBlockAlign, sizeof(waveform.nBlockAlign), 1, fp);
    fwrite(&waveform.wBitsPerSample, sizeof(waveform.wBitsPerSample), 1, fp);
    fwrite(&waveform.cbSize, sizeof(waveform.cbSize), 1, fp);
    fwrite("data", 4, 1, fp);
    fwrite(&dwDataLength, sizeof(dwDataLength), 1, fp);
    fwrite(pSaveBuffer, dwDataLength, 1, fp);
    fclose(fp);

    return 1;
}









// 混合声音   
int wavMix(const char* f1, const char* f2,const char* f3){    // =0

if(!f3) f3="mix.wav";
// FILE*fd=fopen(f3,"wb");

DIW* ps1= (DIW*)wavRead(f1); 

if(!ps1) return 0; 

DIW* ps2=(DIW*)wavRead(f2);
 
if(!ps2) return 0; 


DWORD ic=0;  



DIW*ps=ps1, *pt=ps2;    ic=ps1->dataSize;  
if(ps1->dataSize> ps2->dataSize){  ps=ps2, pt=ps1;  ic=ps2->dataSize; }  // 以数据少者为准 

float s=0.8, t=0.2;  
BYTE*p=ps->pData, *q=pt->pData;  
for(int i=0; i<ic; i++){
*p= *p*t+ *q*s;  
p++; q++; 
}  


wavWrite(ps,f3);  

free(ps);  free(pt);  

return true;  
}



void t_mix(){
wavMix("wavFile/儿童乐园.wav","wavFile/画画班.wav");  
PlaySound("mix.wav",0,SND_FILENAME); 
}