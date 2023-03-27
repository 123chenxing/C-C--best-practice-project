/*
 * execsql.cpp������һ�����߳�������ִ��һ��sql�ű��ļ���
 * ���ߣ��¹ڡ�
*/

#include "_public.h"
#include "_mysql.h"

CLogFile logfile;

connection conn;

CPActive PActive;

void EXIT(int sig);

int main(int argc,char *argv[])
{
  // �����ĵ���
  if (argc!=5)
  {
  printf("\n");
  printf("Using:./execsql sqlfile connstr charset logfile\n");

  printf("Example:/project/tools11/bin/procctl 120 /project/tools11/bin/execsql /project/idc/sql/cleardata.sql \"127.0.0.1,root,13251009668,test2,3306\" utf8 /log/idc/execsql.log\n\n");

  printf("����һ�����߳�������ִ��һ��sql�ű��ļ���\n");
  printf("sqlfile sql�ű��ļ�����ÿ��sql�����Զ�����д���ֺű�ʾһ��sql���Ľ�������֧��ע�͡�\n");
  printf("connstr ���ݿ����Ӳ�����ip,username,password,dbname,port\n");
  printf("charset ���ݿ���ַ�����\n");
  printf("logfile ���������е���־�ļ�����\n\n");

  return -1;
}

 // �ر�ȫ�����źź����������
 // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ���̡�
 // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ��
 CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
 
 // ����־�ļ���
 if (logfile.Open(argv[4],"a+")==false)
 {
    printf("����־�ļ�ʧ�� (%s) ��\n",argv[4]); return -1; 

 }

 PActive.AddPInfo(500,"obtcodetodb");     // ���̵�������ʱ�䳤�ĵ㡣

 // ע�⣬�ڵ��Գ����ʱ�򣬿��������������µĴ��룬��ֹ��ʱ��
 // PActive.AddPInfo(5000,"obtcodetodb");
 
 // �������ݿ⣬����������
 if (conn.connecttodb(argv[2],argv[3],1)!=0)
 {
   logfile.Write("connect database(%s) failed.\n%s\n",argv[2],conn.m_cda.message); return -1;
 }
 logfile.Write("connect database(%s) ok.\n",argv[2]);

 CFile File;

 // ��SQL�ļ���
 if (File.Open(argv[1],"r")==false)
 {
   logfile.Write("File.Open(%s) failed.\n",argv[1]); EXIT(-1);
 }

 char strsql[1001];     // ��Ŵ�SQL�ļ��ж�ȡ����SQL��䡣

 while (true)
 {
   memset(strsql,0,sizeof(strsql));

   // ��SQL�ļ��ж�ȡ�ԷֺŽ�����һ�С�
   if (File.FFGETS(strsql,1000,";")==false) break;

   // �����һ���ַ���#��ע�ͣ���ִ�С�
   if (strsql[0]=='#') continue;

   // ɾ����SQL������ķֺš�
   char *pp=strstr(strsql,";");
   if (pp==0) continue;
   pp[0]=0;

   logfile.Write("%s\n",strsql);

   int iret=conn.execute(strsql);   // ִ��SQL��䡣

   // ��SQL���ִ�н��д��־��
   if (iret==0) logfile.Write("exec ok(rpc=%d).\n",conn.m_cda.rpc);
   else logfile.Write("exec failed(%s).\n",conn.m_cda.message);

   PActive.UptATime();      // ���̵�������
 }

 logfile.WriteEx("\n");

 return 0;

}

void EXIT(int sig)
{
 logfile.Write("�����˳���sig=%d\n\n",sig);

 conn.disconnect();

 exit(0);

}

