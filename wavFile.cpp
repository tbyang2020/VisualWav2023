

// https://blog.csdn.net/meicheng777/article/details/52449443  WAV�ļ��Ķ�д 



#pragma warning(disable:4996)


#include <Windows.h> 
#include <stdio.h> 
#include <io.h>        // filelength  
//#include <math.h> 

#define _WAVFILE_CPP_ 
#include "WAVFILE.h" 


#include "../Interface2D/qTrace.h"


// wavRead, ��ȡ wav �ļ�����.   ���ض�ȡ���ֽ�����.  
// waveOutOpen ������Ҫ���� WAVEFORMATEX,   waveOutWrite ������Ҫ��������  
// ���顢���ԡ����� wav �ļ��ṹ!

// ���� DIW*. ��������ͨ�� 

void* wavRead(const char*szFile, void*pdiw) {   // DIW diw 

FILE*fp = fopen(szFile, "rb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return 0; }


DIW*diw=(DIW*)pdiw;  
if(!diw) diw = (DIW*)malloc( sizeof(DIW) );  





int Len = filelength(fileno(fp));
qLog("\r\n%s\t  %04X Bytes\t   %.1f K\r\n", szFile,Len, Len/1024.);



int ip = 0,  ic = 0;    // ip �ļ�ָ��, =ftell. ic ����ƫ��(�����ֽ�����)   ipo ����ʱ����, ���ڼ����ȡ�ֽ���.
char st[5]={0};
RIFF_HDR rh;  
int ir=0;


CHUNK_HDR ckh;       // "RIFF"��"fmt "��"data" ...


















while(!feof(fp)){    // ���кܶ� Chunk  ...  


// "...." chunk  
ir=fread(&ckh,1, sizeof(ckh), fp);       
memcpy(st,ckh.Name,4);  
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X) \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=ir;  



if( *st==0) { 
qLog("\t  ����[]����! Header (%s, %04X) Ϊ���. ���� 0x%X �ֽ�, �����Խ� 1 �ֽ�.\r\n", st, ckh.ChunkSize, sizeof(ckh)); 
qTrace("����[%s]����! Ӧ����ֹ, ����Ե�ǰʵ���ļ�, �ɳ����ļ�ָ��+1. ",  st+1);  fseek(fp, 1-sizeof(ckh), SEEK_CUR);   ip-=(sizeof(ckh)-1);  
continue; 
}  





#if  READ_MAJOR|1


if(strnicmp(st,"RIFF",4)==0 ){     // RIFF ��Ƚ�����: chunkSize ����������, ����һ��ֻ������ "WAVE"��ʶ.    
qLog("\t   ��֤���С %04X= %04X+%04X =%04X \r\n", Len, ip, ckh.ChunkSize, ip+ckh.ChunkSize  );     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 

ir=fread(st,1, 4, fp);         
qLog("[%04X, %04X) %X\t WAVE Tag (%s) \r\n", ip, ip+ ir, ir,  st);   
ip+=ir; 

continue;  
}






if(strnicmp(st,"fmt ",4)==0 ){     // "fmt " ��: .    


#if   READ_FMT|1     // ��ȡ���ݴ�С�����ϸ���� ckh.ChunkSize  

if(ckh.ChunkSize==0x10) qLog("\t ChunkSize-0x10 = %02X-10=%02X, û�� cbSize û�� fact ��.  \r\n",   ckh.ChunkSize,  ckh.ChunkSize- 0x10);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
if(ckh.ChunkSize> 0x10) qLog("\t ChunkSize-0x10 = %02X-10=%02X, �� cbSize. cbSize>0 ʱ����չ��Ϣ, �� fact ��.  ʵ�鷢�� ChunkSize>0x10 ����  �� fact.\r\n",   ckh.ChunkSize,  ckh.ChunkSize- 0x10);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
 

// ���ǲ��Է��� ckh.ChunkSize >0x10 ����  �� fact. ?   ʵ�鷢������չ��Ϣ, ������ fact ��.  

ir=fread(&diw->wfx,1, sizeof(diw->wfx), fp);  // sizeof( WAVEFORMATEX ) = 0x12,  ������� ChunkSize, ����Ҫ seek back.     
qLog("[%04X, %04X) %X\t fmt  Content (..., %X) \t ʵ�ʴ�С�� %X \r\n", ip, ip+ ir, ir,     diw->wfx.cbSize, ckh.ChunkSize);    
ip+=ir;  


// ����:  --- �ǵ�, Ҫ�� ChunkSize Ϊ׼. 
// 1�� ���  ckh.ChunkSize = 0x10, û�� cbSize û�� fact ��, ����Ҫ��  2 �ֽ�, sizeof(wfx.cbSize)   
// 2�� ���  ckh.ChunkSize > 0x10, ���� cbSize 
//     ��ʱ���  cbSize=0, ��������� fact ��, ���  cbSize> 0, ��û��fact, ���� cbSize ��С����չ��Ϣ.   


if(ckh.ChunkSize==0x10){   //  û�� cbSize û�� fact ��, ����Ҫ��  sizeof(wfx.cbSize)=2 �ֽ�,   ���ҿ��Է�����ѭ��.  
fseek(fp,-2, SEEK_CUR);  ip-=2;  
qLog("[%04X, %04X) 2\t  ChunkSize=0x10, ��� cbSize=%X, ���� 2 �ֽڲ�������ѭ��. ע��, ������Ӧ��û�� fact ��. \r\n", ip, ip+2, diw->wfx.cbSize); 
// ip+=2;    
continue;  
}


if(ckh.ChunkSize>0x10){

if(diw->wfx.cbSize == 0){ 
qLog("\t ChunkSize=%X, �Ѷ�ȡ%X.   cbSize=0 ����չ��Ϣ, ֱ���˻���ѭ��. ע��, ������Ӧ���� fact ��. \r\n", 
ckh.ChunkSize, sizeof(diw->wfx) ); 

continue;     // ֱ�ӷ�����ѭ. ������Ӧ����  "fact" chunk.  ChunkSize>0x10, ���� cbSize=0.  
}


if(diw->wfx.cbSize > 0) {              // ��� ChunkSize>0x10, ���� cbSize> 0.  ����fact��, ���и�����Ϣ cbSize �ֽ�.
fseek(fp, diw->wfx.cbSize, SEEK_CUR);  // ��ʱ��Ӧ����֤   ChunkSize=  0x12 + cbSize 
qLog("[%04X, %04X) %X\t ��չ��Ϣ(...)\t ��֤  %02X= %02X+%02X = %02X.  ע��, ���¾�˵�� fact ��, ��ʵ�鷢��ֻҪ ChunkSize>0x10 ���� fact ��. \r\n", ip, ip+ diw->wfx.cbSize, diw->wfx.cbSize,
ckh.ChunkSize, sizeof(diw->wfx), diw->wfx.cbSize,  sizeof(diw->wfx)+ diw->wfx.cbSize );     
ip+=diw->wfx.cbSize;  

continue; 
}    

}     // if(ckh.ChunkSize>0x10)  


// wfx.cbSize=0;   // WAVEFORMATEX �� cbSize ���� 0 ?   �μ� MSDN:   
//  extra format information appended to the end of the structure. 
// for non-PCM formats to store extra attributes for the wFormatTag. 
// Note that for WAVE_FORMAT_PCM=1 formats (and only WAVE_FORMAT_PCM formats), this member is ignored. 



continue;  // ������ѭ -- ������� 
#endif     

}


if(strnicmp(st,"data",4)==0 ){


diw->dataSize= ckh.ChunkSize;
diw->pData= (BYTE*)malloc( ckh.ChunkSize ); 


ir=fread(diw->pData,1, ckh.ChunkSize, fp);       
qLog("[%04X, %04X) %X\t data Content (%04X bytes)\t  ftell=%04X  \r\n", ip, ip+ ir, ir, ir, ftell(fp));     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 


// "data" �п��������һ��. 
if( ip+ckh.ChunkSize>=Len   )  {  // && strnicmp(st,"RIFF",4)!=0     // Ϊɶ feof() û�з�Ӧ?!  
qLog("\r\n%04X + %04X = %04X >= %04X\r\n", ip, ckh.ChunkSize, ip+ ckh.ChunkSize,  Len); 
break; 
}   

ip+=ir;  
continue; 
}


#endif    // READ_MAJOR 






if(ip+ckh.ChunkSize > Len){
qTrace("����[%s] ���С����! �ļ�ָ��Խ��:  %04X + %04X =%04X >= %04X! �˳�.",  st, ip, ckh.ChunkSize,  ip+ckh.ChunkSize, Len);  
break; 
} 




// ��ʱ���� chunk content
fseek(fp, ckh.ChunkSize, SEEK_CUR);    // ��Щ ChunkSize ��¼�Ǵ�� ... ���� +1

qLog("[%04X, %04X) %X\t %s Content( %04X ). \r\n", ip, ip+ ckh.ChunkSize,  ckh.ChunkSize, st,   ckh.ChunkSize);     


// if(ip>=Len)  {   if(0)qTrace("%04X >= %04X", ip, Len);   break;   }   // Ϊɶ feof() û�з�Ӧ?! 
if( ip+ckh.ChunkSize>=Len   )  {  // && strnicmp(st,"RIFF",4)!=0     // Ϊɶ feof() û�з�Ӧ?!  
qLog("\r\n%04X + %04X = %04X >= %04X\r\n", ip, ckh.ChunkSize, ip+ ckh.ChunkSize,  Len); 
break; 
}   


ip+=ckh.ChunkSize; 
}       // while  


fclose(fp); 
  
if(0)qLog();  

return diw;
}






