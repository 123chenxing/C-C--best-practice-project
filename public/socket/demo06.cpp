/*
 * ��������demo06.cpp���˳���������ʾ��ճ����socket����ˡ�
 * ���ߣ������
*/
#include "../_public.h"
 
int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./demo06 port\nExample:./demo06 5005\n\n"); return -1;
  }

  // ��1������������˵�socket��
  int listenfd;
  if ( (listenfd = socket(AF_INET,SOCK_STREAM,0))==-1) { perror("socket"); return -1; }
  
  // ��2�����ѷ��������ͨѶ�ĵ�ַ�Ͷ˿ڰ󶨵�socket�ϡ�
  struct sockaddr_in servaddr;    // ����˵�ַ��Ϣ�����ݽṹ��
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;  // Э���壬��socket�����ֻ����AF_INET��
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);          // ����ip��ַ��
  servaddr.sin_port = htons(atoi(argv[1]));  // ָ��ͨѶ�˿ڡ�
  if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0 )
  { perror("bind"); close(listenfd); return -1; }
 
  // ��3������socket����Ϊ����ģʽ��
  if (listen(listenfd,5) != 0 ) { perror("listen"); close(listenfd); return -1; }
 
  // ��4�������ܿͻ��˵����ӡ�
  int  clientfd;                  // �ͻ��˵�socket��
  int  socklen=sizeof(struct sockaddr_in); // struct sockaddr_in�Ĵ�С
  struct sockaddr_in clientaddr;  // �ͻ��˵ĵ�ַ��Ϣ��
  clientfd=accept(listenfd,(struct sockaddr *)&clientaddr,(socklen_t*)&socklen);
  printf("�ͻ��ˣ�%s�������ӡ�\n",inet_ntoa(clientaddr.sin_addr));
 
  char buffer[1024];

  // ��5������ͻ���ͨѶ�����տͻ��˷������ı��ġ�
  while (1)
  {
    memset(buffer,0,sizeof(buffer));

    int ibuflen=0;
    if (TcpRead(clientfd,buffer,&ibuflen)==false) break; // ���տͻ��˵������ġ�

    printf("���գ�%s\n",buffer);
  }
 
  // ��6�����ر�socket���ͷ���Դ��
  close(listenfd); close(clientfd); 
}
