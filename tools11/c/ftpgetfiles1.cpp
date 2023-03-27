#include "_public.h"
#include "_ftp.h"

struct st_arg
{
  char host[31];           // Զ�̷�������IP�Ͷ˿ڡ�
  int  mode;               // ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��
  char username[31];       // Զ�̷�����ftp���û�����
  char password[31];       // Զ�̷�����ftp�����롣
  char remotepath[301];    // Զ�̷���������ļ���Ŀ¼��
  char localpath[301];     // �����ļ���ŵ�Ŀ¼��
  char matchname[101];     // �������ļ�ƥ��Ĺ���
} starg;

CLogFile logfile;

Cftp ftp;

// �����˳����ź�2��15�Ĵ�������
void EXIT(int sig);

void _help();

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

int main(int argc,char *argv[])
{
  // СĿ�꣬��ftp������ĳĿ¼�е��ļ����ص����ص�Ŀ¼�С�

  if (argc!=3) { _help(); return -1; }

  // �ر�ȫ�����źź����������
  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ���̡�
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ��
  // CloseIOAndSignal(); 
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // ����־�ļ���
  if (logfile.Open(argv[1],"a+")==false)
  {
    printf("����־�ļ�ʧ�ܣ�%s����\n",argv[1]); return -1;
  }

  // ����xml���õ��������еĲ�����
  if (_xmltoarg(argv[2])==false) return -1;

  // ��¼ftp��������

  // ����ftp����������ļ���Ŀ¼��

  // ����ftp.nlist()�����г�������Ŀ¼�е��ļ��������ŵ������ļ��С�

  // ��ftp.nlist()������ȡ����list�ļ����ص�����vfilelist�С�

  // ��������vfilelist��
/*
  for (int ii=0;ii<vlistfile.size();ii++)
  {
    // ����ftp.get()�����ӷ����������ļ���
  }
*/

  // ftp.logout();

  return 0;
}

void EXIT(int sig)
{
  printf("�����˳���sig=%d\n\n",sig);

  exit(0);
}

void _help()
{
  printf("\n");
  printf("Using:/project/tools1/bin/ftpgetfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log \"<host>127.0.0.1:21</host><mode>1</mode><username>wucz</username><password>wuczpwd</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname>\"\n\n\n");

  printf("��������ͨ�õĹ���ģ�飬���ڰ�Զ��ftp���������ļ����ص�����Ŀ¼��\n");
  printf("logfilename�Ǳ��������е���־�ļ���\n");
  printf("xmlbufferΪ�ļ����صĲ��������£�\n");
  printf("<host>127.0.0.1:21</host> Զ�̷�������IP�Ͷ˿ڡ�\n");
  printf("<mode>1</mode> ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��\n");
  printf("<username>wucz</username> Զ�̷�����ftp���û�����\n");
  printf("<password>wuczpwd</password> Զ�̷�����ftp�����롣\n");
  printf("<remotepath>/tmp/idc/surfdata</remotepath> Զ�̷���������ļ���Ŀ¼��\n");
  printf("<localpath>/idcdata/surfdata</localpath> �����ļ���ŵ�Ŀ¼��\n");
  printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> �������ļ�ƥ��Ĺ���"\
         "��ƥ����ļ����ᱻ���أ����ֶξ��������þ�ȷ����������*ƥ��ȫ�����ļ���\n\n\n");
}

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"host",starg.host,30);   // Զ�̷�������IP�Ͷ˿ڡ�
  if (strlen(starg.host)==0)
  { logfile.Write("host is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);   // ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��
  if (starg.mode!=2)  starg.mode=1;

  GetXMLBuffer(strxmlbuffer,"username",starg.username,30);   // Զ�̷�����ftp���û�����
  if (strlen(starg.username)==0)
  { logfile.Write("username is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"password",starg.password,30);   // Զ�̷�����ftp�����롣
  if (strlen(starg.password)==0)
  { logfile.Write("password is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);   // Զ�̷���������ļ���Ŀ¼��
  if (strlen(starg.remotepath)==0)
  { logfile.Write("remotepath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);   // �����ļ���ŵ�Ŀ¼��
  if (strlen(starg.localpath)==0)
  { logfile.Write("localpath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // �������ļ�ƥ��Ĺ���
  if (strlen(starg.matchname)==0)
  { logfile.Write("matchname is null.\n");  return false; }

  return true;
}