// д(�淶)�����ļ�. ��ѹ��, �� fact ����չ��.  bCopy = ������ pdiw ����.  

int wavWrite(DIW*diw, const char*szFile, bool DefaultFmt){  // =0L  =false

if(!diw) return false;   if( diw->dataSize<=0) return false;  
if(szFile==0L)  szFile="wavWrite.wav";  


FILE*fp = fopen(szFile, "wb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return false; }


// �����ļ�����  



CHUNK_HDR ckh={{'R','I','F','F'}, 0, };       // "RIFF"��"fmt "��"data" ...
int Len= sizeof(ckh)                 //  RIFF  ��ͷ 
    + 4                              // "WAVE" ���   
    + sizeof(ckh)                    // "fmt " ��ͷ
    + 0x10                           // "fmt " Size,  0x10 =sizeof(WAVEFORMATE) �� �ʺ��� WAVE_FORMAT_PCM =1 ��ʽ, �� ��չ��Ϣ c=bSize, �� "fact" ��. 
    + sizeof(ckh)                    // "data" ��ͷ  
    + diw->dataSize                  // ���ݳ���  
;                                    // = 8 + 0x24+diw->dataSize ,   8+4+8+0x10+8 = 8 + 0x24 = 0x2C  

ckh.ChunkSize=Len-sizeof(ckh);             // = 0x24 + diw->dataSize
// ckh.ChunkSize  = 0x24+ diw->dataSize;   // = Len - 8.  diw->dataSize ����ǰ�� 0x24+8 -8 =36 �ֽ�. ���������չ��Ϣ��fact ��, ��Ҫ > 0x24.  



qLog("\r\n%s\t  %04X Bytes\t   %.1f K\r\n", szFile,Len, Len/1024.);

char st[5]={0};  memcpy(st,ckh.Name,4);  

// д RIFF ��ͷ   
int ip=0, ir = fwrite(&ckh, 1, sizeof(ckh), fp);     assert( ir==sizeof(ckh) );  
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X)\t RIFF��ʵ�����ݽ�4�ֽ�,\"WAVE\" \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=sizeof(ckh);  

char WAVE[5]={'W','A','V','E',0};  
// д "WAVE" �ļ���ʶ big-endian 
ir = fwrite(WAVE,1,  4, fp);      assert( ir==4 ); 
qLog("[%04X, %04X) %X\t %s \r\n", ip, ip+ ir, ir,  WAVE);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=4;


// д "fmt " ��   
memcpy( ckh.Name,"fmt ",4);  memcpy(st,ckh.Name,4);  
ckh.ChunkSize=0x10;    // ��֧��  WAVE_FORMAT_PCM ��ʽ, �� cbSize, ����չ��Ϣ, �� fact ��.   
ir = fwrite(&ckh, 1, sizeof(ckh), fp);    assert( ir==sizeof(ckh) ); 
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X)\t RIFF��ʵ�����ݽ�4�ֽ�,\"WAVE\" \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=ir;


