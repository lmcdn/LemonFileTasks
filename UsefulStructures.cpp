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
�������ܣ���ָ����Url��ַ���ļ����ص�����
����������
        Url���ļ���ָ��������ַ
        save_as���ļ����浽���ص�·����ַ
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
�������ܣ���ָ����Url��ַ���ļ��ڱ��صĻ���ɾ�� 
����������
        Url���ļ���ָ��������ַ
        ����ֵ��True/False 
--------------------------------------------------**/

bool DeleteUrlCache(char *url){
	return DeleteUrlCacheEntry(url);
}

/*
����˵����
		download()�����������ļ�������»��棬����ռ�ô����ռ䣬
		����һ�����ص�ĳ�����������ᵼ������ӻ����ȡ����
		�����ݣ�����޷���صĴ�����ʱ��ʱɾ��������Եú�
		��Ҫ��.DeleteUrlCache()�����download()����������ʹ��. 
*/
