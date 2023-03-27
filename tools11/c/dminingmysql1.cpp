/*
 *  ��������dminingmysql.cpp�����������������ĵĹ�������ģ�飬���ڴ�mysql���ݿ�Դ���ȡ���ݣ�����xml�ļ���
 *  ���ߣ��¹ڡ�
*/


#include "_public.h"
#include "_mysql.h"

// �������в����Ľṹ�塣
struct st_arg
{
  char connstr[101];       // ���ݿ�����Ӳ�����
  char charset[51];        // ���ݿ���ַ�����
  char selectsql[1024];    // ������Դ���ݿ��ȡ���ݵ�SQL��䡣   
  char fieldstr[501];      // ��ȡ���ݵ�SQL������������ֶ������ֶ���֮���ö��ŷָ���
  char fieldlen[501];      // ��ȡ���ݵ�SQL������������ֶεĳ��ȣ��ö��ŷָ���
  char bfilename[31];      // ���xml�ļ���ǰ׺��
  char efilename[31];      // ���xml�ļ��ĺ�׺��
  char outpath[301];       // ���xml�ļ���ŵ�Ŀ¼�� 
  char starttime[52];      // �������е�ʱ�����䡣
  char incfield[31];       // �����ֶ�����
  char incfilename[301];   // �ѳ�ȡ���ݵĵ����ֶ����ֵ��ŵ��ļ���
  int timeout;             // ���������ĳ�ʱʱ�䡣
  char pname[51];          // ��������������"dminingmysql_��׺"�ķ�ʽ��
} starg;


CLogFile logfile;


// �����˳����ź�2��15�Ĵ�������
void EXIT(int sig);

void _help();

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

// �ϴ��ļ����ܵ���������
bool _dminingmysql();

CPActive PActive;  // ����������

int main(int argc,char *argv[])
{
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

  PActive.AddPInfo(starg.timeout,starg.pname);  // �ѽ��̵�������Ϣд�빲���ڴ档

  // logfile.Write("ftp.login ok.\n");  // ��ʽ���к󣬿���ע�����д��롣

  _dminingmysql();

  return 0;
}

