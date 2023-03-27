/*
 * ��������demo26.cpp���˳�����ʾHTTPЭ�飬����http�����ġ�
 * ���ߣ������
*/
#include "../_public.h"
 
// ��html�ļ������ݷ��͸��ͻ��ˡ�
bool SendHtmlFile(const int sockfd,const char *filename);

int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./demo26 port\nExample:./demo26 8080\n\n"); return -1;
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
  memset(buffer,0,sizeof(buffer));

  // ����http�ͻ��˷��͹����ı��ġ�
  recv(TcpServer.m_connfd,buffer,1000,0);

  printf("%s\n",buffer);

  // �Ȱ���Ӧ����ͷ�����͸��ͻ��ˡ�
  memset(buffer,0,sizeof(buffer));
  sprintf(buffer,\
         "HTTP/1.1 200 OK\r\n"\
         "Server: demo26\r\n"\
         "Content-Type: text/html;charset=utf-8\r\n"\
         "Content-Length: 108909\r\n\r\n");
  if (Writen(TcpServer.m_connfd,buffer,strlen(buffer))== false) return -1;

  //logfile.Write("%s",buffer);

  // �ٰ�html�ļ������ݷ��͸��ͻ��ˡ�
  SendHtmlFile(TcpServer.m_connfd,"index.html");
}

// ��html�ļ������ݷ��͸��ͻ��ˡ�
bool SendHtmlFile(const int sockfd,const char *filename)
{
  int  bytes=0;
  char buffer[5000];

  FILE *fp=NULL;

  if ( (fp=FOPEN(filename,"rb")) == NULL ) return false;

  while (true)
  {
    memset(buffer,0,sizeof(buffer));

    if ((bytes=fread(buffer,1,5000,fp)) ==0) break;

    if (Writen(sockfd,buffer,bytes) == false) { fclose(fp); fp=NULL; return false; }
  }

  fclose(fp);

  return true;
}
