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
  char matchname[101];     // �������ļ�ƥ��Ĺ���
  char listfilename[301];  // ����ǰ�г�������ļ������ļ���
  int  ptype;              // ���غ������ļ��Ĵ���ʽ��1-ʲôҲ������2-ɾ����3-���ݡ�
  char remotepathbak[301]; // ���غ������ļ��ı���Ŀ¼��
  char okfilename[301];    // �����سɹ��ļ����嵥��
  bool checkmtime;         // �Ƿ���Ҫ��������ļ���ʱ�䣬true-��Ҫ��false-����Ҫ��ȱʡΪfalse��
  int  timeout;            // ���������ĳ�ʱʱ�䡣
  char pname[51];          // ��������������"ftpgetfiles_��׺"�ķ�ʽ��
} starg;

// �ļ���Ϣ�Ľṹ�塣
struct st_fileinfo
{
  char filename[301];   // �ļ�����
  char mtime[21];       // �ļ�ʱ�䡣
};

vector<struct st_fileinfo> vlistfile1;    // �����سɹ��ļ�������������okfilename�м��ء�
vector<struct st_fileinfo> vlistfile2;    // ����ǰ�г�������ļ�������������nlist�ļ��м��ء�
vector<struct st_fileinfo> vlistfile3;    // ���β���Ҫ���ص��ļ���������
vector<struct st_fileinfo> vlistfile4;    // ������Ҫ���ص��ļ���������

// ����okfilename�ļ��е����ݵ�����vlistfile1�С�
bool LoadOKFile();

// �Ƚ�vlistfile2��vlistfile1���õ�vlistfile3��vlistfile4��
bool CompVector();

// ������vlistfile3�е�����д��okfilename�ļ�������֮ǰ�ľ�okfilename�ļ���
bool WriteToOKFile();

// ���ptype==1�������سɹ����ļ���¼׷�ӵ�okfilename�ļ��С�
bool AppendToOKFile(struct st_fileinfo *stfileinfo);

// ��ftp.nlist()������ȡ����list�ļ����ص�����vlistfile2�С�
bool LoadListFile();

CLogFile logfile;

Cftp ftp;

// �����˳����ź�2��15�Ĵ�������
void EXIT(int sig);

void _help();

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

// �����ļ����ܵ���������
bool _ftpgetfiles();

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

  _ftpgetfiles();

  ftp.logout();

  return 0;
}