// д fmt  Content, WAVEFORMATE ����, 0x10 �ֽ�  �� WAVEFORMAT ��һ�� wBitsPerSample,  �� WAVEFORMATEX ��һ�� cbSize 
WAVEFORMATE wfe;   // �Ѿ���ʼ��Ϊ default 
if(!DefaultFmt) wfe=*(WAVEFORMATE*)&diw->wfx;   

// 3 ����Ҫָ�����ֶ� -- ����ĵ�ʽ�Ѿ�ʵ����
// wfe.wFormatTag = WAVE_FORMAT_PCM;  //   = 1;   
// wfe.nChannels = diw->wfx.nChannels;  
// wfe.wBitsPerSample  = diw->wfx.wBitsPerSample;
// wfe.nSamplesPerSec = diw->wfx.nSamplesPerSec;  

// 2 ����Ҫ������ֶ�, �������   
wfe.nAvgBytesPerSec = wfe.nChannels*wfe.nSamplesPerSec* wfe.wBitsPerSample/8; 
wfe.nBlockAlign= wfe.nChannels*wfe.wBitsPerSample/8;  

ir = fwrite(&wfe, 1, sizeof(wfe), fp);    assert( ir==sizeof(wfe) );   
qLog( "[%04X, %04X) %X\t %s  Content ( %04X ) \r\n", ip, ip+ ir, ir,  st,  sizeof(wfe) );     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=ir;

