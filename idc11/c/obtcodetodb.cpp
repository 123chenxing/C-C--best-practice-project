/*
 * obtcodetodb.cpp�����������ڰ�ȫ��վ��������ݱ��浽���ݿ�T_ZHOBTCODE���С�
 * ���ߣ��¹ڡ�
 */
#include "_public.h"
#include "_mysql.h"
// ȫ������վ������ṹ�塣
 struct st_stcode
 {
 char provname[31]; // ʡ
 char obtid[11];    // վ��
 char cityname[31];  // վ��
 char lat[11];        // γ��
 char lon[11];        // ����
 char height[11];     // ���θ߶�
      };

 vector<struct st_stcode> vstcode; // ���ȫ������վ�������������

// ��վ������ļ��м��ص�vstcode�����С�
 bool LoadSTCode(const char *inifile);
 CLogFile logfile;
 connection conn;
 CPActive PActive;
 void EXIT(int sig);
int main(int argc,char *argv[]){
 //�����ĵ���
 if(argc!=5){
 printf("\n");
 printf("Using:./obtcodetodb inifile connstr charset logfile\n");
 printf("Example:/project/tools11/bin/procctl 120 /project/idc11/bin/obtcodetodb /project/idc11/ini/stcode.ini \"127.0.0.1,root,13251009668,test2,3306\" utf8 /log/idc/obtcodetodb.log\n\n ");
 printf("���������ڰ�ȫ��վ��������ݱ��浽���ݿ���У����վ�㲻��������룬վ���Ѵ�������¡�\n");
 printf("inifile վ������ļ�����ȫ·������\n");
 printf("connstr ���ݿ����Ӳ�����ip,username,password,dbname,port\n");
 printf("charset ���ݿ���ַ�����\n");
 printf("logfile ���������е���־�ļ�����\n");
 printf("����ÿ120������һ�Σ���procctl���ȡ�\n\n\n");
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
   PActive.AddPInfo(10,"obtcodetodb");        //���̵�������10���㹻��
   // ע�⣬�ڵ��Գ����ʱ�򣬿��������������µĴ��룬��ֹ��ʱ��
   // PActive.AddPInfo(5000,"obtcodetodb");
   //��ȫ��վ������ļ����ص�vstcode�����С�
 if (LoadSTCode(argv[1])==false) return -1;
 logfile.Write("���ز����ļ���%s���ɹ���վ������%d����\n",argv[1],vstcode.size());
 //���ӵ����ݿ⡣
 if(conn.connecttodb(argv[2],argv[3])!=0){
 logfile.Write("connect database(%s) failed.\n%s\n",argv[2],conn.m_cda.message);
 return -1;
}
 logfile.Write("connect database(%s) ok.\n",argv[2]);
 struct st_stcode stcode;
 //׼��������SQL��䡣
 sqlstatement stmtins(&conn);
 stmtins.prepare("insert into T_ZHOBTCODE(obtid,cityname,provname,lat,lon,height,upttime) values(:1,:2,:3,:4,:5,:6,now())");
 stmtins.bindin(1,stcode.obtid,10);
 stmtins.bindin(2,stcode.cityname,30);
 stmtins.bindin(3,stcode.provname,30);
 stmtins.bindin(4,stcode.lat,10);
 stmtins.bindin(5,stcode.lon,10);
 stmtins.bindin(6,stcode.height,10);
 //׼�����±��SQL��䡣 
 sqlstatement stmtupt(&conn);
 stmtupt.prepare("update T_ZHOBTCODE set cityname=:1,provname=:2,lat=:3,lon=:4,height=:5,upttime=now() where obtid=:6");
 stmtupt.bindin(1,stcode.cityname,30);
 stmtupt.bindin(2,stcode.provname,30);
 stmtupt.bindin(3,stcode.lat,10);
 stmtupt.bindin(4,stcode.lon,10);
 stmtupt.bindin(5,stcode.height,10);
 stmtupt.bindin(6,stcode.obtid,10);
 // �����ϴ����ʱ��ҪС�ģ����������������Ը�����
 int inscount=0,uptcount=0;
 CTimer Timer;
 for(int ii=0;ii<vstcode.size();ii++){
  // ��������ȡ��һ����¼���ṹ��stcode�С�
 memcpy(&stcode,&vstcode[ii],sizeof(struct st_stcode));

 // ִ�в����SQL��䡣
 if(stmtins.execute()!=0){
 if(stmtins.m_cda.rc==1062){
  // �����¼�Ѵ��ڣ�ִ�и��µ�SQL��䡣
  if(stmtupt.execute()!=0){
  logfile.Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message); return -1;

}
  else uptcount++;
}
else{
   //�����д����ʱ��ҲҪС�ģ����������������Ը�����
   logfile.Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message); return -1;
}

} 
else inscount++;
}
 // ���ܼ�¼���������¼�������¼�¼��������ʱ����¼��־��
 logfile.Write("�ܼ�¼��=%d������=%d������=%d����ʱ=%.2f�롣\n",vstcode.size(),inscount,uptcount,Timer.Elapsed());
 // �ύ����
 conn.commit();
return 0;
}
 // ��վ������ļ��м��ص�vstcode�����С� 
 bool LoadSTCode(const char *inifile)
 {
 CFile File;
 // ��վ������ļ���
 if (File.Open(inifile,"r")==false)
  {
 logfile.Write("File.Open(%s) failed.\n",inifile); return false;
          }

 char strBuffer[301];

 CCmdStr CmdStr;

 struct st_stcode stcode;

 while (true)
       {
 // ��վ������ļ��ж�ȡһ�У�����Ѷ�ȡ�꣬����ѭ����
 if (File.Fgets(strBuffer,300,true)==false) break;

 // �Ѷ�ȡ����һ�в�֡�
 CmdStr.SplitToCmd(strBuffer,",",true);

 if (CmdStr.CmdCount()!=6) continue;     // �ӵ���Ч���С�
// ��վ�������ÿ��������浽վ������ṹ���С�
  memset(&stcode,0,sizeof(struct st_stcode));
  CmdStr.GetValue(0, stcode.provname,30); // ʡ
  CmdStr.GetValue(1, stcode.obtid,10);    // վ��
  CmdStr.GetValue(2, stcode.cityname,30);  // վ��
  CmdStr.GetValue(3,stcode.lat,10);         // γ��
  CmdStr.GetValue(4,stcode.lon,10);         // ����
  CmdStr.GetValue(5,stcode.height,10);      // ���θ߶�

  // ��վ������ṹ�����վ�����������
  vstcode.push_back(stcode);
                     }

/*
  for (int ii=0;ii<vstcode.size();ii++)
  logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
  vstcode[ii].provname,vstcode[ii].obtid,vstcode[ii].obtname,vstcode[ii].lat,\
  vstcode[ii].lon,vstcode[ii].height);
                                */

  return true;
     }

void EXIT(int sig){
 logfile.Write("�����˳���sig=%d\n\n",sig);
 conn.disconnect();
 exit(0);
}
