/*
 *  ��������xmltodb.cpp�����������������ĵĹ�������ģ�飬���ڰ�xml�ļ���⵽MYSQL�ı��С�
 *  ���ߣ��¹ڡ�
*/
#include "_public.h"
#include "_mysql.h"

// ����У��ֶΣ���Ϣ�Ľṹ�塣
struct st_columns
{
  char colname[31];    // ������
  char datatype[31];   // �е��������ͣ���Ϊnumber��date��char�����ࡣ
  int collen;          // �еĳ��ȣ�number�̶�20��date�̶�19��char�ĳ����ɱ�ṹ������
  int pkseq;           // ��������������ֶΣ���������ֶε�˳�򣬴�1��ʼ����������ȡֵ0�� 

};

// ��ȡ��ȫ�����к���������Ϣ���ࡣ
class CTABCOLS
{
  public:
    CTABCOLS();

  int m_allcount;   // ȫ���ֶεĸ�����  
  int m_pkcount;    // �����ֶεĸ�����

  vector<struct st_columns> m_vallcols;  // ���ȫ���ֶ���Ϣ��������
  vector<struct st_columns> m_vpkcols;   // ��������ֶ���Ϣ��������

  char m_allcols[3001];  // ȫ���ֶ����б����ַ�����ţ��м��ð�ǵĶ��ŷָ���
  char m_pkcols[301];    // �����ֶ����б����ַ�����ţ��м��ð�ǵĶ��ŷָ���

  void initdata();       // ��Ա������ʼ����

  // ��ȡָ�����ȫ���ֶ���Ϣ��
  bool allcols(connection *conn,char *tablename);

  // ��ȡָ����������ֶ���Ϣ��
  bool pkcols(connection *conn,char *tablename);

};
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
} starg;

// ��ʾ����İ���
void _help(char *argv[]);

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

CLogFile logfile;

connection conn;

void EXIT(int sig);

// ҵ������������
bool _xmltodb();

struct st_xmltotable
{
  char filename[101];   // xml�ļ���ƥ������ö��ŷָ���
  char tname[31];       // �����ı�����
  int uptbz;            // ���±�־��1-���£�2-�����¡�
  char execsql[301];    // ����xml�ļ�֮ǰ��ִ�е�SQL��䡣
} stxmltotable;

vector<struct st_xmltotable> vxmltotable;    // �������Ĳ�����������

// ���������Ĳ��������ļ�starg.inifilename���ص�vxmltotable�����С�
bool loadxmltotable();

// ��vxmltotable�����в���xmlfilename���������������stxmltotable�ṹ���С�
bool findxmltotable(char *xmlfilename);

// ���ô���xml�ļ����Ӻ���������ֵ��0-�ɹ��������Ķ���ʧ�ܣ�ʧ�ܵ�����кܶ��֣���ʱ��ȷ����
int _xmltodb(char *fullfilename,char *filename);

// ��xml�ļ��ƶ�������Ŀ¼�����Ŀ¼��
bool xmltobakerr(char *fullfilename,char *srcpath,char *dstpath);

int main(int argc,char *argv[])
{
  if (argc!=3) { _help(argv); return -1; }

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

  // ҵ������������
  _xmltodb();
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
}

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
  if (strlen(starg.pname)==0) { logfile.Write("pname is null.\n"); return false;}

  return true;
}

void EXIT(int sig)
{ 
  printf("�����˳���sig=%d\n\n",sig); 
 
  conn.disconnect();
 
  exit(0);
} 

// ҵ������������
bool _xmltodb()
{
  int counter=50;        // �����������ļ���������ʼ��Ϊ50��Ϊ���ڵ�һ�ν���ѭ����ʱ��ͼ��ز�����

  CDir Dir;

  while (true)
  {
    if (counter++>30)
    {
      counter=0;   // ���¼�����
      
      // ���������Ĳ��������ļ�starg.inifilename���ص�vxmltotable�����С�

      if (loadxmltotable()==false) return false; 
    }
    // ��starg.xmlpathĿ¼��Ϊ�˱�֤�����ɵ�������⣬��Ŀ¼��ʱ��Ӧ�ð��ļ�������
    if (Dir.OpenDir(starg.xmlpath,"*.XML",10000,false,true)==false)
    {
      logfile.Write("Dir.OpenDir(%s) failed.\n",starg.xmlpath); return false;
    }

    while (true)
    {
      // ��ȡĿ¼���õ�һ��xml�ļ���
      if (Dir.ReadDir()==false) break;

      logfile.Write("�����ļ�%s...",Dir.m_FullFileName);

      // ���ô���xml�ļ����Ӻ�����
      int iret= _xmltodb(Dir.m_FullFileName,Dir.m_FileName);      

      // ����xml�ļ��ɹ���д��־�������ļ���
      if (iret==0)
      {
        logfile.WriteEx("ok.\n");
        // ��xml�ļ��ƶ���starg.xmlpathbak����ָ����Ŀ¼�У�һ�㲻�ᷢ�����������ķ����ˣ������˳���
        if (xmltobakerr(Dir.m_FullFileName,starg.xmlpath,starg.xmlpathbak)==false) return false; 
      }
      // 1-û��������������2-�����ı����ڣ�
      if ((iret==1) || (iret==2))
      {
        if (iret==1) logfile.WriteEx("failed��û��������������\n");
        if (iret==2) logfile.WriteEx("failed�������ı�%s�������ڡ�\n",stxmltotable.tname);

        // ��xml�ļ��ƶ���starg.xmlpatherr����ָ����Ŀ¼�У�һ�㲻�ᷢ�����������ķ����ˣ������˳���
        if (xmltobakerr(Dir.m_FullFileName,starg.xmlpath,starg.xmlpatherr)==false) return false;
      }

      // ���ݿ���󣬺������أ������˳���
      if (iret==4)
      {
        logfile.WriteEx("failed�����ݿ����\n"); return false;
      }
    }

    break;
    sleep(starg.timetvl);
  }

  return true;
} 

