#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<vector>
#include<Windows.h>
#include<wininet.h>
#include<urlmon.h>
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Urlmon.lib")
#pragma comment(lib,"wininet.lib")
#define MAXBLOCKSIZE 1024


/**-----download(const char *Url,const char *save_as)--------------------
函数功能：将指定的Url地址的文件下载到本地
函数参数：
        Url：文件所指向的网络地址
        save_as：文件保存到本地的路径地址
-------------------------------------------------------------------------**/

void download(const char *Url,const char *save_as){  
    byte Temp[MAXBLOCKSIZE];  
    ULONG Number = 1;  
    FILE *stream;  
    HINTERNET hSession=InternetOpen("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
    if(hSession!=NULL){
        HINTERNET handle2=InternetOpenUrl(hSession,Url,"cache-control: no-cache",0,INTERNET_FLAG_RELOAD,0);
        if(handle2!=NULL){
            if((stream=fopen(save_as,"wb"))!=NULL){
                while(Number>0){
                    InternetReadFile(handle2,Temp,MAXBLOCKSIZE-1,&Number);
                    fwrite(Temp,sizeof(char),Number,stream);
                }
                fclose(stream);
            }
            InternetCloseHandle(handle2);
            handle2=NULL;
        }
        InternetCloseHandle(hSession);
        hSession=NULL;
    }
}

/**-----DeleteUrlCache(char *url)--------------------
函数功能：将指定的Url地址的文件在本地的缓存删除 
函数参数：
        Url：文件所指向的网络地址
        返回值：True/False 
--------------------------------------------------**/

bool DeleteUrlCache(char *url){
	return DeleteUrlCacheEntry(url);
}

/*
功能说明：
		download()函数在下载文件后会留下缓存，不仅占用大量空间，
		而且一旦下载的某个任务出错，则会导致任务从缓存读取错误
		的内容，造成无法挽回的错误，这时候及时删除缓存就显得很
		必要了.DeleteUrlCache()最好在download()结束后立即使用. 
*/
