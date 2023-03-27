/*
 * ��������demo14.cpp���˳���������ʾ����APP����ķ���ˣ��������������ġ�
 * ���ߣ������
*/
#include "../_public.h"

CLogFile logfile;      // ��������������־��
CTcpServer TcpServer;  // ��������˶���

void FathEXIT(int sig);  // �������˳�������
void ChldEXIT(int sig);  // �ӽ����˳�������

bool bsession=false;     // �ͻ����Ƿ��ѵ�¼��true-�ѵ�¼;false-δ��¼���¼ʧ�ܡ�

// ����ҵ�����������
bool _main(const char *strrecvbuffer,char *strsendbuffer);

// ������
bool srv000(const char *strrecvbuffer,char *strsendbuffer);

// ��¼ҵ��������
bool srv001(const char *strrecvbuffer,char *strsendbuffer);

// ��ѯ���ҵ��������
bool srv002(const char *strrecvbuffer,char *strsendbuffer);

// ת�ˡ�
bool srv003(const char *strrecvbuffer,char *strsendbuffer);
 
int main(int argc,char *argv[])
{
  if (argc!=4)
  {
    printf("Using:./demo14 port logfile timeout\nExample:./demo14 5005 /tmp/demo14.log 35\n\n"); return -1;
  }

  // �ر�ȫ�����źź����������
  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ����
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ
  CloseIOAndSignal(); signal(SIGINT,FathEXIT); signal(SIGTERM,FathEXIT);

  if (logfile.Open(argv[2],"a+")==false) { printf("logfile.Open(%s) failed.\n",argv[2]); return -1; }

  // ����˳�ʼ����
  if (TcpServer.InitServer(atoi(argv[1]))==false)
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); return -1;
  }

  while (true)
  {
    // �ȴ��ͻ��˵���������
    if (TcpServer.Accept()==false)
    {
      logfile.Write("TcpServer.Accept() failed.\n"); FathEXIT(-1);
    }

    logfile.Write("�ͻ��ˣ�%s�������ӡ�\n",TcpServer.GetIP());

    if (fork()>0) { TcpServer.CloseClient(); continue; }  // �����̼����ص�Accept()��
   
    // �ӽ������������˳��źš�
    signal(SIGINT,ChldEXIT); signal(SIGTERM,ChldEXIT);

    TcpServer.CloseListen();

    // �ӽ�����ͻ��˽���ͨѶ������ҵ��
    char strrecvbuffer[1024],strsendbuffer[1024];

    // ��ͻ���ͨѶ�����տͻ��˷������ı��ĺ󣬻ظ�ok��
    while (1)
    {
      memset(strrecvbuffer,0,sizeof(strrecvbuffer));
      memset(strsendbuffer,0,sizeof(strsendbuffer));

      if (TcpServer.Read(strrecvbuffer,atoi(argv[3]))==false) break; // ���տͻ��˵������ġ�
      logfile.Write("���գ�%s\n",strrecvbuffer);

      // ����ҵ�����������
      if (_main(strrecvbuffer,strsendbuffer)==false) break;

      if (TcpServer.Write(strsendbuffer)==false) break; // ��ͻ��˷�����Ӧ�����
      logfile.Write("���ͣ�%s\n",strsendbuffer);
    }

    ChldEXIT(0);
  }
}

// �������˳�������
void FathEXIT(int sig)  
{
  // ���´�����Ϊ�˷�ֹ�źŴ�������ִ�еĹ����б��ź��жϡ�
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("�������˳���sig=%d��\n",sig);

  TcpServer.CloseListen();    // �رռ�����socket��

  kill(0,15);     // ֪ͨȫ�����ӽ����˳���

  exit(0);
}

// �ӽ����˳�������
void ChldEXIT(int sig)  
{
  // ���´�����Ϊ�˷�ֹ�źŴ�������ִ�еĹ����б��ź��жϡ�
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("�ӽ����˳���sig=%d��\n",sig);

  TcpServer.CloseClient();    // �رտͻ��˵�socket��

  exit(0);
}

// ����ҵ�����������
bool _main(const char *strrecvbuffer,char *strsendbuffer)
{
  // ����strrecvbuffer����ȡ������루ҵ����룩��
  int isrvcode=-1;
  GetXMLBuffer(strrecvbuffer,"srvcode",&isrvcode);

  if ( (isrvcode!=1) && (bsession==false) )
  {
    strcpy(strsendbuffer,"<retcode>-1</retcode><message>�û�δ��¼��</message>"); return true;
  }

  // ����ÿ��ҵ��
  switch (isrvcode)
  {
    case 0:   // ������
      srv000(strrecvbuffer,strsendbuffer); break;
    case 1:   // ��¼��
      srv001(strrecvbuffer,strsendbuffer); break;
    case 2:   // ��ѯ��
      srv002(strrecvbuffer,strsendbuffer); break;
    case 3:   // ת�ˡ�
      srv003(strrecvbuffer,strsendbuffer); break;
    default:
      logfile.Write("ҵ����벻�Ϸ���%s\n",strrecvbuffer); return false;
  }

  return true;
}

// ������
bool srv000(const char *strrecvbuffer,char *strsendbuffer)
{
  strcpy(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message>");
  
  return true;
}

// ��¼��
bool srv001(const char *strrecvbuffer,char *strsendbuffer)
{
  // <srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>

  // ����strrecvbuffer����ȡҵ�������
  char tel[21],password[31];
  GetXMLBuffer(strrecvbuffer,"tel",tel,20);
  GetXMLBuffer(strrecvbuffer,"password",password,30);

  // ����ҵ��
  // �Ѵ���������strsendbuffer��
  if ( (strcmp(tel,"1392220000")==0) && (strcmp(password,"123456")==0) )
  {
    strcpy(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message>");  bsession=true;
  }
  else
    strcpy(strsendbuffer,"<retcode>-1</retcode><message>ʧ�ܡ�</message>");

  return true;
}

// ��ѯ���ҵ��������
bool srv002(const char *strrecvbuffer,char *strsendbuffer)
{
  // <srvcode>2</srvcode><cardid>62620000000001</cardid>

  // ����strrecvbuffer����ȡҵ�������
  char cardid[31];
  GetXMLBuffer(strrecvbuffer,"cardid",cardid,30);

  // ����ҵ��
  // �Ѵ���������strsendbuffer��
  if (strcmp(cardid,"62620000000001")==0) 
    strcpy(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message><ye>100.58</ye>");
  else
    strcpy(strsendbuffer,"<retcode>-1</retcode><message>ʧ�ܡ�</message>");

  return true;
}

// ת�ˡ�
bool srv003(const char *strrecvbuffer,char *strsendbuffer)
{
  // ��дת��ҵ��Ĵ��롣

  strcpy(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message><ye>100.58</ye>");

  return true;
}

