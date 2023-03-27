/*
 * ��������demo11.cpp���˳���������ʾ����APP����Ŀͻ��ˡ�
 * ���ߣ�����ܡ�
*/
#include "../_public.h"

CTcpClient TcpClient;

bool srv001();    // ��¼ҵ��
bool srv002();    // �ҵ��˻�����ѯ����
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo11 ip port\nExample:./demo11 127.0.0.1 5005\n\n"); return -1;
  }

  // �����˷�����������
  if (TcpClient.ConnectToServer(argv[1],atoi(argv[2]))==false)
  {
    printf("TcpClient.ConnectToServer(%s,%s) failed.\n",argv[1],argv[2]); return -1;
  }

  if (srv001()==false)     // ��¼ҵ��
  {
    printf("srv001() failed.\n"); return -1;
  }

  if (srv002()==false)     // �ҵ��˻�����ѯ����
  {
    printf("srv002() failed.\n"); return -1;
  }

  return 0;
}

// ��¼ҵ�� 
bool srv001()    
{
  char buffer[1024];
 
  SPRINTF(buffer,sizeof(buffer),"<srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>");
  printf("���ͣ�%s\n",buffer);
  if (TcpClient.Write(buffer)==false) return false; // �����˷��������ġ�

  memset(buffer,0,sizeof(buffer));
  if (TcpClient.Read(buffer)==false) return false; // ���շ���˵Ļ�Ӧ���ġ�
  printf("���գ�%s\n",buffer);

  // ��������˷��ص�xml��
  int iretcode=-1;
  GetXMLBuffer(buffer,"retcode",&iretcode);
  if (iretcode!=0) { printf("��¼ʧ�ܡ�\n"); return false; }

  printf("��¼�ɹ���\n"); 

  return true;
}

// �ҵ��˻�����ѯ����
bool srv002()    
{
  char buffer[1024];

  SPRINTF(buffer,sizeof(buffer),"<srvcode>2</srvcode><cardid>62620000000001</cardid>");
  printf("���ͣ�%s\n",buffer);
  if (TcpClient.Write(buffer)==false) return false; // �����˷��������ġ�

  memset(buffer,0,sizeof(buffer));
  if (TcpClient.Read(buffer)==false) return false; // ���շ���˵Ļ�Ӧ���ġ�
  printf("���գ�%s\n",buffer);

  // ��������˷��ص�xml��
  int iretcode=-1;
  GetXMLBuffer(buffer,"retcode",&iretcode);
  if (iretcode!=0) { printf("��ѯ���ʧ�ܡ�\n"); return false; }

  double ye=0;
  GetXMLBuffer(buffer,"ye",&ye);

  printf("��ѯ���ɹ�(%.2f)��\n",ye); 
  
  return true;
}