// д data ��ͷ 
memcpy( ckh.Name,"data",4);  memcpy(st,ckh.Name,4);  
ckh.ChunkSize=diw->dataSize;       
ir = fwrite(&ckh,  1, sizeof(ckh),fp);    assert( ir==sizeof(ckh) ); 
qLog("[%04X, %04X) %X\t %s  Header (%s, %04X) \r\n", ip, ip+ ir, ir,  st, st, ckh.ChunkSize);     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=ir;

// д data Content 
ir= fwrite(diw->pData, 1, diw->dataSize, fp);  assert(ir == diw->dataSize);
qLog( "[%04X, %04X) %X\t %s  Content ( %04X ) \r\n", ip, ip+ ir, ir,  st,  diw->dataSize );     //   ��� ChunkSize>0x10, ���и�����Ϣ cbSize �ֽ�. 
ip+=ir;


fclose(fp); qLog();  

return ip;  
}





void t_rw(const char*wavf){
DIW diw;  
DIW* pdiw=(DIW*)wavRead( wavf, &diw);  if(!pdiw) return;  
wavWrite(&diw,  "wavFile/t_�Ұ����й�.wav"); 
}



















bool wav_read(const char*szFile, WAVDATA&wd) {

FILE*fp = fopen(szFile, "rb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return false; }

int fLen = filelength(fileno(fp));
sLog("\r\n%s\r\n%X / %d  BYTES\r\n", szFile, fLen, fLen);


    
int ip = 0, ic = 0;    // ip �ļ�ָ��, =ftell. ic ����ƫ��(�����ֽ�����) 

WavHeader wt;  
int ir=fread(&wt,sizeof(wt),1, fp);   assert(ir==1); 
char* p = (char*)wt.RIFF;   char c = *(p + 4);    *(p + 4) = 0;
ic= wt.ChunkSize +8;    // +8, ����Chunkͷ��8�ֽ�. 
sLog("\r\n[%04X, %04X) %X\t [%s]   \r\n", ip, ip+ ic, ic, 
wt.RIFF);    *(p + 4) = c;
ip+= sizeof(wt);   

sLog("\r\nChunkSize\t%X\r\nAudioFmt\t%X\r\nChannels\t%X\r\nSampleRate\t%X\r\nByteRate\t%X\r\nBlockAlign\t%X\r\nBitsPerSample\t%X\r\n", 
 wt.ChunkSize, wt.AudioFmt, wt.Channels,  wt.SampleRate, wt.ByteRate, wt.BlockAlign, wt.BitsPerSample);


sLog("\r\n��֤ BlockAlign\t %d = %d\r\n", wt.BlockAlign, wt.Channels * wt.BitsPerSample / 8);
sLog("��֤ ByteRate\t %d = %d\r\n", wt.ByteRate, wt.SampleRate*wt.Channels * wt.BitsPerSample / 8);
// sLog("\r\n��֤ SubCk2Size %X = %X\r\n", wav->SubCk2Size, wav->dwSamples*wav->Channels * wav->BitsPerSample / 8);




//WavFile wf; 


char ckName[5]; ckName[4] = 0;  // β��ʼ��Ϊ \0
DWORD ckSize;


p = (char*)wt.fmt;   c = *(p + 4);   
ic =  8+ wt.SubCk1Size;   *(p + 4) = 0;
sLog("\r\n[%04X, %04X)\t%X\t [%s]  \r\n", 12, 12 + ic, ic,
wt.fmt);    *(p + 4) = c;




if( wt.SubCk1Size > 0x10) {  // ��Ҫ��ȡ  ex_size. Ҳ���������ļ�ָ��  wt.SubCk1Size - 0x10 

WORD ex_size=0;   
ir = fread(&ex_size, sizeof(ex_size), 1, fp);  assert(ir == 1);
    sLog("\r\n[%04X]\t ex_size  %X\r\n", ip, ex_size);
    ip += sizeof(ex_size);

// if (wf.ex_size > 0) { fseek(fp, wf.ex_size, SEEK_CUR); ip += wf.ex_size; }  // <--- ������� ex_size �ĺ���. 

if(wt.SubCk1Size > 0x12) { fseek(fp, wt.SubCk1Size -0x12, SEEK_CUR); ip += wt.SubCk1Size - 0x12; MessageBox(0,"?",0,0); }  // "fmt " �ǲ��������һ�� WORD ex_size, �в��϶�


DWORD dwSamples=0;   // ע��, sizeof(ckName) ����1. 
ir = fread(ckName, 4, 1, fp);  assert(ir == 1);  // "fact"  
ir = fread(&ckSize, sizeof(ckSize), 1, fp);  assert(ir == 1);
ir = fread(&dwSamples, sizeof(dwSamples), 1, fp);  assert(ir == 1);

ic= 4+ sizeof(ckSize)+ sizeof(dwSamples);

sLog("\r\n[%04X, %04X)\t%X\t [%s]\r\n", ip, ip+ic, ic, ckName);  
ip += ic;

if(ckSize>4){ fseek(fp, ckSize - 4, SEEK_CUR); ip += ckSize - 4; }    // Ӧ�ò��ᷢ��  

}  // if 



// "data" ��֮����ܻ���������  

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



if(memcmp(ckName,"data",4)==0){  // ֻ������������  

wd.dataSize= ckSize;
wd.ps=(BYTE*)realloc(wd.ps, ckSize);  // new BYTE[ckSize];  
fread(wd.ps, ckSize,1,fp);

}else fseek(fp, ckSize,SEEK_CUR);



// �ݴ���. ��Щ�ļ��� ChunkSize �Ǵ��. 
#if FAULT_TOLERANT|1

char c=0;   // 0x20 �C 0x7E 
while (!feof(fp) && !isprint(c)) fread(&c, 1, 1, fp);

if(isprint(c)) fseek(fp,-1,SEEK_CUR);  // ��Ϊ���ٶ�һ�� 

#endif   // FAULT_TOLERANT 

}  // while  



//sLog("\r\n��֤ SubCk2Size %X = %X\r\n", wd.dataSize, dwSamples*wd.Channels * wd.BitsPerSample / 8);  // ����û�� "fact"

// ir = ftell(fp);  sLog("��֤ FileLength = %X\t  ir=%X\t  ��=%X   \r\n", fLen, ir, fLen - ir);



_EXIT_:
    fclose(fp);
    return true;
}