// ���ô���xml�ļ����Ӻ���������ֵ��0-�ɹ��������Ķ���ʧ�ܣ�ʧ�ܵ�����кܶ��֣���ʱ��ȷ����
int _xmltodb(char *fullfilename,char *filename)
{
  // ��vxmltotable�����в���filenam���������������stxmltotable�ṹ���С�
  if (findxmltotable(filename)==false) return 1;

  CTABCOLS TABCOLS;

  // ��ȡ��ȫ�����ֶκ�������Ϣ�������ȡʧ�ܣ�Ӧ�������ݿ�������ʧЧ��
  // �ڱ��������еĹ����У�������ݿ�����쳣��һ���������﷢�֡�
  if (TABCOLS.allcols(&conn,stxmltotable.tname)==false) return 4;
  if (TABCOLS.pkcols(&conn,stxmltotable.tname)==false)  return 4;

  // ���TABCOLS.m_allcountΪ0��˵������������ڣ�����2��
  if (TABCOLS.m_allcount==0) return 2; 
  
  // ƴ�����ɲ���͸��±����ݵ�SQL��

  // prepare����͸��µ�sql��䣬�����������

  // �ڴ���xml�ļ�֮ǰ�����stxmltotable.execsql��Ϊ�գ���ִ������

  // ��xml�ļ���

 /* while (true)
  {
    // ��xml�ļ��ж�ȡһ�С�
    
    // ����xml��������Ѱ󶨵���������С�
    
    // ִ�в���͸��µ�SQL�� 

  }
*/
  return 0;
}


// ���������Ĳ��������ļ�starg.inifilename���ص�vxmltotable�����С�
bool loadxmltotable()
{
  vxmltotable.clear();

  CFile File;
  if (File.Open(starg.inifilename,"r")==false)
  {
    logfile.Write("File.Open(%s) ʧ�ܡ�\n",starg.inifilename);  
    return false;
  }

  char strBuffer[501];

  while (true)
  {
    if (File.FFGETS(strBuffer,500,"<endl/>")==false) break;

    memset(&stxmltotable,0,sizeof(struct st_xmltotable));

    GetXMLBuffer(strBuffer,"filename",stxmltotable.filename,100); // xml�ļ���ƥ������ö��ŷָ���
    GetXMLBuffer(strBuffer,"tname",stxmltotable.tname,30);        // �����ı�����
    GetXMLBuffer(strBuffer,"uptbz",&stxmltotable.uptbz);          // ���±�־��1-���£�2-�����¡�
    GetXMLBuffer(strBuffer,"execsql",stxmltotable.execsql,300);   // ����xml�ļ�֮ǰ��ִ�е�SQL��䡣

    vxmltotable.push_back(stxmltotable);
  }

  logfile.Write("loadxmltotable(%s) ok.\n",starg.inifilename);

  return true;
}


// ��vxmltotable�����в���xmlfilename���������������stxmltotable�ṹ���С�
bool findxmltotable(char *xmlfilename)
{
  for (int ii=0;ii<vxmltotable.size();ii++)
  {
    if (MatchStr(xmlfilename,vxmltotable[ii].filename)==false)
    {
      memcpy(&stxmltotable,&vxmltotable[ii],sizeof(struct st_xmltotable));
      return true;
    }
  }

  return false; 
}

// ��xml�ļ��ƶ�������Ŀ¼�����Ŀ¼��
bool xmltobakerr(char *fullfilename,char *srcpath,char *dstpath)
{
  char dstfilename[301];     // Ŀ���ļ�����
  STRCPY(dstfilename,sizeof(dstfilename),fullfilename);

  UpdateStr(dstfilename,srcpath,dstpath,false);   // С�ĵ��ĸ�������һ��Ҫ��false��

  if (RENAME(fullfilename,dstfilename)==false)
  {
    logfile.Write("RENAME(%s,%s) failed.\n",fullfilename,dstfilename);
    return false;
  }
  return true;
}

