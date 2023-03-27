/*
 * ��������demo01.cpp���˳���������ʾsocketͨѶ�Ŀͻ��ˡ�
 * ���ߣ�����ܡ�
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo01 ip port\nExample:./demo01 127.0.0.1 5005\n\n"); return -1;
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

  int iret;
  char buffer[102400];
 
  // ��3����������ͨѶ������һ�����ĺ�ȴ��ظ���Ȼ���ٷ���һ�����ġ�
  for (int ii=0;ii<10;ii++)
  {
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"���ǵ�%d������Ů�������%03d��",ii+1,ii+1);
    if ( (iret=send(sockfd,buffer,strlen(buffer),0))<=0) // �����˷��������ġ�
    { perror("send"); break; }
    printf("���ͣ�%s\n",buffer);

    memset(buffer,0,sizeof(buffer));
    if ( (iret=recv(sockfd,buffer,sizeof(buffer),0))<=0) // ���շ���˵Ļ�Ӧ���ġ�
    {
      printf("iret=%d\n",iret); break;
    }
    printf("���գ�%s\n",buffer);

    sleep(1);  // ÿ��һ����ٴη��ͱ��ġ�
  }
 
  // ��4�����ر�socket���ͷ���Դ��
  close(sockfd);
}

