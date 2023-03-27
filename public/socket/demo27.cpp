/*
 * ��������demo27.cpp���˳���������ʾHTTP�ͻ��ˡ�
 * ���ߣ�����ܡ�
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo27 ip port\nExample:./demo27 www.weather.com.cn 80\n\n"); return -1;
  }

  CTcpClient TcpClient;

  // �����˷�����������
  if (TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
  {
    printf("TcpClient.ConnectToServer(%s,%s) failed.\n",argv[1],argv[2]); return -1;
  }

  char buffer[102400];
  memset(buffer,0,sizeof(buffer));

  // ����http�����ġ�
  sprintf(buffer,\
          "GET / HTTP/1.1\r\n"\
          "Host: %s:%s\r\n"\
          "\r\n",argv[1],argv[2]);

  // ��ԭ����send������http���ķ��͸�����ˡ�
  send(TcpClient.m_connfd,buffer,strlen(buffer),0);

  // ���շ���˷��ص���ҳ���ݡ�
  while (true)
  {
    memset(buffer,0,sizeof(buffer));
    if (recv(TcpClient.m_connfd,buffer,102400,0)<=0) return 0;
    printf("%s",buffer);
  }
}

