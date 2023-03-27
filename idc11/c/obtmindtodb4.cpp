/*
 * obtmindtodb.cpp�����������ڰ�ȫ��վ����ӹ۲�������⵽T_ZHOBTCODE���У�֧��xml��csv�����ļ���ʽ��
 * ���ߣ��¹ڡ�
 */
#include "_public.h"
#include "_mysql.h"

struct st_zhobtmind{
 char obtid[11];          //վ�����
 char ddatetime[21];      //����ʱ�䣬��ȷ�����ӡ�
 char t[11];              //�¶ȣ���λ��0.1���϶ȡ�
 char p[11];		  //��ѹ����λ��0.1������
 char u[11];		  //���ʪ�ȣ�0-100֮���ֵ��
 char wd[11];		  //����0-360֮���ֵ��
 char wf[11];		  //���٣���λ0.1m/s��
 char r[11];		  //��������0.1mm��
 char vis[11];		  //�ܼ��ȣ�0.1�ס�
};

// ȫ��վ����ӹ۲����ݲ����ࡣ
class CZHOBTMIND{
public:
  connection *m_conn;	               // ���ݿ����ӡ�
  CLogFile *m_logfile;                  //��־��

  sqlstatement m_stmt;                 // ����������sql��

  char m_buffer[1024];                 // ���ļ��ж�ȡ����һ�С�
  struct st_zhobtmind m_zhobtmind;     // ȫ��վ����ӹ۲����ݽṹ��
  CZHOBTMIND();
  CZHOBTMIND(connection *conn,CLogFile *logfile);
 
  ~CZHOBTMIND();
   void bindConnLog(connection *conn,CLogFile *logfile);  // ��connection��CLogFile�Ĵ���ȥ��
   bool SplitBuffer(char *strBuffer);  // �Ѵ��ļ�������һ�����ݲ�ֵ�m_zhobtmind�ṹ���С�
   bool InsertTable();               // ��m_zhobtmind�ṹ���е����ݲ��뵽T_ZHOBTMIND���С�
};
CZHOBTMIND::CZHOBTMIND(){
m_conn=0; m_logfile=0;
}
CZHOBTMIND::CZHOBTMIND(connection *conn,CLogFile *logfile){
 m_conn=conn;
 m_logfile=logfile;
}
CZHOBTMIND::~CZHOBTMIND(){
}
void CZHOBTMIND::bindConnLog(connection *conn,CLogFile *logfile){
 m_conn=conn;
 m_logfile=logfile;
}

// �Ѵ��ļ�������һ�����ݲ�ֵ�m_zhobtmind�ṹ���С�
bool CZHOBTMIND::SplitBuffer(char *strBuffer){

memset(&m_zhobtmind,0,sizeof(struct st_zhobtmind));
GetXMLBuffer(strBuffer,"obtid",m_zhobtmind.obtid,10);
GetXMLBuffer(strBuffer,"ddatetime",m_zhobtmind.ddatetime,14);
char tmp[11];
GetXMLBuffer(strBuffer,"t",tmp,10); if(strlen(tmp)>0) snprintf(m_zhobtmind.t,10,"%d",(int)(atof(tmp)*10));
GetXMLBuffer(strBuffer,"p",tmp,10); if(strlen(tmp)>0) snprintf(m_zhobtmind.p,10,"%d",(int)(atof(tmp)*10));
GetXMLBuffer(strBuffer,"u",m_zhobtmind.u,10);
GetXMLBuffer(strBuffer,"wd",m_zhobtmind.wd,10);
GetXMLBuffer(strBuffer,"wf",tmp,10); if(strlen(tmp)>0) snprintf(m_zhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
GetXMLBuffer(strBuffer,"r",tmp,10);if(strlen(tmp)>0) snprintf(m_zhobtmind.r,10,"%d",(int)(atof(tmp)*10));
GetXMLBuffer(strBuffer,"vis",tmp,10);if(strlen(tmp)>0) snprintf(m_zhobtmind.vis,10,"%d",(int)(atof(tmp)*10));
STRCPY(m_buffer,sizeof(m_buffer),strBuffer);
return true;
}

// ��m_zhobtmind�ṹ���е����ݲ��뵽T_ZHOBTMIND���С�
bool CZHOBTMIND::InsertTable(){
if (m_stmt.m_state==0){
  m_stmt.connect(m_conn);
 m_stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis) values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9)");
 m_stmt.bindin(1,m_zhobtmind.obtid,10);
 m_stmt.bindin(2,m_zhobtmind.ddatetime,14);
 m_stmt.bindin(3,m_zhobtmind.t,10);
 m_stmt.bindin(4,m_zhobtmind.p,10);
 m_stmt.bindin(5,m_zhobtmind.u,10);
 m_stmt.bindin(6,m_zhobtmind.wd,10);
 m_stmt.bindin(7,m_zhobtmind.wf,10);
 m_stmt.bindin(8,m_zhobtmind.r,10);
 m_stmt.bindin(9,m_zhobtmind.vis,10);
}
//�ѽṹ���е����ݲ�����С�
if (m_stmt.execute()!=0){
// 1��ʧ�ܵ��������Щ���Ƿ�ȫ����ʧ�ܶ�Ҫд��־��
// ��ʧ�ܵ�ԭ����Ҫ�ж���һ�Ǽ�¼�ظ��������������ݷǷ���
// 2�����ʧ������ô�죿 �����Ƿ���Ҫ�������Ƿ�rollback���Ƿ񷵻�false?
// �����ʧ�ܵ�ԭ�����������ݷǷ�����¼��־�����������Ǽ�¼�ظ������ؼ�¼��־���Ҽ�����
 if(m_stmt.m_cda.rc!=1062){
 m_logfile->Write("Buffer=%s\n",m_buffer);
 m_logfile->Write("m_stmt.execute() failed.\n%s\n%s\n",m_stmt.m_sql,m_stmt.m_cda.message);

 }
 return false;
 }

 return true;
}
CLogFile logfile;
connection conn;
CPActive PActive;
void EXIT(int sig);