// ��ȡ��ȫ�����к���������Ϣ���ࡣ
CTABCOLS::CTABCOLS()
{
  initdata();  // ���ó�Ա������ʼ��������

}

void CTABCOLS::initdata()  // ��Ա������ʼ����
{
  m_allcount=m_pkcount=0;
  m_vallcols.clear();      // ���������
  m_vpkcols.clear();       // ���������
  memset(m_allcols,0,sizeof(m_allcols));
  memset(m_pkcols,0,sizeof(m_pkcols));
}

// ��ȡָ�����ȫ���ֶ���Ϣ��
bool CTABCOLS::allcols(connection *conn,char *tablename)
{
  m_allcount=0;
  m_vallcols.clear();
  memset(m_allcols,0,sizeof(m_allcols));
  
  struct st_columns stcolumns;

  sqlstatement stmt;
  stmt.connect(conn);
  stmt.prepare("select lower(column_name),lower(data_type),character_maximum_length from information_schema.COLUMNS where table_name=:1");
  stmt.bindin(1,tablename,30);
  stmt.bindout(1,stcolumns.colname,30);
  stmt.bindout(2,stcolumns.datatype,30);
  stmt.bindout(3,&stcolumns.collen);

  if (stmt.execute()!=0) return false;

  while (true)
  {
    memset(&stcolumns,0,sizeof(struct st_columns));

    if (stmt.next()!=0) break;

    // �е��������ͣ���Ϊnumber��date��char�����ࡣ
    if (strcmp(stcolumns.datatype,"char")==0) strcpy(stcolumns.datatype,"char");
    if (strcmp(stcolumns.datatype,"varchar")==0) strcpy(stcolumns.datatype,"char");

    if (strcmp(stcolumns.datatype,"datetime")==0) strcpy(stcolumns.datatype,"date");
    if (strcmp(stcolumns.datatype,"timestamp")==0) strcpy(stcolumns.datatype,"date");

    if (strcmp(stcolumns.datatype,"tinyint")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"smallint")==0) strcpy(stcolumns.datatype,"number");    
    if (strcmp(stcolumns.datatype,"mediumint")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"int")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"integer")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"bigint")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"numeric")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"decimal")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"float")==0) strcpy(stcolumns.datatype,"number");
    if (strcmp(stcolumns.datatype,"double")==0) strcpy(stcolumns.datatype,"number");

    // ���ҵ������Ҫ�������޸�����Ĵ��룬���ӶԸ����������͵�֧�֡�
    // ����ֶ����������Ͳ��������г������У���������
    if ((strcmp(stcolumns.datatype,"char")!=0) && (strcmp(stcolumns.datatype,"datate")!=0) && ((stcolumns.datatype,"number")!=0))
    continue;

    // ����ֶ�������date���ѳ�������Ϊ19��yyyy-mm-dd hh:mi:ss
    if (strcmp(stcolumns.datatype,"date")==0) stcolumns.collen=19;

    // ����ֶ�������number���ѳ�������Ϊ20��
    if (strcmp(stcolumns.datatype,"number")==0) stcolumns.collen=20;

    strcat(m_allcols,stcolumns.colname) ; strcat(m_allcols,",");

    m_vallcols.push_back(stcolumns);

    m_allcount++;    
  }

  // ɾ��m_allcols���һ������Ķ��š�
  if (m_allcount>0) m_allcols[strlen(m_allcols)-1]=0;
  return true;
}

// ��ȡָ����������ֶ���Ϣ��
bool CTABCOLS::pkcols(connection *conn,char *tablename)
{
  m_pkcount=0;
  memset(m_pkcols,0,sizeof(m_pkcols));
  m_vpkcols.clear();

  struct st_columns stcolumns;

  sqlstatement stmt;
  stmt.connect(conn);
  stmt.prepare("select lower(column_name),seq_in_index from information_schema.STATISTICS where table_name=:1 and index_name='primary' order by seq_in_index");
  stmt.bindin(1,tablename,30);
  stmt.bindout(1,stcolumns.colname,30);
  stmt.bindout(2,&stcolumns.pkseq);

  if (stmt.execute()!=0) return false;

  while (true)
  {
    memset(&stcolumns,0,sizeof(struct st_columns));

    if (stmt.next()!=0) break;

    strcat(m_pkcols,stcolumns.colname); strcat(m_pkcols,",");

    m_vpkcols.push_back(stcolumns);

    m_pkcount++;
  }

  if (m_pkcount>0) m_pkcols[strlen(m_pkcols)-1]=0;  // ɾ��m_pkcols���һ������Ķ��š�
 
  return true;
}
  
