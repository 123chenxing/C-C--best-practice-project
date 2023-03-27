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
}stzhobtmind;
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
 sqlstatement stmt;
 CDir Dir;

//��Ŀ¼
if(Dir.OpenDir(pathname,"*.xml")==false){
 logfile.Write("Dir.OpenDir(%s) failed.\n",pathname); return false;
}

CFile File;
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
  if (stmt.m_state==0){
  stmt.connect(&conn);
 stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis) values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9)");
 stmt.bindin(1,stzhobtmind.obtid,10);
 stmt.bindin(2,stzhobtmind.ddatetime,14);
 stmt.bindin(3,stzhobtmind.t,10);
 stmt.bindin(4,stzhobtmind.p,10);
 stmt.bindin(5,stzhobtmind.u,10);
 stmt.bindin(6,stzhobtmind.wd,10);
 stmt.bindin(7,stzhobtmind.wf,10);
 stmt.bindin(8,stzhobtmind.r,10);
 stmt.bindin(9,stzhobtmind.vis,10);
}
 logfile.Write("filename=%s\n",Dir.m_FullFileName);
 //���ļ���
 if(File.Open(Dir.m_FullFileName,"r")==false){
 logfile.Write("File.Open(%s) failed.\n",Dir.m_FullFileName); return false;
}

 char strBuffer[1001];   //��Ŵ��ļ���ȡ��һ�С�
 while(true){

 if(File.FFGETS(strBuffer,1000,"<endl/>")==false) break;
 logfile.Write("strBuffer=%s",strBuffer);

 //�����ļ��е�ÿһ�С�
 memset(&stzhobtmind,0,sizeof(struct st_zhobtmind));
 GetXMLBuffer(strBuffer,"obtid",stzhobtmind.obtid,10);
 GetXMLBuffer(strBuffer,"ddatetime",stzhobtmind.ddatetime,14);
 char tmp[11];
 GetXMLBuffer(strBuffer,"t",stzhobtmind.t,10); if(strlen(tmp)>0) snprintf(stzhobtmind.t,10,"%d",(int)(atof(tmp)*10));
 GetXMLBuffer(strBuffer,"p",stzhobtmind.p,10); if(strlen(tmp)>0) snprintf(stzhobtmind.p,10,"%d",(int)(atof(tmp)*10));
 GetXMLBuffer(strBuffer,"u",stzhobtmind.u,10);
 GetXMLBuffer(strBuffer,"wd",stzhobtmind.wd,10);
 GetXMLBuffer(strBuffer,"wf",stzhobtmind.wf,10); if(strlen(tmp)>0) snprintf(stzhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
 GetXMLBuffer(strBuffer,"r",stzhobtmind.r,10);if(strlen(tmp)>0) snprintf(stzhobtmind.r,10,"%d",(int)(atof(tmp)*10));
 GetXMLBuffer(strBuffer,"vis",stzhobtmind.vis,10);if(strlen(tmp)>0) snprintf(stzhobtmind.vis,10,"%d",(int)(atof(tmp)*10));
 logfile.Write("obtid=%s,ddatetime=%s,t=%s,p=%s,u=%s,wd=%s,wf=%s,r=%s,vis=%s\n",stzhobtmind.obtid,stzhobtmind.ddatetime,stzhobtmind.t,stzhobtmind.p,stzhobtmind.u,stzhobtmind.wd,stzhobtmind.wf,stzhobtmind.r,stzhobtmind.vis);

//�ѽṹ���е����ݲ�����С�
 if (stmt.execute()!=0){
 // 1��ʧ�ܵ��������Щ���Ƿ�ȫ����ʧ�ܶ�Ҫд��־��
 // ��ʧ�ܵ�ԭ����Ҫ�ж���һ�Ǽ�¼�ظ��������������ݷǷ���
 // 2�����ʧ������ô�죿 �����Ƿ���Ҫ�������Ƿ�rollback���Ƿ񷵻�false?
 // �����ʧ�ܵ�ԭ�����������ݷǷ�����¼��־�����������Ǽ�¼�ظ������ؼ�¼��־���Ҽ�����
 if(stmt.m_cda.rc!=1062){
 logfile.Write("Buffer=%s\n",strBuffer);
 logfile.Write("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message);

}
}
}
//ɾ���ļ����ύ����
//File.CloseAndRemove();
 conn.commit();
}

return true;
}
