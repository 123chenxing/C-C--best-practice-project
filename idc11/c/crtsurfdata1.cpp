#include "_public.h"
CLogFile logfile;
int main(int argc,char *argv[]){
//inifile outpath logfile
if(argc!=4){
printf("Using:./crtsurfdata1 inifile outpath logfile\n");
printf("Example:/project/idc11/bin/crtsurfdata1 /project/idc11/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata1.log\n\n");
printf("inifile ȫ������վ������ļ�����\n");
printf("outpath ȫ������վ�������ļ���ŵ�Ŀ¼��\n");
printf("logfile ���������е���־�ļ�����\n\n");
return -1;  
}
if(logfile.Open(argv[3],"a+","false")==false){
printf("logfile.Open(%s) failed.\n",argv[3]); return -1;
}
logfile.Write("crtsurfdata1 ��ʼ���С�\n");
//�������ҵ�����
logfile.WriteEx("crtsurfdata1 ���н�����\n");
return 0;
}
