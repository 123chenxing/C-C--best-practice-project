/*
 *  ��������xmltodb.cpp�����������������ĵĹ�������ģ�飬���ڰ�xml�ļ���⵽MYSQL�ı��С�
 *  ���ߣ��¹ڡ�
*/
#include "_public.h"
#include "_mysql.h"

// �������в����Ľṹ�塣
struct st_arg
{
  char connstr[101];     // ���ݿ�����Ӳ�����
  char charset[51];      // ���ݿ���ַ�����
  char inifilename[301]; // �������Ĳ��������ļ���
  char xmlpath[301];     // �����xml�ļ���ŵ�Ŀ¼��
  char xmlpathbak[301];  // xml�ļ�����ı���Ŀ¼��
  char xmlpatherr[301];  // ���ʧ�ܵ�xml�ļ���ŵ�Ŀ¼��
  int timetvl;           // ���������е�ʱ������������פ�ڴ档 
  int timeout;           // ����������ʱ�ĳ�ʱʱ�䡣
  char pname[51];        // ����������ʱ�ĳ�������
  char starttime[52];    // �������е�ʱ������
  char incfield[31];     // �����ֶ�����
  char incfilename[301]; // �ѳ�ȡ���ݵĵ����ֶ����ֵ��ŵ��ļ���
  char connstr1[101];    // �ѳ�ȡ���ݵĵ����ֶ����ֵ��ŵ����ݿ�����Ӳ�����
  int  timeout;          // ���������ĳ�ʱʱ�䡣
  char pname[51];        // ��������������"dminingmysql_��׺"�ķ�ʽ��
} starg;

// ��ʾ����İ���
void _help(char *argv[]);

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

CPActive PActive;  // ����������

int main(int argc,char *argv[])
{
  if (argc!=3) { _help(); return -1; }

  // �ر�ȫ�����źź����������
  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ���̡�
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ��
  // CloseIOAndSignal();
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[1],"a+")==false)
  {
    printf("����־�ļ�ʧ�ܣ�%s����\n",argv[1]); return -1;
  }

  // ��xml����������starg�ṹ��
  if (_xmltoarg(argv[2])==false) return -1;

  if (conn.connecttodb(starg.connstr,starg.charset)!=0)
  {
    printf("connect database(%s) failed.\n%s\n",starg.connstr,conn.m_cda.message);
    EXIT(-1);
  }
  logfile.Write("connect database(%s) ok.\n",starg.connstr);

}
  


// ��ʾ����İ���
void _help(char *argv[])
{
  printf("Using:/project/tools11/bin/xmltodb logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools11/bin/procctl 10 /project/tools11/bin/xmltodb /log/idc/xmltodb_vip1.log \"<connstr>127.0.0.1,root,13251009668,test2,3306</connstr><charset>utf8</charset><inifilename>/project/tools/ini/xmltodb.xml</inifilename><xmlpath>/idcdata/xmltodb/vip1</xmlpath><xmlpathbak>/idcdata/xmltodb/vip1bak</xmlpathbak><xmlpatherr>/idcdata/xmltodb/vip1err</xmlpatherr><timetvl>5</timetvl><timeout>50</timeout><pname>xmltodb_vip1</pname>\"\n\n");

  printf("���������������ĵĹ�������ģ�飬���ڰ�xml�ļ���⵽MYSQL�ı��С�\n");
  printf("logfilename ���������е���־�ļ���\n");
  printf("xmlbuffer   ���������еĲ�������xml��ʾ���������£�\n\n");

  printf("connstr     ���ݿ�����Ӳ�������ʽ��ip,username,password,dbname,port��\n");
  printf("charset     ���ݿ���ַ������������Ҫ������Դ���ݿⱣ��һ�£�����������������������\n");
  printf("inifilename �������Ĳ��������ļ���\n");
  printf("xmlpath     �����xml�ļ���ŵ�Ŀ¼��\n");
  printf("xmlpathbak  xml�ļ�����ı���Ŀ¼��\n");
  printf("xmlpatherr  ���ʧ�ܵ�xml�ļ���ŵ�Ŀ¼��\n");
  printf("timetvl     �������ʱ��������λ���룬��ҵ�����������2-30֮�䡣\n");
  printf("timeout     ������ĳ�ʱʱ�䣬��λ���룬��xml�ļ���С��������������30���ϡ�\n");
  printf("pname       �������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣\n");

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"connstr",starg.connstr,100);
  if (strlen(starg.connstr)==0) { logfile.Write("connstr is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"charset",starg.charset,50);
  if (strlen(starg.charset)==0) { logfile.Write("charset is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"inifilename",starg.inifilename,300);
  if (strlen(starg.inifilename)==0) { logfile.Write("inifilename is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"xmlpath",starg.xmlpath,300);
  if (strlen(starg.xmlpath)==0) { logfile.Write("xmlpath is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"xmlpathbak",starg.xmlpathbak,300);
  if (strlen(starg.xmlpathbak)==0) { logfile.Write("xmlpathbak is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"xmlpatherr",starg.xmlpatherr,300);
  if (strlen(starg.xmlpatherr)==0) { logfile.Write("xmlpatherr is null.\n"); return false; }

  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if (starg.timetvl<2) starg.timetvl=2;
  if (starg.timetvl>30) starg.timetvl=30;

  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if (starg.timeout==0) { logfile.Write("timeout is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50); 
  if (strlen(starg.pname)==0) { logfile.Write("pname is null.\n");  
  return false;}

  return true;
} 

void EXIT(int sig)
{ 
  printf("�����˳���sig=%d\n\n",sig);
  
  conn.disconnect();
}    
