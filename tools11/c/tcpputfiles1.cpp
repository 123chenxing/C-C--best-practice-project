/*
 * ��������tcpputfiles.cpp������tcpЭ�飬ʵ���ļ��ϴ��Ŀͻ��ˡ�
 * ���ߣ�����ܡ�
*/
#include "_public.h"

// �������еĲ����ṹ�塣
struct st_arg
{
  int  clienttype;          // �ͻ������ͣ�1-�ϴ��ļ���2-�����ļ���
  char ip[31];              // ����˵�IP��ַ��
  int  port;                // ����˵Ķ˿ڡ�
  int  ptype;               // �ļ��ϴ��ɹ����ļ��Ĵ���ʽ��1-ɾ���ļ���2-�ƶ�������Ŀ¼��
  char clientpath[301];     // �����ļ���ŵĸ�Ŀ¼��
  char clientpathbak[301];  // �ļ��ɹ��ϴ��󣬱����ļ����ݵĸ�Ŀ¼����ptype==2ʱ��Ч��
  bool andchild;            // �Ƿ��ϴ�clientpathĿ¼�¸�����Ŀ¼���ļ���true-�ǣ�false-��
  char matchname[301];      // ���ϴ��ļ�����ƥ�������"*.TXT,*.XML"��
  char srvpath[301];        // ������ļ���ŵĸ�Ŀ¼��
  int  timetvl;             // ɨ�豾��Ŀ¼�ļ���ʱ��������λ���롣
  int  timeout;             // ���������ĳ�ʱʱ�䡣
  char pname[51];           // ��������������"tcpputfiles_��׺"�ķ�ʽ��
} starg;

CLogFile logfile;

// �����˳����ź�2��15�Ĵ�������
void EXIT(int sig);

void _help();

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

CTcpClient TcpClient;

bool Login(const char *argv);    // ��¼ҵ��

bool ActiveTest();    // ������

char strrecvbuffer[1024];   // ���ͱ��ĵ�buffer��
char strsendbuffer[1024];   // ���ձ��ĵ�buffer��

CPActive PActive;  // ����������

int main(int argc,char *argv[])
{
  if (argc!=3) { _help(); return -1; }

  // �ر�ȫ�����źź����������
  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ���̡�
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ��
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // ����־�ļ���
  if (logfile.Open(argv[1],"a+")==false)
  {
    printf("����־�ļ�ʧ�ܣ�%s����\n",argv[1]); return -1;
  }

  // ����xml���õ��������еĲ�����
  if (_xmltoarg(argv[2])==false) return -1;

  // PActive.AddPInfo(starg.timeout,starg.pname);  // �ѽ��̵�������Ϣд�빲���ڴ档

  // �����˷�����������
  if (TcpClient.ConnectToServer(starg.ip,starg.port)==false)
  {
    logfile.Write("TcpClient.ConnectToServer(%s,%d) failed.\n",starg.ip,starg.port); EXIT(-1);
  }

  // ��¼ҵ��
  if (Login(argv[2])==false) { logfile.Write("Login() failed.\n"); EXIT(-1); }

  for (int ii=3;ii<5;ii++)
  {
    if (ActiveTest()==false) break;

    sleep(ii);
  }

  EXIT(0);
}

// ������ 
bool ActiveTest()    
{
  memset(strsendbuffer,0,sizeof(strsendbuffer));
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
 
  SPRINTF(strsendbuffer,sizeof(strsendbuffer),"<activetest>ok</activetest>");
  logfile.Write("���ͣ�%s\n",strsendbuffer);
  if (TcpClient.Write(strsendbuffer)==false) return false; // �����˷��������ġ�

  if (TcpClient.Read(strrecvbuffer,20)==false) return false; // ���շ���˵Ļ�Ӧ���ġ�
  logfile.Write("���գ�%s\n",strrecvbuffer);

  return true;
}

// ��¼ҵ�� 
bool Login(const char *argv)    
{
  memset(strsendbuffer,0,sizeof(strsendbuffer));
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
 
  SPRINTF(strsendbuffer,sizeof(strsendbuffer),"%s<clienttype>1</clienttype>",argv);
  logfile.Write("���ͣ�%s\n",strsendbuffer);
  if (TcpClient.Write(strsendbuffer)==false) return false; // �����˷��������ġ�

  if (TcpClient.Read(strrecvbuffer,20)==false) return false; // ���շ���˵Ļ�Ӧ���ġ�
  logfile.Write("���գ�%s\n",strrecvbuffer);

  logfile.Write("��¼(%s:%d)�ɹ���\n",starg.ip,starg.port); 

  return true;
}