//ҵ������������
bool _obtmindtodb(char *pathname,char *connstr,char *charset);
int main(int argc,char *argv[]){
 //�����ĵ���
 if(argc!=5){
 printf("\n");
 printf("Using:./obtmindtodb pathname connstr charset logfile\n");
 printf("Example:/project/tools11/bin/procctl 10 /project/idc11/bin/obtmindtodb /idcdata/surfdata \"127.0.0.1,root,13251009668,test2,3306\" utf8 /log/idc/obtmindtodb.log\n\n ");
 printf("���������ڰ�ȫ��վ����ӹ۲����ݱ��浽���ݿ��T_ZHOBTMIND���У�����ֻ���룬�����¡�\n");
 printf("pathname ȫ��վ����ӹ۲������ļ���ŵ�Ŀ¼��\n");
 printf("connstr ���ݿ����Ӳ�����ip,username,password,dbname,port\n");
 printf("charset ���ݿ���ַ�����\n");
 printf("logfile ���������е���־�ļ�����\n");
 printf("����ÿ10������һ�Σ���procctl���ȡ�\n\n\n");
 return -1;
}
 // �ر�ȫ�����źź����������
 // �����źţ���shell״̬�¿��� "kill + ���̺�" ������ֹЩ���̡�
 // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ��
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);


 // ����־�ļ���
 if (logfile.Open(argv[4],"a+")==false){
   printf("����־�ļ�ʧ�ܣ�%s����\n",argv[4]);   return -1;
}
   //PActive.AddPInfo(30,"obtmindtodb");        //���̵�������30���㹻��
   // ע�⣬�ڵ��Գ����ʱ�򣬿��������������µĴ��룬��ֹ��ʱ��
    PActive.AddPInfo(5000,"obtmindtodb");
    //ҵ������������
    _obtmindtodb(argv[1],argv[2],argv[3]);
    return 0;
}

void EXIT(int sig){
 logfile.Write("�����˳���sig=%d\n\n",sig);
 conn.disconnect();
 exit(0);
}

//ҵ������������
bool _obtmindtodb(char *pathname,char *connstr,char *charset){
 CDir Dir;

//��Ŀ¼
if(Dir.OpenDir(pathname,"*.xml")==false){
 logfile.Write("Dir.OpenDir(%s) failed.\n",pathname); return false;
}

CFile File;
CZHOBTMIND ZHOBTMIND(&conn,&logfile);
int totalcount=0;       // �ļ����ܼ�¼����
int insertcount=0;	// �ɹ������¼����
CTimer Timer;		// ��ʱ������¼ÿ�������ļ��Ĵ����ʱ��
while(true){

 //��ȡĿ¼���õ�һ�������ļ�����
 if (Dir.ReadDir()==false) break;
 //���ӵ����ݿ⡣
 if(conn.m_state==0){
 if(conn.connecttodb(connstr,charset)!=0){
 logfile.Write("connect database(%s) failed.\n%s\n",connstr,conn.m_cda.message);
 return -1;
 }
 logfile.Write("connect database(%s) ok.\n",connstr);
}
 //���ļ���
 if(File.Open(Dir.m_FullFileName,"r")==false){
 logfile.Write("File.Open(%s) failed.\n",Dir.m_FullFileName); return false;
}

 char strBuffer[1001];   //��Ŵ��ļ���ȡ��һ�С�
 while(true){

 if(File.FFGETS(strBuffer,1000,"<endl/>")==false) break;
 //�����ļ��е�ÿһ�С�
 totalcount++;
 ZHOBTMIND.SplitBuffer(strBuffer);
 if(ZHOBTMIND.InsertTable()==true)  insertcount++;
}
//ɾ���ļ����ύ����
//File.CloseAndRemove();
 conn.commit();
 logfile.Write("�Ѵ����ļ�%s (totalcount=%d,insertcount=%d)����ʱ%.2f�롣\n",Dir.m_FullFileName,totalcount,insertcount,Timer.Elapsed());
}

return true;
}
