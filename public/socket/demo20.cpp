/*
 * ��������demo20.cpp���˳�����ʾ���ÿ�����ܵ�CTcpServer��ʵ��socketͨѶ���̵߳ķ���ˡ�
 * ���ߣ������
*/
#include "../_public.h"

CLogFile   logfile;    // ��������������־��
CTcpServer TcpServer;  // ��������˶���

void EXIT(int sig);    // ���̵��˳�������

pthread_spinlock_t vthidlock;  // ��������vthid����������
vector<pthread_t> vthid;       // ���ȫ���߳�id��������
void *thmain(void *arg);       // �߳���������

void thcleanup(void *arg);     // �߳���������
 
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./demo20 port logfile\nExample:./demo20 5005 /tmp/demo20.log\n\n"); return -1;
  }

  // �ر�ȫ�����źź����������
  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ����
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[2],"a+")==false) { printf("logfile.Open(%s) failed.\n",argv[2]); return -1; }

  // ����˳�ʼ����
  if (TcpServer.InitServer(atoi(argv[1]))==false)
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); return -1;
  }

  pthread_spin_init(&vthidlock,0);

  while (true)
  {
    // �ȴ��ͻ��˵���������
    if (TcpServer.Accept()==false)
    {
      logfile.Write("TcpServer.Accept() failed.\n"); EXIT(-1);
    }

    logfile.Write("�ͻ��ˣ�%s�������ӡ�\n",TcpServer.GetIP());

    // ����һ���µ��̣߳�������ͻ���ͨѶ��
    pthread_t thid;
    if (pthread_create(&thid,NULL,thmain,(void *)(long)TcpServer.m_connfd)!=0)
    {
      logfile.Write("pthread_create() failed.\n"); TcpServer.CloseListen(); continue;
    }

    pthread_spin_lock(&vthidlock);
    vthid.push_back(thid);    // ���߳�id����������
    pthread_spin_unlock(&vthidlock);
  }
}

void *thmain(void *arg)     // �߳���������
{
  pthread_cleanup_push(thcleanup,arg);       // ���߳���������ջ���رտͻ��˵�socket����

  int connfd=(int)(long)arg;    // �ͻ��˵�socket��

  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);   // �߳�ȡ����ʽΪ����ȡ����

  pthread_detach(pthread_self());           // ���̷߳����ȥ��

  // �ӽ�����ͻ��˽���ͨѶ������ҵ��
  int  ibuflen;
  char buffer[102400];

  // ��ͻ���ͨѶ�����տͻ��˷������ı��ĺ󣬻ظ�ok��
  while (1)
  {
    memset(buffer,0,sizeof(buffer));
    if (TcpRead(connfd,buffer,&ibuflen,30)==false) break; // ���տͻ��˵������ġ�
    logfile.Write("���գ�%s\n",buffer);

    strcpy(buffer,"ok");
    if (TcpWrite(connfd,buffer)==false) break; // ��ͻ��˷�����Ӧ�����
    logfile.Write("���ͣ�%s\n",buffer);
  }

  close(connfd);       // �رտͻ��˵����ӡ�

  // �ѱ��߳�id�Ӵ���߳�id��������ɾ����
  pthread_spin_lock(&vthidlock);
  for (int ii=0;ii<vthid.size();ii++)
  {
    if (pthread_equal(pthread_self(),vthid[ii])) { vthid.erase(vthid.begin()+ii); break; }
  }
  pthread_spin_unlock(&vthidlock);

  pthread_cleanup_pop(1);         // ���߳���������ջ��
}

// ���̵��˳�������
void EXIT(int sig)  
{
  // ���´�����Ϊ�˷�ֹ�źŴ�������ִ�еĹ����б��ź��жϡ�
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("�����˳���sig=%d��\n",sig);

  TcpServer.CloseListen();    // �رռ�����socket��

  // ȡ��ȫ�����̡߳�
  for (int ii=0;ii<vthid.size();ii++)
  {
    pthread_cancel(vthid[ii]);
  }

  sleep(1);        // �����߳����㹻��ʱ���˳���

  pthread_spin_destroy(&vthidlock);

  exit(0);
}

void thcleanup(void *arg)     // �߳���������
{
  close((int)(long)arg);      // �رտͻ��˵�socket��

  logfile.Write("�߳�%lu�˳���\n",pthread_self());
}

