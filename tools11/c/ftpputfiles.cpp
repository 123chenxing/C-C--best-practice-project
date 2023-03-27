#include "_public.h"
#include "_ftp.h"

// �������в����Ľṹ�塣
struct st_arg
{
  char host[31];           // Զ�̷���˵�IP�Ͷ˿ڡ�
  int  mode;               // ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��
  char username[31];       // Զ�̷����ftp���û�����
  char password[31];       // Զ�̷����ftp�����롣
  char remotepath[301];    // Զ�̷���˴���ļ���Ŀ¼��
  char localpath[301];     // �����ļ���ŵ�Ŀ¼��
  char matchname[101];     // ���ϴ��ļ�ƥ��Ĺ���
  int  ptype;              // �ϴ���ͻ����ļ��Ĵ���ʽ��1-ʲôҲ������2-ɾ����3-���ݡ�
  char localpathbak[301];  // �ϴ���ͻ����ļ��ı���Ŀ¼��
  char okfilename[301];    // ���ϴ��ɹ��ļ����嵥��
  int  timeout;            // ���������ĳ�ʱʱ�䡣
  char pname[51];          // ��������������"ftpputfiles_��׺"�ķ�ʽ��
} starg;

// �ļ���Ϣ�Ľṹ�塣
struct st_fileinfo
{
  char filename[301];   // �ļ�����
  char mtime[21];       // �ļ�ʱ�䡣
};

vector<struct st_fileinfo> vlistfile1;    // ���ϴ��ɹ��ļ�������������okfilename�м��ء�
vector<struct st_fileinfo> vlistfile2;    // �ϴ�ǰ�г��ͻ����ļ�������������nlist�ļ��м��ء�
vector<struct st_fileinfo> vlistfile3;    // ���β���Ҫ�ϴ����ļ���������
vector<struct st_fileinfo> vlistfile4;    // ������Ҫ�ϴ����ļ���������

// ����okfilename�ļ��е����ݵ�����vlistfile1�С�
bool LoadOKFile();

// �Ƚ�vlistfile2��vlistfile1���õ�vlistfile3��vlistfile4��
bool CompVector();

// ������vlistfile3�е�����д��okfilename�ļ�������֮ǰ�ľ�okfilename�ļ���
bool WriteToOKFile();

// ���ptype==1�����ϴ��ɹ����ļ���¼׷�ӵ�okfilename�ļ��С�
bool AppendToOKFile(struct st_fileinfo *stfileinfo);

// ��localpathĿ¼�µ��ļ����ص�vlistfile2�����С�
bool LoadLocalFile();

CLogFile logfile;

Cftp ftp;

// �����˳����ź�2��15�Ĵ�������
void EXIT(int sig);

void _help();

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

// �ϴ��ļ����ܵ���������
bool _ftpputfiles();

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

  // ��¼ftp����ˡ�
  if (ftp.login(starg.host,starg.username,starg.password,starg.mode)==false)
  {
    logfile.Write("ftp.login(%s,%s,%s) failed.\n",starg.host,starg.username,starg.password); return -1;
  }

  // logfile.Write("ftp.login ok.\n");  // ��ʽ���к󣬿���ע�����д��롣

  _ftpputfiles();

  ftp.logout();

  return 0;
}

// �ϴ��ļ����ܵ���������
bool _ftpputfiles()
{
  // ��localpathĿ¼�µ��ļ����ص�vlistfile2�����С�
  if (LoadLocalFile()==false)
  {
    logfile.Write("LoadLocalFile() failed.\n");  return false;
  }

  PActive.UptATime();   // ���½��̵�������

  if (starg.ptype==1)
  {
    // ����okfilename�ļ��е����ݵ�����vlistfile1�С�
    LoadOKFile();

    // �Ƚ�vlistfile2��vlistfile1���õ�vlistfile3��vlistfile4��
    CompVector();

    // ������vlistfile3�е�����д��okfilename�ļ�������֮ǰ�ľ�okfilename�ļ���
    WriteToOKFile();

    // ��vlistfile4�е����ݸ��Ƶ�vlistfile2�С�
    vlistfile2.clear(); vlistfile2.swap(vlistfile4);
  }

  PActive.UptATime();   // ���½��̵�������

  char strremotefilename[301],strlocalfilename[301];

  // ��������vlistfile2��
  for (int ii=0;ii<vlistfile2.size();ii++)
  {
    SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s/%s",starg.remotepath,vlistfile2[ii].filename);
    SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s/%s",starg.localpath,vlistfile2[ii].filename);

    logfile.Write("put %s ...",strlocalfilename);

    // ����ftp.put()�������ļ��ϴ�������ˣ�������������true��Ŀ����ȷ���ļ��ϴ��ɹ����Է����ɵ�����
    if (ftp.put(strlocalfilename,strremotefilename,true)==false) 
    {
      logfile.WriteEx("failed.\n"); return false;
    }

    logfile.WriteEx("ok.\n");

    PActive.UptATime();   // ���½��̵�������
    
    // ���ptype==1�����ϴ��ɹ����ļ���¼׷�ӵ�okfilename�ļ��С�
    if (starg.ptype==1) AppendToOKFile(&vlistfile2[ii]);

    // ɾ���ļ���
    if (starg.ptype==2)
    {
      if (REMOVE(strlocalfilename)==false)
      {
        logfile.Write("REMOVE(%s) failed.\n",strlocalfilename); return false;
      }
    }

    // ת�浽����Ŀ¼��
    if (starg.ptype==3)
    {
      char strlocalfilenamebak[301];
      SNPRINTF(strlocalfilenamebak,sizeof(strlocalfilenamebak),300,"%s/%s",starg.localpathbak,vlistfile2[ii].filename);
      if (RENAME(strlocalfilename,strlocalfilenamebak)==false)
      {
        logfile.Write("RENAME(%s,%s) failed.\n",strlocalfilename,strlocalfilenamebak); return false;
      }
    }
  }

  return true;
}

