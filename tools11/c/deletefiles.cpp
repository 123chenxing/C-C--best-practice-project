#include "_public.h"
//�����˳����ź�2��15�Ĵ�����
void EXIT(int sig);
int main(int argc,char *argv[]){
//����İ���
if(argc!=4){
printf("\n");
printf("Using:/project/tools11/bin/deletefiles pathname matchstr timeout\n\n");
printf("Example:/project/tools11/bin/deletefiles /log/idc \"*.log.20*\" 0.02\n");
printf("	/project/tools11/bin/deletefiles /tmp/idc/surfdata \"*.xml,*.json\" 0.01\n");
printf("	/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /log/idc \"*.log.20*\" 0.02\n");
printf("	/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /tmp/idc/surfdata \"*.xml,*.json\" 0.01\n\n");
printf("����һ�����߳�������ɾ����ʷ�������ļ�����־�ļ���\n");
printf("�������pathnameĿ¼����Ŀ¼��timeout��֮ǰ��ƥ��matchstr�ļ�ȫ��ɾ����timeout������С����\n");
printf("������д��־�ļ���Ҳ�����ڿ���̨����κ���Ϣ��\n\n\n");
return -1;
}
//�ر�ȫ�����źź��������
//CloseIOAndSignal(true);
signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
//��ȡ�ļ���ʱ��ʱ���
char strTimeOut[21];
LocalTime(strTimeOut,"yyyy-mm-dd hh24:mi:ss",0-(int)(atof(argv[3])*24*60*60));
CDir Dir;
//��Ŀ¼��CDir.OpenDir()
if(Dir.OpenDir(argv[1],argv[2],10000,true)==false){
printf("Dir.OpenDir(%s) failed.\n",argv[1]); return -1;
}
//����Ŀ¼�е��ļ���
while(true){
//�õ�һ���ļ�����Ϣ��CDir.ReadDir()
if(Dir.ReadDir()==false) break;
printf("FullFileName=%s\n",Dir.m_FullFileName);

//�볬ʱ��ʱ���Ƚϣ�������磬����Ҫѹ��
if(strcmp(Dir.m_ModifyTime,strTimeOut)<0){
if(REMOVE(Dir.m_FullFileName)==true)
printf("REMOVE %s ok.\n",Dir.m_FullFileName);
else
printf("REMOVE %s failed.\n",Dir.m_FullFileName);
   }
}
return 0;
}
void EXIT(int sig){
printf("�����˳���sig=%d\n\n",sig);
exit(0);

}