// �����ļ����ܵ���������
bool _ftpgetfiles()
{
  // ����ftp����˴���ļ���Ŀ¼��
  if (ftp.chdir(starg.remotepath)==false)
  {
    logfile.Write("ftp.chdir(%s) failed.\n",starg.remotepath); return false;
  }

  // ����ftp.nlist()�����г������Ŀ¼�е��ļ��������ŵ������ļ��С�
  if (ftp.nlist(".",starg.listfilename)==false)
  {
    logfile.Write("ftp.nlist(%s) failed.\n",starg.remotepath); return false;
  }

  PActive.UptATime();   // ���½��̵�������

  // ��ftp.nlist()������ȡ����list�ļ����ص�����vlistfile2�С�
  if (LoadListFile()==false)
  {
    logfile.Write("LoadListFile() failed.\n");  return false;
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

    logfile.Write("get %s ...",strremotefilename);

    // ����ftp.get()�����ӷ���������ļ���
    if (ftp.get(strremotefilename,strlocalfilename)==false) 
    {
      logfile.WriteEx("failed.\n"); return false;
    }

    logfile.WriteEx("ok.\n");

    PActive.UptATime();   // ���½��̵�������
    
    // ���ptype==1�������سɹ����ļ���¼׷�ӵ�okfilename�ļ��С�
    if (starg.ptype==1) AppendToOKFile(&vlistfile2[ii]);

    // ɾ������˵��ļ���
    if (starg.ptype==2) 
    {
      if (ftp.ftpdelete(strremotefilename)==false)
      {
        logfile.Write("ftp.ftpdelete(%s) failed.\n",strremotefilename); return false;
      }
    }

    // �ѷ���˵��ļ�ת�浽����Ŀ¼��
    if (starg.ptype==3) 
    {
      char strremotefilenamebak[301];
      SNPRINTF(strremotefilenamebak,sizeof(strremotefilenamebak),300,"%s/%s",starg.remotepathbak,vlistfile2[ii].filename);
      if (ftp.ftprename(strremotefilename,strremotefilenamebak)==false)
      {
        logfile.Write("ftp.ftprename(%s,%s) failed.\n",strremotefilename,strremotefilenamebak); return false;
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
  printf("Using:/project/tools11/bin/ftpgetfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools11/bin/procctl 30 /project/tools11/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log \"<host>192.168.244.128:21</host><mode>1</mode><username>chenguan</username><password>13251009668</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>\"\n\n\n");

  printf("��������ͨ�õĹ���ģ�飬���ڰ�Զ��ftp����˵��ļ����ص�����Ŀ¼��\n");
  printf("logfilename�Ǳ��������е���־�ļ���\n");
  printf("xmlbufferΪ�ļ����صĲ��������£�\n");
  printf("<host>192.168.244.128:21</host> Զ�̷���˵�IP�Ͷ˿ڡ�\n");
  printf("<mode>1</mode> ����ģʽ��1-����ģʽ��2-����ģʽ��ȱʡ���ñ���ģʽ��\n");
  printf("<username>chenguan</username> Զ�̷����ftp���û�����\n");
  printf("<password>13251009668</password> Զ�̷����ftp�����롣\n");
  printf("<remotepath>/tmp/idc/surfdata</remotepath> Զ�̷���˴���ļ���Ŀ¼��\n");
  printf("<localpath>/idcdata/surfdata</localpath> �����ļ���ŵ�Ŀ¼��\n");
  printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> �������ļ�ƥ��Ĺ���"\
         "��ƥ����ļ����ᱻ���أ����ֶξ��������þ�ȷ����������*ƥ��ȫ�����ļ���\n");
  printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename> ����ǰ�г�������ļ������ļ���\n");
  printf("<ptype>1</ptype> �ļ����سɹ���Զ�̷�����ļ��Ĵ���ʽ��1-ʲôҲ������2-ɾ����3-���ݣ����Ϊ3����Ҫָ�����ݵ�Ŀ¼��\n");
  printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> �ļ����سɹ��󣬷�����ļ��ı���Ŀ¼���˲���ֻ�е�ptype=3ʱ����Ч��\n");
  printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> �����سɹ��ļ����嵥���˲���ֻ�е�ptype=1ʱ����Ч��\n");
  printf("<checkmtime>true</checkmtime> �Ƿ���Ҫ��������ļ���ʱ�䣬true-��Ҫ��false-����Ҫ���˲���ֻ�е�ptype=1ʱ����Ч��ȱʡΪfalse��\n");
  printf("<timeout>80</timeout> �����ļ���ʱʱ�䣬��λ���룬���ļ���С��������������\n");
  printf("<pname>ftpgetfiles_surfdata</pname> �������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣\n\n\n");
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

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // �������ļ�ƥ��Ĺ���
  if (strlen(starg.matchname)==0)
  { logfile.Write("matchname is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"listfilename",starg.listfilename,300);   // ����ǰ�г�������ļ������ļ���
  if (strlen(starg.listfilename)==0)
  { logfile.Write("listfilename is null.\n");  return false; }

  // ���غ������ļ��Ĵ���ʽ��1-ʲôҲ������2-ɾ����3-���ݡ�
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);   
  if ( (starg.ptype!=1) && (starg.ptype!=2) && (starg.ptype!=3) )
  { logfile.Write("ptype is error.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"remotepathbak",starg.remotepathbak,300); // ���غ������ļ��ı���Ŀ¼��
  if ( (starg.ptype==3) && (strlen(starg.remotepathbak)==0) )
  { logfile.Write("remotepathbak is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename,300); // �����سɹ��ļ����嵥��
  if ( (starg.ptype==1) && (strlen(starg.okfilename)==0) )
  { logfile.Write("okfilename is null.\n");  return false; }

  // �Ƿ���Ҫ��������ļ���ʱ�䣬true-��Ҫ��false-����Ҫ���˲���ֻ�е�ptype=1ʱ����Ч��ȱʡΪfalse��
  GetXMLBuffer(strxmlbuffer,"checkmtime",&starg.checkmtime);

  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);   // ���������ĳ�ʱʱ�䡣
  if (starg.timeout==0) { logfile.Write("timeout is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);     // ��������
  if (strlen(starg.pname)==0) { logfile.Write("pname is null.\n");  return false; }

  return true;
}

// ��ftp.nlist()������ȡ����list�ļ����ص�����vlistfile2�С�
bool LoadListFile()
{
  vlistfile2.clear();

  CFile  File;

  if (File.Open(starg.listfilename,"r")==false)
  {
    logfile.Write("File.Open(%s) ʧ�ܡ�\n",starg.listfilename); return false;
  }

  struct st_fileinfo stfileinfo;

  while (true)
  {
    memset(&stfileinfo,0,sizeof(struct st_fileinfo));
   
    if (File.Fgets(stfileinfo.filename,300,true)==false) break;

    if (MatchStr(stfileinfo.filename,starg.matchname)==false) continue;

    if ( (starg.ptype==1) && (starg.checkmtime==true) )
    {
      // ��ȡftp������ļ�ʱ�䡣
      if (ftp.mtime(stfileinfo.filename)==false)
      {
        logfile.Write("ftp.mtime(%s) failed.\n",stfileinfo.filename); return false;
      }
    
      strcpy(stfileinfo.mtime,ftp.m_mtime);
    }

    vlistfile2.push_back(stfileinfo);
  }

  /*
  for (int ii=0;ii<vlistfile2.size();ii++)
    logfile.Write("filename=%s=\n",vlistfile2[ii].filename);
  */

  return true;
}

// ����okfilename�ļ��е����ݵ�����vlistfile1�С�
bool LoadOKFile()
{
  vlistfile1.clear();

  CFile File;

  // ע�⣺��������ǵ�һ�����أ�okfilename�ǲ����ڵģ������Ǵ�������Ҳ����true��
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

// ���ptype==1�������سɹ����ļ���¼׷�ӵ�okfilename�ļ��С�
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