#if  SIMPLE_PLAY|1

//  ReadFile,  loadAudioBlock, see   https://blog.csdn.net/xbk123123/article/details/53787287

// ȫ����ȡ -- û���ô� 
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



// ����һЩ bug: ������ϲ��˳�?  

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
waveForm.nAvgBytesPerSec = waveForm.nBlockAlign * waveForm.nSamplesPerSec;    //  4* 44100     ÿ��  ������ �� ������ �� �����ֽ���   
		


#endif    // DEFAULT_WFX
	
	
HWAVEOUT hWaveOut; 
//if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveForm, 0, 0, CALLBACK_NULL) !=MMSYSERR_NOERROR)	{ MessageBox(0, "��Ƶ�豸��ʧ��", 0, MB_OK); return FALSE; }
if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &diw.wfx, 0, 0, CALLBACK_NULL) !=MMSYSERR_NOERROR)	{ MessageBox(0, "��Ƶ�豸��ʧ��", 0, MB_OK); return FALSE; }
	



// LPSTR block;  DWORD blockSize;    \
if((block = ReadFile(soundFileName, &blockSize)) ==NULL){ MessageBox(0,"�޷�����Ƶ�ļ�",0, MB_OK); return FALSE; }






// writeWavBlock(hWaveOut, block, blockSize); 
writeWavBlock(hWaveOut, (LPSTR)diw.pData, diw.dataSize); 
	