// �ϴ��ļ����ܵ���������
bool _dminingmysql()
{

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
  printf("Using:/project/tools11/bin/dminingmysql logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools11/bin/procctl 3600 /project/tools11/bin/dminingmysql /log/idc/dminingmysql_ZHOBTCODE.log \"<connstr>127.0.0.1,root,13251009668,test2,3306</connstr><charset>utf8</charset><selectsql>select obtid,cityname,provname,lat,lon,height from T_ZHOBTCOE</selectsql><fieldstr>obtid,cityname,provname,lat,lon,height</fieldstr><fieldlen>10,30,30,10,10,10</fieldlen><bfilename>ZHOBTCODE</bfilename><efilename>HYCZ</efilename><outpath>/idcdata/dmindata</outpath>\"\n\n");
  printf("	/project/tools11/bin/procctl 30 /project/tools11/bin/dminingmysql_ZHOBTMIND.log\"<connstr>127.0.0.1,root,13251009668,test2,3306</connstr><charset>utf8</charset><selectsql>select obtid,date_format(ddatetime,'%%%%Y-%%%%m-%%%%d %%%%H:%%%%m:%%%%s'),t,p,u,wd,wf,r,vis,keyid from t_zhobtmind where keyid>:1 and ddatetime>timestampadd(minute,-120,now())</selectsql><fieldstr>obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>10,19,8,8,8,8,8,8,8,15</fieldlen><bfilename>ZHOBTMIND</bfilename><efilename>HYCZ</efilename><outpath>/idcdata/dmindata</outpath><starttime></starttime><incfield>keyid</incfield><incfilename>/idcdata/dmining/dminingmysql_ZHOBTMIND_HYCZ.list</incfilename>\"\n\n");
  printf("���������������ĵĹ�������ģ�飬���ڴ�mysql���ݿ�Դ���ȡ���ݣ�����xml�ļ���\n");
  printf("logfilename	���������е���־�ļ���\n");
  printf("xmlbuffer	���������еĲ�������xml��ʾ���������£�\n\n");
  printf("connstr	���ݿ�����Ӳ�������ʽ��ip,username,password,dbname,port��\n");
  printf("charset	���ݿ���ַ������������Ҫ������Դ���ݿⱣ��һ�£�����������������������\n");
  printf("selectsql	������Դ���ݿ��ȡ���ݵ�SQL��䣬ע�⣺ʱ�亯���İٷֺ�%��Ҫ�ĸ�����ʾ����������������prepare֮��ʣһ����\n");
  printf("fieldstr	��ȡ���ݵ�SQL������������ֶ������м��ö��ŷָ�������Ϊxml�ļ����ֶ�����\n");
  printf("fieldlen	��ȡ���ݵ�SQL������������ֶεĳ��ȣ��м��ö��ŷָ���fieldstr��fieldlen���ֶα���һһ��Ӧ��\n");
  printf("bfilename	���xml�ļ���ǰ׺��\n");
  printf("efilename	����ļ��ĺ�׺��\n");
  printf("outpath	���xml�ļ���ŵ�Ŀ¼��\n");
  printf("starttime	�������е�ʱ�����䣬����02,13��ʾ�������������ʱ��̤��02ʱ��13ʱ�����У�����ʱ�䲻���С����starttimeΪ�գ���ôstarttime������ʧЧ��ֻҪ�����������ͻ�ִ�����ݳ�ȡ��Ϊ�˼�������Դ��ѹ���������ݿ��ȡ���ݵ�ʱ��һ���ڶԷ����ݿ����е�ʱ��ʱ���С�\n");
  printf("incfield	�����ֶ�������������fieldstr���ֶ���������ֻ�������ͣ�һ��Ϊ�����ֶΡ����incfieldΪ�գ���ʾ������������ȡ������\n");
  printf("incfilename	�ѳ�ȡ���ݵĵ����ֶ����ֵ��ŵ��ļ���������ļ���ʧ��������ȫ�������ݡ�\n\n\n");
  printf("timeout	������ĳ�ʱʱ�䣬��λ���롣\n");
  printf("pname		�������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣\n\n");
}
  

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"connstr",starg.connstr,100);
  if (strlen(starg.connstr)==0){logfile.Write("connstr is null.\n"); return false;}  

  GetXMLBuffer(strxmlbuffer,"charset",starg.charset,50);
  if (strlen(starg.charset)==0) {logfile.Write("charset is null.\n"); return false;}
  
  
  GetXMLBuffer(strxmlbuffer,"selectsql",starg.selectsql,1000);
  if (strlen(starg.selectsql)==0)
  { logfile.Write("selectsql is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"fieldstr",starg.fieldstr,500);
  if (strlen(starg.fieldstr)==0)
  { logfile.Write("fieldstr is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"fieldlen",starg.fieldlen,500);
  if (strlen(starg.fieldlen)==0)
  { logfile.Write("fieldlen is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"bfilename",starg.bfilename,30);
  if (strlen(starg.bfilename)==0)
  { logfile.Write("bfilename is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"efilename",starg.efilename,30);
  if (strlen(starg.efilename)==0)
  { logfile.Write("efilename is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"outpath",starg.outpath,300);   
  if (strlen(starg.outpath)==0) {logfile.Write("outpath is null.\n"); return false;}
  
   GetXMLBuffer(strxmlbuffer,"starttime",starg.starttime,50);  // ��ѡ������

   GetXMLBuffer(strxmlbuffer,"incfield",starg.incfield,30);    // ��ѡ������

   GetXMLBuffer(strxmlbuffer,"incfilename",starg.incfilename,300); // ��ѡ������  

  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);  // ���������ĳ�ʱʱ�䡣
  if (starg.timeout==0)
  { logfile.Write("timeout is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);   // ��������
  if (starg.pname==0) { logfile.Write("pname is null.\n");  return false; }

  return true;
}

