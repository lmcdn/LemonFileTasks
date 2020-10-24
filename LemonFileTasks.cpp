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
using namespace std;
char data_file[1002],name[1002];
char tit[202],after_todo[1005]; 
int origin_num,file_num,err_num=0;
struct ORIGININFO{
	int protocol,id;
	// HTTP : 0   --   HTTPS : 1
	char url[1005]; 
};
struct FILEINFO{
	// id : 编号  --  origin : 源编号  --  siz : 文件大小（建议<2G） 
	int id,origin;long siz;
	char filename[1000];
};
vector<ORIGININFO>origin;
vector<FILEINFO>filelist,errlist,tasklist;
bool file_exist(const char *path){
	if(FILE *file = fopen(path,"r")) {
    	fclose(file);
        return true;
    }else return false;   
}
long file_size(char *path){
	FILE *file;long rt;
	if(!file_exist(path)){
		return -1;
	}
	file=fopen(path,"rb");
	fseek(file,0,SEEK_END);
	rt=ftell(file);
	fclose(file);
	return rt;
}
bool DeleteUrlCache(char *url){
	return DeleteUrlCacheEntry(url);
}
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
bool deal_errors(){
	int tasknum=err_num;char durl[1005];long dsize;
	bool status=true;FILEINFO p;tasklist.push_back(p);
	for(int i=1;i<=tasknum;i++){
		sprintf(tit,"title %s:正在处理异常文件(ID:%d)",name,errlist[i].id);
		system(tit);
		printf("处理异常文件:%d...",errlist[i].id,errlist[i].origin);
		if(origin[errlist[i].origin].protocol==1){
			sprintf(durl,"https://%s%s",origin[errlist[i].origin].url,errlist[i].filename);
		}else if(origin[errlist[i].origin].protocol==0){
			sprintf(durl,"http://%s%s",origin[errlist[i].origin].url,errlist[i].filename);
		}else{
			printf("\n出现异常，程序退出\n");
			return false;
		}
		download(durl,errlist[i].filename);
		dsize=file_size(errlist[i].filename);
		if(dsize!=errlist[i].siz){
			remove(errlist[i].filename);
			printf("下载异常！加入异常队列准备再次重新下载\n");
			DeleteUrlCache(durl);
			tasklist.push_back(errlist[i]);
			status=false;
			continue;
		}else{
			printf("成功!\n");
			err_num--;
		}
		DeleteUrlCache(durl);
	}
	swap(errlist,tasklist);
	tasklist.clear();
	return status;
}
void get_quote(char *str){
	char in;int loc=0;bool status=0;
	while(1){
		in=getchar();
		if(in==EOF){
			status=false;break;
		}else if(in=='\"'){
			status=true;break;
		}
	}
	if(status==false)return;
	while(1){
		in=getchar();
		if(in==EOF)break;
		else if(in=='\"')break;
		str[loc++]=in;
	}
	return;
}
int main(){
	printf("请输入下载配置文件的路径(文件在程序本目录下则可以输入相对路径):\n");
	gets(data_file);
	if(strcmp(data_file,"CON")==0){
		printf("滚！去你妈的！\n");
		system("pause");
		return 0;
	}
	if(file_exist(data_file))printf("已找到文件:%s\n",data_file);
	else{
		printf("找不到文件:%s",data_file);
		return 0;
	}
	system("title 正在读取信息");
	printf("读取文件信息...\n");
	freopen(data_file,"r",stdin);
	char com[3005];bool full=true;
	while(1){
		scanf("%s",com);
		if(strcmp(com,"name")==0){
			get_quote(name);
			printf("程序名:%s\n",name);
		}else if(strcmp(com,"origins")==0)scanf("%d",&origin_num);
		else if(strcmp(com,"file_number")==0)scanf("%d",&file_num);
		else if(strcmp(com,"origin")==0){
			ORIGININFO p;origin.push_back(p);
			for(int i=1;i<=origin_num;i++){
				ORIGININFO p;
				scanf("%d%d",&p.id,&p.protocol);
				scanf("%s",p.url);
				origin.push_back(p);
			}
		}else if(strcmp(com,"files")==0){
			FILEINFO p;filelist.push_back(p);
			errlist.push_back(p);
			for(int i=1;i<=file_num;i++){
				FILEINFO p;
				scanf("%d",&p.id);
				scanf("%s",p.filename);
				scanf("%d%d",&p.origin,&p.siz);
				filelist.push_back(p);
			}
		}else if(strcmp(com,"after")==0){
			get_quote(after_todo);
		}else if(strcmp(com,"end")==0){
			printf("\n读取结束\n");
			break;
		}
	}
	freopen("CON","r",stdin);
	printf("共识别源%d个，文件%d个\n",origin_num,file_num);
	system("title 做出你的选择");
	printf("----Menu----\n");
	printf("【1】完整下载(所以说，中途停止的时候一定要看好编号)\n");
	printf("【2】从某个ID接着下载\n");
	printf("【3】仅下载某个ID文件\n");
	printf("【4】查看某个ID的文件信息\n其他数字默认按【1】处理\n请输入你的选择:");
	int choic,st=1,ed=file_num;
	scanf("%d",&choic);
	if(choic==2){
		printf("请输入已完成的ID任务(e.g. 在下载ID57时退出，请输入57):");
		scanf("%d",&st);
	}else if(choic==3){
		printf("请输入欲单独完成的的ID任务(填入ID):");
		scanf("%d",&st);
		ed=st;
		full=false;
	}else if(choic==4){
		printf("请输入欲查看文件信息的文件ID:");
		scanf("%d",&st);
		char durlo[1005];
		if(origin[filelist[st].origin].protocol==1){
			sprintf(durlo,"https://%s%s",origin[filelist[st].origin].url,filelist[st].filename);
		}else if(origin[filelist[st].origin].protocol==0){
			sprintf(durlo,"http://%s%s",origin[filelist[st].origin].url,filelist[st].filename);
		}else{
			printf("\n出现异常，程序退出\n");
			return 1;
		}
		printf("----------\nID:%d\n文件名:%s\n下载地址:%s\n大小:%ld\n----------\n",filelist[st].id,filelist[st].filename,durlo,filelist[st].siz);
		system("pause");
		return 0;
	}
	bool first_complete=true;
	for(int i=st;i<=ed;i++){
		sprintf(tit,"title %s:正在下载文件(ID:%d)",name,i);
		system(tit);
		printf("下载ID:%d 文件:%s...",filelist[i].id,filelist[i].filename);
		char durl[1005];long dsize;
		if(origin[filelist[i].origin].protocol==1){
			sprintf(durl,"https://%s%s",origin[filelist[i].origin].url,filelist[i].filename);
		}else if(origin[filelist[i].origin].protocol==0){
			sprintf(durl,"http://%s%s",origin[filelist[i].origin].url,filelist[i].filename);
		}else{
			printf("\n出现异常，程序退出\n");
			return 1;
		}
		download(durl,filelist[i].filename);
		dsize=file_size(filelist[i].filename);
		if(dsize!=filelist[i].siz){
			first_complete=false;
			remove(filelist[i].filename);
			printf("下载异常！加入异常队列准备重新下载\n");
			DeleteUrlCache(durl);
			errlist.push_back(filelist[i]);err_num++;
			continue;
		}else printf("成功!\n"); 
		DeleteUrlCache(durl);
	}
	if(!first_complete){
		printf("开始处理异常文件...");
		bool complete=false;
		for(int i=1;i<=10;i++){
			Sleep(2000);
			printf("\n进行第%d/10次处理\n",i);
			if(!deal_errors()){
				complete=false;
				printf("尚余异常文件ID队列:");
				for(int j=1;j<=err_num;j++){
					printf("%d ",errlist[j].id);
				}
				printf("\n");
			}else{
				complete=true;
				printf("异常文件处理成功!\n");
				break;
			}
		}
		if(!complete)printf("异常文件处理失败，可能是网络问题或者配置文件错误导致的，请进行检查。\n你可以在功能【4】中查看异常文件的信息，手动下载异常的文件\n");
	}
	printf("\n任务完成，程序退出。\n");
	sprintf(tit,"title %s:Done!",name);
	system(tit);
	if(full)system(after_todo);
	system("pause");
	return 0;
}
