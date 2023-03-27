/*
 * ��������demo08.cpp���˳���������ʾ����TcpServer��ʵ��socketͨѶ�ķ���ˡ�
 * ���ߣ������
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./demo08 port\nExample:./demo08 5005\n\n"); return -1;
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

  // ��ͻ���ͨѶ�����տͻ��˷������ı��ĺ󣬻ظ�ok��
  while (1)
  {
    memset(buffer,0,sizeof(buffer));
    if (TcpServer.Read(buffer)==false) break; // ���տͻ��˵������ġ�
    printf("���գ�%s\n",buffer);

    strcpy(buffer,"ok");
    if (TcpServer.Write(buffer)==false) break; // ��ͻ��˷�����Ӧ�����
    printf("���ͣ�%s\n",buffer);
  }
}