void EXIT(int sig)
{
  logfile.Write("�����˳���sig=%d\n\n",sig);

  exit(0);
}

void _help()
{
  printf("\n");
  printf("Using:/project/tools1/bin/tcpputfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools1/bin/procctl 20 /project/tools1/bin/tcpputfiles /log/idc/tcpputfiles_surfdata.log \"<ip>192.168.174.132</ip><port>5005</port><ptype>1</ptype><clientpath>/tmp/tcp/surfdata1</clientpath><clientpathbak>/tmp/tcp/surfdata1bak</clientpathbak><andchild>true</andchild><matchname>*.XML,*.CSV</matchname><srvpath>/tmp/tcp/surfdata2</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>\"\n");
  printf("       /project/tools1/bin/procctl 20 /project/tools1/bin/tcpputfiles /log/idc/tcpputfiles_surfdata.log \"<ip>192.168.174.132</ip><port>5005</port><ptype>2</ptype><clientpath>/tmp/tcp/surfdata1</clientpath><clientpathbak>/tmp/tcp/surfdata1bak</clientpathbak><andchild>true</andchild><matchname>*.XML,*.CSV</matchname><srvpath>/tmp/tcp/surfdata2</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>\"\n\n\n");

  printf("���������������ĵĹ�������ģ�飬����tcpЭ����ļ����͸�����ˡ�\n");
  printf("logfilename   ���������е���־�ļ���\n");
  printf("xmlbuffer     ���������еĲ��������£�\n");
  printf("ip            ����˵�IP��ַ��\n");
  printf("port          ����˵Ķ˿ڡ�\n");
  printf("ptype         �ļ��ϴ��ɹ���Ĵ���ʽ��1-ɾ���ļ���2-�ƶ�������Ŀ¼��\n");
  printf("clientpath    �����ļ���ŵĸ�Ŀ¼��\n");
  printf("clientpathbak �ļ��ɹ��ϴ��󣬱����ļ����ݵĸ�Ŀ¼����ptype==2ʱ��Ч��\n");
  printf("andchild      �Ƿ��ϴ�clientpathĿ¼�¸�����Ŀ¼���ļ���true-�ǣ�false-��ȱʡΪfalse��\n");
  printf("matchname     ���ϴ��ļ�����ƥ�������\"*.TXT,*.XML\"\n");
  printf("srvpath       ������ļ���ŵĸ�Ŀ¼��\n");
  printf("timetvl       ɨ�豾��Ŀ¼�ļ���ʱ��������λ���룬ȡֵ��1-30֮�䡣\n");
  printf("timeout       ������ĳ�ʱʱ�䣬��λ���룬���ļ���С����������������������50���ϡ�\n");
  printf("pname         �������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣\n\n");
}

// ��xml����������starg�ṹ
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"ip",starg.ip);
  if (strlen(starg.ip)==0) { logfile.Write("ip is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"port",&starg.port);
  if ( starg.port==0) { logfile.Write("port is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  if ((starg.ptype!=1)&&(starg.ptype!=2)) { logfile.Write("ptype not in (1,2).\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath);
  if (strlen(starg.clientpath)==0) { logfile.Write("clientpath is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"clientpathbak",starg.clientpathbak);
  if ((starg.ptype==2)&&(strlen(starg.clientpathbak)==0)) { logfile.Write("clientpathbak is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname);
  if (strlen(starg.matchname)==0) { logfile.Write("matchname is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath);
  if (strlen(starg.srvpath)==0) { logfile.Write("srvpath is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if (starg.timetvl==0) { logfile.Write("timetvl is null.\n"); return false; }

  // ɨ�豾��Ŀ¼�ļ���ʱ��������λ���롣
  // starg.timetvlû�б�Ҫ����30�롣
  if (starg.timetvl>30) starg.timetvl=30;

  // ���������ĳ�ʱʱ�䣬һ��Ҫ����starg.timetvl��û�б�ҪС��50�롣
  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if (starg.timeout==0) { logfile.Write("timeout is null.\n"); return false; }
  if (starg.timeout<50) starg.timeout=50;

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);
  if (strlen(starg.pname)==0) { logfile.Write("pname is null.\n"); return false; }

  return true;
}

