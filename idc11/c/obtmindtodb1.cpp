/*
 * obtmindtodb.cpp�����������ڰ�ȫ��վ����ӹ۲�������⵽T_ZHOBTCODE���У�֧��xml��csv�����ļ���ʽ��
 * ���ߣ��¹ڡ�
 */
#include "_public.h"
#include "_mysql.h"
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
 /*   //���ӵ����ݿ⡣
    if(conn.connecttodb(argv[2],argv[3])!=0){
    logfile.Write("connect database(%s) failed.\n%s\n",argv[2],conn.m_cda.message);
 return -1;
}
 logfile.Write("connect database(%s) ok.\n",argv[2]);
 // �ύ����
 conn.commit();
*/
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
while(true){

//��ȡĿ¼���õ�һ�������ļ�����
if (Dir.ReadDir()==false) break;
 logfile.Write("filename=%s\n",Dir.m_FullFileName);
//���ļ���

/* while(true){

//�����ļ��е�ÿһ�С�

} */

//ɾ���ļ����ύ����
}

return true;
}
