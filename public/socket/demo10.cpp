/*
 * ��������demo10.cpp���˳�����ʾ���ÿ�����ܵ�CTcpServer��ʵ��socketͨѶ����̵ķ���ˡ�
 * 1���ڶ���̵ķ�������У����ɱ��һ���ӽ��̣�������ӽ���ͨѶ�Ŀͻ��˻�Ͽ������ǣ���
 *    ��Ӱ���������ӽ��̺Ϳͻ��ˣ�Ҳ����Ӱ�츸���̡�
 * 2�����ɱ�������̣�����Ӱ������ͨѶ�е��ӽ��̣����ǣ��µĿͻ����޷��������ӡ�
 * 3�������killall+������������ɱ�������̺�ȫ�����ӽ��̡�
 *
 * ������������˳����˳������������
 * 1��������ӽ����յ��˳��źţ����ӽ��̶Ͽ���ͻ������ӵ�socket��Ȼ���˳���
 * 2������Ǹ������յ��˳��źţ��������ȹرռ�����socket��Ȼ����ȫ�����ӽ��̷����˳��źš�
 * 3��������ӽ��̶��յ��˳��źţ����������2�������ͬ��
 *
 * ���ߣ������
*/
#include "../_public.h"

CLogFile logfile;      // ��������������־��
CTcpServer TcpServer;  // ��������˶���

void FathEXIT(int sig);  // �������˳�������
void ChldEXIT(int sig);  // �ӽ����˳�������
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo10 port logfile\nExample:./demo10 5005 /tmp/demo10.log\n\n"); return -1;
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
    char buffer[102400];

    // ��ͻ���ͨѶ�����տͻ��˷������ı��ĺ󣬻ظ�ok��
    while (1)
    {
      memset(buffer,0,sizeof(buffer));
      if (TcpServer.Read(buffer)==false) break; // ���տͻ��˵������ġ�
      logfile.Write("���գ�%s\n",buffer);

      strcpy(buffer,"ok");
      if (TcpServer.Write(buffer)==false) break; // ��ͻ��˷�����Ӧ�����
      logfile.Write("���ͣ�%s\n",buffer);
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