waveOutClose(hWaveOut);
	
return TRUE;
}




#endif    // SIMPLE_PLAY 
























// uint8 ת int8, -128~127    

// b+~b&0xFF= 0xFF 

int b_int8(BYTE b) {  // BYTE 0~255  

    int i = b & (1 << 7);
    if (i == 0) return b;

    return -(1 + ~b & 0xFF);   // ȡ�� + 1 


    int r = b;
    if (r > 127) r -= 256;
    return r;
}




// w_int16, WORD ת��Ϊ 16 λ�з�������. 

// Ϊ�� 16 λ int ���ͷ�Χ�� [-32768, 32767] 

// w+~w&0xFFFF= 0xFFFF 

int w_int16(WORD w) {

    int i = w & (1 << 15);

    if (i == 0) return w;

    //  3 ����ֵ�� 0000 0011, ��3 ��Ϊ 11111100��1 �� 11111101

    //  32767 �� 0111 1111  1111 1111 (15��1), -32767�� 1000 0000  0000 0000 + 1 = 1000 0000  0000 0001  (ȡ����1)

    // -32768 �� 0111 1111  1111 1111 + 1 = 1000 0000  0000 0000

    return -(1 + ~w & 0xFFFF);   // ȡ�� + 1 
};


// W+~W&0xFFFFFFFF= 0xFFFFFFFF 

int dw_int32(DWORD W) {
    return (int)W;
    if ((0 == (1 << 31)&W)) return W;
    //return -( 1 + ~W&0xFFFFFFFF );  
    return (1 << 32) - W;
}







// _wav_read, �����ȡ wav �ļ�����. ���ܶ�����ĳЩ��.  

bool _wav_read(const char*szFile, wave_t*wav=0){

if (!wav)  wav=new wave_t; 



//qTrace("ChunkSize=%d, SubCk1Size=%d,  RIFF=%s, data=%s",  \
wav->ChunkSize, wav->SubCk1Size, wav->RIFF, wav->data );

FILE*fp=fopen(szFile,"rb");
if(!fp) { qTrace("%s\r\n%s", strerror(errno),szFile);  return false; }

int fLen= filelength( fileno(fp) );
sLog("%s\t %X / %d  BYTES\r\n", szFile, fLen, fLen);


int ip=0, ic=0;    // �ļ�ָ�롢����ƫ�� 

int ir=fread(wav->RIFF,sizeof(wav->RIFF),1,fp);  assert(ir==1); 
char* p=  (char*) wav->RIFF;   char c=*(p+4);    *(p + 4) = 0;
sLog("\r\n[%04X]\t \"%s\"\r\n", ip,  wav->RIFF);  *(p + 4) = c;
ip+= sizeof(wav->RIFF);  


ir = fread(&wav->ChunkSize, sizeof(wav->ChunkSize), 1, fp);  assert(ir == 1);
sLog("[%04X]\t ChunkSize  %X\r\n", ip, wav->ChunkSize );  // + 8 Ϊ�ļ���С 
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


sLog("\r\n��֤ BlockAlign %d = %d ... \r\n", wav->BlockAlign, wav->Channels * wav->BitsPerSample / 8);
sLog("��֤ ByteRate %d = %d\r\n", wav->ByteRate, wav->SampleRate*wav->Channels * wav->BitsPerSample / 8);





// fseek(fp,2,SEEK_CUR); 
// ir = fread(wav->data, sizeof(wav->data), 1, fp);  assert(ir == 1);
// sLog("\r\ndata=%s\r\n", wav->data);


