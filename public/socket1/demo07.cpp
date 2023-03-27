/*
 * ��������demo07.cpp���˳���������ʾ����TcpClient��ʵ��socketͨѶ�Ŀͻ��ˡ�
 * ���ߣ�����ܡ�
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo07 ip port\nExample:./demo07 127.0.0.1 5005\n\n"); return -1;
  }

  CTcpClient TcpClient;

  // �����˷�����������
  if (TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
  {
    printf("TcpClient.ConnectToServer(%s,%s) failed.\n",argv[1],argv[2]); return -1;
  }

  char buffer[102400];
 
  // ������ͨѶ������һ�����ĺ�ȴ��ظ���Ȼ���ٷ���һ�����ġ�
  for (int ii=0;ii<100000;ii++)
  {
    SPRINTF(buffer,sizeof(buffer),"���ǵ�%d������Ů�������%03d��",ii+1,ii+1);
    if (TcpClient.Write(buffer)==false) break; // �����˷��������ġ�
    printf("���ͣ�%s\n",buffer);

    memset(buffer,0,sizeof(buffer));
    if (TcpClient.Read(buffer)==false) break; // ���շ���˵Ļ�Ӧ���ġ�
    printf("���գ�%s\n",buffer);

    sleep(1);  // ÿ��һ����ٴη��ͱ��ġ�
  }
}

