/*
 * ��������tcpgetfiles.cpp������tcpЭ�飬ʵ���ļ����صĿͻ��ˡ�
 * ���ߣ��¹ڡ�
*/
#include "_public.h"

// �������еĲ����ṹ�塣
struct st_arg
{
  int  clienttype;          // �ͻ������ͣ�1-�ϴ��ļ���2-�����ļ���
  char ip[31];              // ����˵�IP��ַ��
  int  port;                // ����˵Ķ˿ڡ�
  int  ptype;               // �ļ����سɹ��������ļ��Ĵ���ʽ��1-ɾ���ļ���2-�ƶ�������Ŀ¼��
  char srvpath[301];        // ������ļ���ŵĸ�Ŀ¼��
  char srvpathbak[301];     // �ļ��ɹ����غ󣬷�����ļ����ݵĸ�Ŀ¼����ptype==2ʱ��Ч��
  bool andchild;            // �Ƿ�����srvpathĿ¼�¸�����Ŀ¼���ļ���true-�ǣ�false-��
  char matchname[301];      // �������ļ�����ƥ�������"*.TXT,*.XML"��
  char clientpath[301];     // �ͻ����ļ���ŵĸ�Ŀ¼��
  int  timetvl;             // ɨ������Ŀ¼�ļ���ʱ��������λ���롣
  int  timeout;             // ���������ĳ�ʱʱ�䡣
  char pname[51];           // ��������������"tcpgetfiles_��׺"�ķ�ʽ��
} starg;

CLogFile logfile;

// �����˳����ź�2��15�Ĵ�������
void EXIT(int sig);

void _help();

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

CTcpClient TcpClient;

bool Login(const char *argv);    // ��¼ҵ��

char strrecvbuffer[1024];   // ���ͱ��ĵ�buffer��
char strsendbuffer[1024];   // ���ձ��ĵ�buffer��

// �ļ����ص���������
void _tcpgetfiles();

// �����ļ������ݡ�
bool RecvFile(const int sockfd,const char *filename,const char *mtime,int filesize);

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

  PActive.AddPInfo(starg.timeout,starg.pname);  // �ѽ��̵�������Ϣд�빲���ڴ档

  // �����˷�����������
  if (TcpClient.ConnectToServer(starg.ip,starg.port)==false)
  {
    logfile.Write("TcpClient.ConnectToServer(%s,%d) failed.\n",starg.ip,starg.port); EXIT(-1);
  }

  // ��¼ҵ��
  if (Login(argv[2])==false) { logfile.Write("Login() failed.\n"); EXIT(-1); }

  // �����ļ����ص���������
  _tcpgetfiles();

  EXIT(0);
}