void EXIT(int sig)
{
  printf("�����˳���sig=%d\n\n",sig);

  exit(0);
}

void _help()
{
  printf("\n");
  printf("Using:/project/tools11/bin/ftpputfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools11/bin/procctl 30 /project/tools11/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log \"<host>127.0.0.1:21</host><mode>1</mode><username>chenguan</username><password>13251009668</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputest</remotepath><matchname>SURF_ZH*.JSON</matchname><ptype>1</ptype><localpathbak>/tmp/idc/surfdatabak</localpathbak><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>\"\n\n\n");

  printf("��������ͨ�õĹ���ģ�飬���ڰѱ���Ŀ¼�е��ļ��ϴ���Զ�̵�ftp��������\n");
  printf("logfilename�Ǳ��������е���־�ļ���\n");
  printf("xmlbufferΪ�ļ��ϴ��Ĳ��������£�\n");
  printf("<host>192.168.244.128:21</host> Զ�̷���˵�IP�Ͷ˿ڡ�\n");
  printf("<mode>1</mode> ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��\n");
  printf("<username>chenguan</username> Զ�̷����ftp���û�����\n");
  printf("<password>13251009668</password> Զ�̷����ftp�����롣\n");
  printf("<remotepath>/tmp/ftpputest</remotepath> Զ�̷���˴���ļ���Ŀ¼��\n");
  printf("<localpath>/tmp/idc/surfdata</localpath> �����ļ���ŵ�Ŀ¼��\n");
  printf("<matchname>SURF_ZH*.JSON</matchname> ���ϴ��ļ�ƥ��Ĺ���"\
         "��ƥ����ļ����ᱻ�ϴ������ֶξ��������þ�ȷ����������*ƥ��ȫ�����ļ���\n");
  printf("<ptype>1</ptype> �ļ��ϴ��ɹ��󣬱����ļ��Ĵ���ʽ��1-ʲôҲ������2-ɾ����3-���ݣ����Ϊ3����Ҫָ�����ݵ�Ŀ¼��\n");
  printf("<localpathbak>/tmp/idc/surfdatabak</localpathbak> �ļ��ϴ��ɹ��󣬱����ļ��ı���Ŀ¼���˲���ֻ�е�ptype=3ʱ����Ч��\n");
  printf("<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename> ���ϴ��ɹ��ļ����嵥���˲���ֻ�е�ptype=1ʱ����Ч��\n");
  printf("<timeout>80</timeout> �ϴ��ļ���ʱʱ�䣬��λ���룬���ļ���С��������������\n");
  printf("<pname>ftpputfiles_surfdata</pname> �������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣\n\n\n");
}

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"host",starg.host,30);   // Զ�̷���˵�IP�Ͷ˿ڡ�
  if (strlen(starg.host)==0)
  { logfile.Write("host is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);   // ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��
  if (starg.mode!=2)  starg.mode=1;

  GetXMLBuffer(strxmlbuffer,"username",starg.username,30);   // Զ�̷����ftp���û�����
  if (strlen(starg.username)==0)
  { logfile.Write("username is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"password",starg.password,30);   // Զ�̷����ftp�����롣
  if (strlen(starg.password)==0)
  { logfile.Write("password is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);   // Զ�̷���˴���ļ���Ŀ¼��
  if (strlen(starg.remotepath)==0)
  { logfile.Write("remotepath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);   // �����ļ���ŵ�Ŀ¼��
  if (strlen(starg.localpath)==0)
  { logfile.Write("localpath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // ���ϴ��ļ�ƥ��Ĺ���
  if (strlen(starg.matchname)==0)
  { logfile.Write("matchname is null.\n");  return false; }

  // �ϴ���ͻ����ļ��Ĵ���ʽ��1-ʲôҲ������2-ɾ����3-���ݡ�
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);   
  if ( (starg.ptype!=1) && (starg.ptype!=2) && (starg.ptype!=3) )
  { logfile.Write("ptype is error.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"localpathbak",starg.localpathbak,300); // �ϴ���ͻ����ļ��ı���Ŀ¼��
  if ( (starg.ptype==3) && (strlen(starg.localpathbak)==0) )
  { logfile.Write("localpathbak is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename,300); // ���ϴ��ɹ��ļ����嵥��
  if ( (starg.ptype==1) && (strlen(starg.okfilename)==0) )
  { logfile.Write("okfilename is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);   // ���������ĳ�ʱʱ�䡣
  if (starg.timeout==0) { logfile.Write("timeout is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);     // ��������
  if (strlen(starg.pname)==0) { logfile.Write("pname is null.\n");  return false; }

  return true;
}

// ��localpathĿ¼�µ��ļ����ص�vlistfile2�����С�
bool LoadLocalFile()
{
  vlistfile2.clear();

  CDir Dir;

  Dir.SetDateFMT("yyyymmddhh24miss");

  // ��������Ŀ¼��
  // ע�⣬�������Ŀ¼�µ����ļ�������10000�������ϴ��ļ����ܽ������⡣
  // ������deletefiles����ʱ������Ŀ¼�е���ʷ�ļ���
  if (Dir.OpenDir(starg.localpath,starg.matchname)==false)
  {
    logfile.Write("Dir.OpenDir(%s) ʧ�ܡ�\n",starg.localpath); return false;
  }

  struct st_fileinfo stfileinfo;

  while (true)
  {
    memset(&stfileinfo,0,sizeof(struct st_fileinfo));
   
    if (Dir.ReadDir()==false) break;

    strcpy(stfileinfo.filename,Dir.m_FileName);   // �ļ�����������Ŀ¼����
    strcpy(stfileinfo.mtime,Dir.m_ModifyTime);    // �ļ�ʱ�䡣

    vlistfile2.push_back(stfileinfo);
  }

  return true;
}

// ����okfilename�ļ��е����ݵ�����vlistfile1�С�
bool LoadOKFile()
{
  vlistfile1.clear();

  CFile File;

  // ע�⣺��������ǵ�һ���ϴ���okfilename�ǲ����ڵģ������Ǵ�������Ҳ����true��
  if ( (File.Open(starg.okfilename,"r"))==false )  return true;

  char strbuffer[501];

  struct st_fileinfo stfileinfo;

  while (true)
  {
    memset(&stfileinfo,0,sizeof(struct st_fileinfo));

    if (File.Fgets(strbuffer,300,true)==false) break;

    GetXMLBuffer(strbuffer,"filename",stfileinfo.filename);
    GetXMLBuffer(strbuffer,"mtime",stfileinfo.mtime);

    vlistfile1.push_back(stfileinfo);
  }

  return true;
}

// �Ƚ�vlistfile2��vlistfile1���õ�vlistfile3��vlistfile4��
bool CompVector()
{
  vlistfile3.clear(); vlistfile4.clear();

  int ii,jj;

  // ����vlistfile2��
  for (ii=0;ii<vlistfile2.size();ii++)
  {
    // ��vlistfile1�в���vlistfile2[ii]�ļ�¼��
    for (jj=0;jj<vlistfile1.size();jj++)
    {
      // ����ҵ��ˣ��Ѽ�¼����vlistfile3��
      if ( (strcmp(vlistfile2[ii].filename,vlistfile1[jj].filename)==0) &&
           (strcmp(vlistfile2[ii].mtime,vlistfile1[jj].mtime)==0) )
      {
        vlistfile3.push_back(vlistfile2[ii]); break;
      }
    }

    // ���û���ҵ����Ѽ�¼����vlistfile4��
    if (jj==vlistfile1.size()) vlistfile4.push_back(vlistfile2[ii]);
  }

  return true;
}

// ������vlistfile3�е�����д��okfilename�ļ�������֮ǰ�ľ�okfilename�ļ���
bool WriteToOKFile()
{
  CFile File;    

  if (File.Open(starg.okfilename,"w")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",starg.okfilename); return false;
  }

  for (int ii=0;ii<vlistfile3.size();ii++)
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",vlistfile3[ii].filename,vlistfile3[ii].mtime);

  return true;
}

// ���ptype==1�����ϴ��ɹ����ļ���¼׷�ӵ�okfilename�ļ��С�
bool AppendToOKFile(struct st_fileinfo *stfileinfo)
{
  CFile File;

  if (File.Open(starg.okfilename,"a")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",starg.okfilename); return false;
  }

  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",stfileinfo->filename,stfileinfo->mtime);

  return true;
}
