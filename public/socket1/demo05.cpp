/*
 * ��������demo05.cpp���˳���������ʾ��ճ����socket�ͻ��ˡ�
 * ���ߣ�����ܡ�
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo05 ip port\nExample:./demo05 127.0.0.1 5005\n\n"); return -1;
  }

  // ��1���������ͻ��˵�socket��
  int sockfd;
  if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) { perror("socket"); return -1; }
 
  // ��2�����������������������
  struct hostent* h;
  if ( (h = gethostbyname(argv[1])) == 0 )   // ָ������˵�ip��ַ��
  { printf("gethostbyname failed.\n"); close(sockfd); return -1; }
  struct sockaddr_in servaddr;
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2])); // ָ������˵�ͨѶ�˿ڡ�
  memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);
  if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)  // �����˷�����������
  { perror("connect"); close(sockfd); return -1; }

  char buffer[1024];
 
  // ��3����������ͨѶ����������1000�����ġ�
  for (int ii=0;ii<1000;ii++)
  {
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"���ǵ�%d������Ů�������%03d��",ii+1,ii+1);

    if (TcpWrite(sockfd,buffer,strlen(buffer))==false) break; // �����˷��������ġ�

    printf("���ͣ�%s\n",buffer);
  }
 
  // ��4�����ر�socket���ͷ���Դ��
  close(sockfd);
}

