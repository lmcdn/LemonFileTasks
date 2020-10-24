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
	// id : ���  --  origin : Դ���  --  siz : �ļ���С������<2G�� 
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
bool deal_errors(){
	int tasknum=err_num;char durl[1005];long dsize;
	bool status=true;FILEINFO p;tasklist.push_back(p);
	for(int i=1;i<=tasknum;i++){
		sprintf(tit,"title %s:���ڴ����쳣�ļ�(ID:%d)",name,errlist[i].id);
		system(tit);
		printf("�����쳣�ļ�:%d...",errlist[i].id,errlist[i].origin);
		if(origin[errlist[i].origin].protocol==1){
			sprintf(durl,"https://%s%s",origin[errlist[i].origin].url,errlist[i].filename);
		}else if(origin[errlist[i].origin].protocol==0){
			sprintf(durl,"http://%s%s",origin[errlist[i].origin].url,errlist[i].filename);
		}else{
			printf("\n�����쳣�������˳�\n");
			return false;
		}
		download(durl,errlist[i].filename);
		dsize=file_size(errlist[i].filename);
		if(dsize!=errlist[i].siz){
			remove(errlist[i].filename);
			printf("�����쳣�������쳣����׼���ٴ���������\n");
			DeleteUrlCache(durl);
			tasklist.push_back(errlist[i]);
			status=false;
			continue;
		}else{
			printf("�ɹ�!\n");
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
	printf("���������������ļ���·��(�ļ��ڳ���Ŀ¼��������������·��):\n");
	gets(data_file);
	if(strcmp(data_file,"CON")==0){
		printf("����ȥ����ģ�\n");
		system("pause");
		return 0;
	}
	if(file_exist(data_file))printf("���ҵ��ļ�:%s\n",data_file);
	else{
		printf("�Ҳ����ļ�:%s",data_file);
		return 0;
	}
	system("title ���ڶ�ȡ��Ϣ");
	printf("��ȡ�ļ���Ϣ...\n");
	freopen(data_file,"r",stdin);
	char com[3005];bool full=true;
	while(1){
		scanf("%s",com);
		if(strcmp(com,"name")==0){
			get_quote(name);
			printf("������:%s\n",name);
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
			printf("\n��ȡ����\n");
			break;
		}
	}
	freopen("CON","r",stdin);
	printf("��ʶ��Դ%d�����ļ�%d��\n",origin_num,file_num);
	system("title �������ѡ��");
	printf("----Menu----\n");
	printf("��1����������(����˵����;ֹͣ��ʱ��һ��Ҫ���ñ��)\n");
	printf("��2����ĳ��ID��������\n");
	printf("��3��������ĳ��ID�ļ�\n");
	printf("��4���鿴ĳ��ID���ļ���Ϣ\n��������Ĭ�ϰ���1������\n���������ѡ��:");
	int choic,st=1,ed=file_num;
	scanf("%d",&choic);
	if(choic==2){
		printf("����������ɵ�ID����(e.g. ������ID57ʱ�˳���������57):");
		scanf("%d",&st);
	}else if(choic==3){
		printf("��������������ɵĵ�ID����(����ID):");
		scanf("%d",&st);
		ed=st;
		full=false;
	}else if(choic==4){
		printf("���������鿴�ļ���Ϣ���ļ�ID:");
		scanf("%d",&st);
		char durlo[1005];
		if(origin[filelist[st].origin].protocol==1){
			sprintf(durlo,"https://%s%s",origin[filelist[st].origin].url,filelist[st].filename);
		}else if(origin[filelist[st].origin].protocol==0){
			sprintf(durlo,"http://%s%s",origin[filelist[st].origin].url,filelist[st].filename);
		}else{
			printf("\n�����쳣�������˳�\n");
			return 1;
		}
		printf("----------\nID:%d\n�ļ���:%s\n���ص�ַ:%s\n��С:%ld\n----------\n",filelist[st].id,filelist[st].filename,durlo,filelist[st].siz);
		system("pause");
		return 0;
	}
	bool first_complete=true;
	for(int i=st;i<=ed;i++){
		sprintf(tit,"title %s:���������ļ�(ID:%d)",name,i);
		system(tit);
		printf("����ID:%d �ļ�:%s...",filelist[i].id,filelist[i].filename);
		char durl[1005];long dsize;
		if(origin[filelist[i].origin].protocol==1){
			sprintf(durl,"https://%s%s",origin[filelist[i].origin].url,filelist[i].filename);
		}else if(origin[filelist[i].origin].protocol==0){
			sprintf(durl,"http://%s%s",origin[filelist[i].origin].url,filelist[i].filename);
		}else{
			printf("\n�����쳣�������˳�\n");
			return 1;
		}
		download(durl,filelist[i].filename);
		dsize=file_size(filelist[i].filename);
		if(dsize!=filelist[i].siz){
			first_complete=false;
			remove(filelist[i].filename);
			printf("�����쳣�������쳣����׼����������\n");
			DeleteUrlCache(durl);
			errlist.push_back(filelist[i]);err_num++;
			continue;
		}else printf("�ɹ�!\n"); 
		DeleteUrlCache(durl);
	}
	if(!first_complete){
		printf("��ʼ�����쳣�ļ�...");
		bool complete=false;
		for(int i=1;i<=10;i++){
			Sleep(2000);
			printf("\n���е�%d/10�δ���\n",i);
			if(!deal_errors()){
				complete=false;
				printf("�����쳣�ļ�ID����:");
				for(int j=1;j<=err_num;j++){
					printf("%d ",errlist[j].id);
				}
				printf("\n");
			}else{
				complete=true;
				printf("�쳣�ļ�����ɹ�!\n");
				break;
			}
		}
		if(!complete)printf("�쳣�ļ�����ʧ�ܣ�����������������������ļ������µģ�����м�顣\n������ڹ��ܡ�4���в鿴�쳣�ļ�����Ϣ���ֶ������쳣���ļ�\n");
	}
	printf("\n������ɣ������˳���\n");
	sprintf(tit,"title %s:Done!",name);
	system(tit);
	if(full)system(after_todo);
	system("pause");
	return 0;
}
