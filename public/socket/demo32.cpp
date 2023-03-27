/*
 * ��������demo32.cpp���˳���������ͨ�ŵķ���˳���������ʾͬ��ͨ�ŵ�Ч�ʡ�
 * ���ߣ������
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./demo32 port\nExample:./demo32 5005\n\n"); return -1;
  }

  CTcpServer TcpServer;

  // ����˳�ʼ����
  if (TcpServer.InitServer(atoi(argv[1]))==false)
  {
    printf("TcpServer.InitServer(%s) failed.\n",argv[1]); return -1;
  }

  // �ȴ��ͻ��˵���������
  if (TcpServer.Accept()==false)
  {
    printf("TcpServer.Accept() failed.\n"); return -1;
  }

  printf("�ͻ��ˣ�%s�������ӡ�\n",TcpServer.GetIP());

  char buffer[102400];

  CLogFile logfile;
  logfile.Open("/tmp/demo32.log","a+");

  // ��ͻ���ͨѶ�����տͻ��˷������ı��ĺ󣬻ظ�ok��
  while (1)
  {
    memset(buffer,0,sizeof(buffer));
    if (TcpServer.Read(buffer)==false) break; // ���տͻ��˵������ġ�
    logfile.Write("���գ�%s\n",buffer);

    strcpy(buffer,"ok");
    if (TcpServer.Write(buffer)==false) break; // ��ͻ��˷�����Ӧ�����
    logfile.Write("���ͣ�%s\n",buffer);
  }
}