if(wav->SubCk1Size!=16){    // ��Ҫ��չ��... Ϊʲô ex_sizeһ������?  

ir = fread(&wav->ex_size, sizeof(wav->ex_size), 1, fp);  assert(ir == 1);
sLog("\r\n[%04X]\t ex_size  %X\r\n", ip,wav->ex_size);
ip+=sizeof(wav->ex_size); 

if(wav->ex_size>0){ fseek(fp,wav->ex_size,SEEK_CUR); ip+= wav->ex_size;  }



ir = fread(wav->fact, sizeof(wav->fact), 1, fp);  assert(ir == 1);
p = (char*)wav->fact;  c = *(p + 4);  *(p + 4) = 0;
sLog("\r\n[%04X]\t [%s]\r\n", ip, wav->fact); *(p + 4) = c;
ip+=sizeof(wav->fact);


if( 0!=memcmp(wav->fact,"fact",4) ){  fseek(fp, wav->ex_size-4,SEEK_CUR); ip += wav->ex_size-4;  }  // ������չ�� 
else{  // �� fact ��  

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
sLog("\r\n[%04X]\t [��%s]\r\n", ip,wav->data); *(p + 4) = c;
ip+=sizeof(wav->data);


if (0 != memcmp(wav->data, "data", 4)) {  qTrace("��ȡ����!û����ȷ��λ��'data'��.");   goto _EXIT_; }
   

ir = fread(&wav->SubCk2Size, sizeof(wav->SubCk2Size), 1, fp);  assert(ir == 1);
sLog("[%04X]\t SubCk2Size  %X\r\n", ip, wav->SubCk2Size);   // +0x2c ? Ϊ�ļ���С 
ip+=sizeof(wav->SubCk2Size);

wav->DATA=new BYTE[wav->SubCk2Size];

ir = fread(wav->DATA, 1,  wav->SubCk2Size, fp);  assert(ir == wav->SubCk2Size);
ip+= wav->SubCk2Size; 



if(wav->dwSamples<=0)   wav->dwSamples= wav->SubCk2Size / wav->Channels / (wav->BitsPerSample / 8);

sLog("\r\n��֤ SubCk2Size %X = %X\r\n", wav->SubCk2Size, wav->dwSamples*wav->Channels * wav->BitsPerSample / 8);


ir=ftell(fp); 

sLog("��֤ FileLength = %X\t  ir=%X, ip=%X\t  ��=%X   \r\n", fLen,  ir, ip, fLen-ip);






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




// д(�淶)�����ļ�. ��ѹ��, �� fact ����չ�� 
  

bool wav_write(const char*szFile, WAVDATA*wd, bool bCopy=false){

if(!wd) return false; 

FILE*fp = fopen(szFile, "wb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), szFile);  return false; }

WavHeader wt;

// 3����Ҫָ�����ֶ�
// wt.AudioFmt  = 1 
wt.Channels=wd->Channels;  
wt.BitsPerSample = wd->BitsPerSample;
wt.SampleRate    = wd->SampleRate; 

// 3����Ҫ������ֶ� ChunkSize ...  

wt.ByteRate   = wd->SampleRate*wd->Channels * wd->BitsPerSample / 8;
wt.BlockAlign = wd->Channels * wd->BitsPerSample / 8;

wt.ChunkSize  = 0x24+ wd->dataSize;   // ����ǰ�� 36+8 -8 =36 =0x24 �ֽ�. ����� fact, һ���� 50=0x32, ���12+2=14�ֽ�. 

// qTrace("ChunkSize=%X, ByteRate=%X, BlockAlign=%X", wt.ChunkSize, wt.ByteRate, wt.BlockAlign);


int ir = fwrite(&wt, sizeof(wt), 1, fp);   assert(ir == 1);



// д data �� 
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

wav_write("t_��ͯ��԰.wav",&wd); 


    
sLog(0);
}







// λͼ��=ʱ��(����), �� ��Ӧ����������/ÿ���������� 

int get_sd(int i, WavData*wd, int cs ) {

    if (!wd) return 0;

    static int n = wd->dataSize * 8 / wd->BitsPerSample / wd->Channels;  // dwSamples 

    if (i<0 || i >= n) return 0;

    BYTE*p = wd->ps;
    //BYTE* q= p + 2 * i * wd->Channels + cs;    // q >>= (wd->BitsPerSample-8);  // ������0���� ...... 

    if (wd->BitsPerSample <= 8) { BYTE v = *(p + i * wd->Channels + cs);   v >>= (8 - wd->BitsPerSample);   return v; }


    if (8 < wd->BitsPerSample && wd->BitsPerSample <= 16) { WORD v = *(WORD*)(p + 2 * i * wd->Channels + cs);   v >>= (16 - wd->BitsPerSample);   return w_int16(v); }

    if (16 < wd->BitsPerSample && wd->BitsPerSample <= 24) { DWORD v = *(DWORD*)(p + 3 * i * wd->Channels + cs);  v &= 0x00FFFFFF;  v >>= (24 - wd->BitsPerSample);   return w_int16(v); }

    if (24 < wd->BitsPerSample && wd->BitsPerSample <= 32) { DWORD v = *(DWORD*)(p + 4 * i * wd->Channels + cs);  v >>= (32 - wd->BitsPerSample);   return dw_int32(v); }

    MessageBox(0, "����bitֵ����!", 0, 0);
    return 0;
}










// wav_cx, ���� wav �ļ���������  2-->1, [���෴--tbc].  ����ԭwav�� Channels, �� ����ֵ 0.   


int wav_cx(WAVDATA&wd, const char*zf) {

FILE*fp = fopen(zf, "wb");
if (!fp) { qTrace("%s\r\n%s", strerror(errno), zf);  return false; }

if(wd.dataSize<=0) return 0;
if(wd.Channels <=0) return 0;
if(wd.BitsPerSample <=0) return 0;

int BS= wd.BitsPerSample / 8;    // BytesPerSample  
if(BS*8<wd.BitsPerSample) BS++; 


DWORD dwSamples = wd.dataSize / BS / wd.Channels; 
 
int Channels=1;  // ���������� 
DWORD dataSize= dwSamples* BS*1;   // ���¼���  

//qTrace("wd.dataSize=%d, dataSize=%d", wd.dataSize, dataSize);

WavHeader wt;

// 3����Ҫָ�����ֶ�  // wt.AudioFmt  = 1 
wt.Channels = 1;
wt.BitsPerSample = wd.BitsPerSample;
wt.SampleRate = wd.SampleRate;

// 3����Ҫ������ֶ� ChunkSize ...  

wt.ByteRate = wd.SampleRate* Channels * BS;
wt.BlockAlign = Channels * BS;

wt.ChunkSize = 0x24 + dataSize;   // ����ǰ�� 36+8 -8 =36 =0x24 �ֽ�. ����� fact, һ���� 50=0x32, ���12+2=14�ֽ�. 

// qTrace("ChunkSize=%X, ByteRate=%X, BlockAlign=%X", wt.ChunkSize, wt.ByteRate, wt.BlockAlign);


int ir = fwrite(&wt, sizeof(wt), 1, fp);   assert(ir == 1);



// д data �� 
char ckName[4] = { 'd','a','t','a' };
ir = fwrite(ckName, 1, 4, fp);  assert(ir == 4);

ir= fwrite(&dataSize, 1, 4, fp);  assert(ir == 4);



#if WRITE_DATA|1

int cs=0;  // 0 ����  
BYTE*p= wd.ps+cs*BS;  

int v=0;
//DWORD v=0; // DWORD msk=(1<<BS*8);  

assert(BS==sizeof(WORD));

for(int i=0; i< dwSamples; i++){
// v=get_sd(i,&wd,0);   // 0-�������� 
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

    wav_cx(wd,"t_��ͯ��԰cx.wav");



    sLog(0);
}
















int wavSave(WAVEFORMATEX& waveform, BYTE* pSaveBuffer, DWORD dwDataLength, const char*szF="t_audio.wav") {

    // ��¼����������Ϊwav�ļ�����: https://blog.csdn.net/benny5609/article/details/1915135

    // �Ʊ�д��Ϣ�����������У�MM_WIM_DATA �����Ǳ�����ĺ��ġ�����Ҫ�����ǽ������������б�����һ�������ڣ�pSaveBuffer�����û������ĳ��Ƚ�������¼�����ݵĴ�С�����ӣ��Ӷ�ʵ�ֱ������뻰�����ݵĹ��ܡ�ͬʱ���ɽ������������ݱ���Ϊwav�ļ��������ʵ�����£�
    FILE* fp = fopen(szF, "wb");    //����·��
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









// �������   
int wavMix(const char* f1, const char* f2,const char* f3){    // =0

if(!f3) f3="mix.wav";
// FILE*fd=fopen(f3,"wb");

DIW* ps1= (DIW*)wavRead(f1); 

if(!ps1) return 0; 

DIW* ps2=(DIW*)wavRead(f2);
 
if(!ps2) return 0; 


DWORD ic=0;  



DIW*ps=ps1, *pt=ps2;    ic=ps1->dataSize;  
if(ps1->dataSize> ps2->dataSize){  ps=ps2, pt=ps1;  ic=ps2->dataSize; }  // ����������Ϊ׼ 

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
wavMix("wavFile/��ͯ��԰.wav","wavFile/������.wav");  
PlaySound("mix.wav",0,SND_FILENAME); 
}