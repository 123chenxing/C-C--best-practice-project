/*
 * ��������demo33.cpp���˳���������ͨ�ŵĿͻ��˳���������ʾ�첽ͨ�ţ�poll��Ч�ʡ�
 * ���ߣ�����ܡ�
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo33 ip port\nExample:./demo33 192.168.174.133 5005\n\n"); return -1;
  }

  CTcpClient TcpClient;

  // �����˷�����������
  if (TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
  {
    printf("TcpClient.ConnectToServer(%s,%s) failed.\n",argv[1],argv[2]); return -1;
  }

  char buffer[102400];
  int  ibuflen=0;

  CLogFile logfile(1000);
  logfile.Open("/tmp/demo33.log","a+");

  int jj=0;

  // ������ͨѶ������һ�����ĺ�ȴ��ظ���Ȼ���ٷ���һ�����ġ�
  for (int ii=0;ii<1000000;ii++)
  {
    SPRINTF(buffer,sizeof(buffer),"���ǵ�%d������Ů�������%03d��",ii+1,ii+1);
    if (TcpClient.Write(buffer)==false) break; // �����˷��������ġ�
    logfile.Write("���ͣ�%s\n",buffer);

    // ���շ���˵Ļ�Ӧ���ġ�
    while (true)
    {
      memset(buffer,0,sizeof(buffer));
      if (TcpRead(TcpClient.m_connfd,buffer,&ibuflen,-1)==false) break;
      logfile.Write("���գ�%s\n",buffer);
      jj++;
    }
  }

  // ���շ���˵Ļ�Ӧ���ġ�
  while (jj<1000000)
  {
    memset(buffer,0,sizeof(buffer));
    if (TcpRead(TcpClient.m_connfd,buffer,&ibuflen)==false) break;
    logfile.Write("���գ�%s\n",buffer);
    jj++;
  }
}