// ��¼ҵ�� 
bool Login(const char *argv)    
{
  memset(strsendbuffer,0,sizeof(strsendbuffer));
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
 
  SPRINTF(strsendbuffer,sizeof(strsendbuffer),"%s<clienttype>2</clienttype>",argv);
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
  printf("Using:/project/tools11/bin/tcpgetfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools11/bin/procctl 20 /project/tools11/bin/tcpgetfiles /log/idc/tcpgetfiles_surfdata.log \"<ip>127.0.0.1</ip><port>5007</port><ptype>1</ptype><srvpath>/tmp/tcp/surfdata2</srvpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcp/surfdata3</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>\"\n");
  printf("       /project/tools11/bin/procctl 20 /project/tools11/bin/tcpgetfiles /log/idc/tcpgetfiles_surfdata.log \"<ip>127.0.0.1</ip><port>5007</port><ptype>2</ptype><srvpath>/tmp/tcp/surfdata2</srvpath><srvpathbak>/tmp/tcp/surfdata2bak</srvpathbak><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcp/surfdata3</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>\"\n\n\n");

  printf("���������������ĵĹ�������ģ�飬����tcpЭ��ӷ���������ļ���\n");
  printf("logfilename   ���������е���־�ļ���\n");
  printf("xmlbuffer     ���������еĲ��������£�\n");
  printf("ip            ����˵�IP��ַ��\n");
  printf("port          ����˵Ķ˿ڡ�\n");
  printf("ptype         �ļ����سɹ��������ļ��Ĵ���ʽ��1-ɾ���ļ���2-�ƶ�������Ŀ¼��\n");
  printf("srvpath       ������ļ���ŵĸ�Ŀ¼��\n");
  printf("srvpathbak    �ļ��ɹ����غ󣬷�����ļ����ݵĸ�Ŀ¼����ptype==2ʱ��Ч��\n");
  printf("andchild      �Ƿ�����srvpathĿ¼�¸�����Ŀ¼���ļ���true-�ǣ�false-��ȱʡΪfalse��\n");
  printf("matchname     �������ļ�����ƥ�������\"*.TXT,*.XML\"\n");
  printf("clientpath    �ͻ����ļ���ŵĸ�Ŀ¼��\n");
  printf("timetvl       ɨ�����Ŀ¼�ļ���ʱ��������λ���룬ȡֵ��1-30֮�䡣\n");
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

  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath);
  if (strlen(starg.srvpath)==0) { logfile.Write("srvpath is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"srvpathbak",starg.srvpathbak);
  if ((starg.ptype==2)&&(strlen(starg.srvpathbak)==0)) { logfile.Write("srvpathbak is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname);
  if (strlen(starg.matchname)==0) { logfile.Write("matchname is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath);
  if (strlen(starg.clientpath)==0) { logfile.Write("clientpath is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if (starg.timetvl==0) { logfile.Write("timetvl is null.\n"); return false; }

  // ɨ������Ŀ¼�ļ���ʱ��������λ���롣
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

// �ļ����ص���������
void _tcpgetfiles()
{
  PActive.AddPInfo(starg.timeout,starg.pname);

  while (true)
  {
    memset(strsendbuffer,0,sizeof(strsendbuffer));
    memset(strrecvbuffer,0,sizeof(strrecvbuffer));

    PActive.UptATime();

    // ���շ���˵ı��ġ�
    // �ڶ���������ȡֵ�������starg.timetvl��С��starg.timeout��
    if (TcpClient.Read(strrecvbuffer,starg.timetvl+10)==false)
    {
      logfile.Write("TcpClient.Read() failed.\n"); return;
    }
    // logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);

    // �����������ġ�
    if (strcmp(strrecvbuffer,"<activetest>ok</activetest>")==0)
    {
      strcpy(strsendbuffer,"ok");
      // logfile.Write("strsendbuffer=%s\n",strsendbuffer);
      if (TcpClient.Write(strsendbuffer)==false)
      {
        logfile.Write("TcpClient.Write() failed.\n"); return;
      }
    }

    // ���������ļ��������ġ�
    if (strncmp(strrecvbuffer,"<filename>",10)==0)
    {
      // ���������ļ������ĵ�xml��
      char serverfilename[301];  memset(serverfilename,0,sizeof(serverfilename));
      char mtime[21];            memset(mtime,0,sizeof(mtime));
      int  filesize=0;
      GetXMLBuffer(strrecvbuffer,"filename",serverfilename,300);

      GetXMLBuffer(strrecvbuffer,"mtime",mtime,19);
      GetXMLBuffer(strrecvbuffer,"size",&filesize);

      // �ͻ��˺ͷ�����ļ���Ŀ¼�ǲ�һ���ģ����´������ɿͻ��˵��ļ�����
      // ���ļ����е�srvpath�滻��clientpath��ҪС�ĵ���������
      char clientfilename[301];  memset(clientfilename,0,sizeof(clientfilename));
      strcpy(clientfilename,serverfilename);
      UpdateStr(clientfilename,starg.srvpath,starg.clientpath,false);

      // �����ļ������ݡ�
      logfile.Write("recv %s(%d) ...",clientfilename,filesize);
      if (RecvFile(TcpClient.m_connfd,clientfilename,mtime,filesize)==true)
      {
        logfile.WriteEx("ok.\n");
        SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><result>ok</result>",serverfilename);
      }
      else
      {
        logfile.WriteEx("failed.\n");
        SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><result>failed</result>",serverfilename);
      }

      // �ѽ��ս�����ظ��Զˡ�
      // logfile.Write("strsendbuffer=%s\n",strsendbuffer);
      if (TcpClient.Write(strsendbuffer)==false)
      {
        logfile.Write("TcpClient.Write() failed.\n"); return;
      }
    }
  }
}

// �����ļ������ݡ�
bool RecvFile(const int sockfd,const char *filename,const char *mtime,int filesize)
{
  // ������ʱ�ļ�����
  char strfilenametmp[301];
  SNPRINTF(strfilenametmp,sizeof(strfilenametmp),300,"%s.tmp",filename);

  int  totalbytes=0;        // �ѽ����ļ������ֽ�����
  int  onread=0;            // ���δ�����յ��ֽ�����
  char buffer[1000];        // �����ļ����ݵĻ�������
  FILE *fp=NULL;

  // ������ʱ�ļ���
  if ( (fp=FOPEN(strfilenametmp,"wb"))==NULL ) return false;

  while (true)
  {
    memset(buffer,0,sizeof(buffer));

    // ���㱾��Ӧ�ý��յ��ֽ�����
    if (filesize-totalbytes>1000) onread=1000;
    else onread=filesize-totalbytes;

    // �����ļ����ݡ�
    if (Readn(sockfd,buffer,onread)==false) { fclose(fp); return false; }

    // �ѽ��յ�������д���ļ���
    fwrite(buffer,1,onread,fp);

    // �����ѽ����ļ������ֽ���������ļ������꣬����ѭ����
    totalbytes=totalbytes+onread;

    if (totalbytes==filesize) break;
  }

  // �ر���ʱ�ļ���
  fclose(fp);

  // �����ļ���ʱ�䡣
  UTime(strfilenametmp,mtime);

  // ����ʱ�ļ�RENAMEΪ��ʽ���ļ���
  if (RENAME(strfilenametmp,filename)==false) return false;

  return true;
}







