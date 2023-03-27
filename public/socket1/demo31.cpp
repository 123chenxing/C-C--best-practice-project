/*
 * ��������demo31.cpp���˳���������ͨ�ŵĿͻ��˳���������ʾ�첽ͨ�ţ�����̣���Ч�ʡ�
 * ���ߣ�����ܡ�
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo31 ip port\nExample:./demo31 192.168.174.132 5005\n\n"); return -1;
  }

  CTcpClient TcpClient;

  // �����˷�����������
  if (TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
  {
    printf("TcpClient.ConnectToServer(%s,%s) failed.\n",argv[1],argv[2]); return -1;
  }

  char buffer[102400];

  CLogFile logfile(1000);
  logfile.Open("/tmp/demo31.log","a+");
int pid=fork(); 
  // ������ͨѶ������һ�����ĺ�ȴ��ظ���Ȼ���ٷ���һ�����ġ�
  for (int ii=0;ii<1000000;ii++)
  {
if (pid>0)
{
    SPRINTF(buffer,sizeof(buffer),"���ǵ�%d������Ů�������%03d��",ii+1,ii+1);
    if (TcpClient.Write(buffer)==false) break; // �����˷��������ġ�
    logfile.Write("���ͣ�%s\n",buffer);
}
else
{
    memset(buffer,0,sizeof(buffer));
    if (TcpClient.Read(buffer)==false) break; // ���շ���˵Ļ�Ӧ���ġ�
    logfile.Write("���գ�%s\n",buffer);
}
  }
}

