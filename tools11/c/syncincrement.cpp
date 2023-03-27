/*
 *  ��������syncincrement.cpp�����������������ĵĹ�������ģ�飬���������ķ���ͬ��MySQL���ݿ�֮��ı�
 *  ���ߣ��¹ڡ�
*/
#include "_tools.h"

struct st_arg
{
  char localconnstr[101];  // �������ݿ�����Ӳ�����
  char charset[51];        // ���ݿ���ַ�����
  char fedtname[31];       // Federated������
  char localtname[31];     // ���ر�����
  char remotecols[1001];   // Զ�̱���ֶ��б�
  char localcols[1001];    // ���ر���ֶ��б�
  char where[1001];        // ͬ�����ݵ�������
  char remoteconnstr[101]; // Զ�����ݿ�����Ӳ�����
  char remotetname[31];    // Զ�̱�����
  char remotekeycol[31];   // Զ�̱�������ֶ�����
  char localkeycol[31];    // ���ر�������ֶ�����
  int  maxcount;           // ÿ��ִ��һ��ͬ�������ļ�¼����
  int timetvl;             // ͬ��ʱ��������λ���룬ȡֵ1-30��
  int  timeout;            // ����������ʱ�ĳ�ʱʱ�䡣
  char pname[51];          // ����������ʱ�ĳ�������
} starg;

// ��ʾ����İ���
void _help(char *argv[]);

// ��xml����������starg�ṹ��
bool _xmltoarg(char *strxmlbuffer);

CLogFile logfile;

connection connloc;   // �������ݿ����ӡ�
connection connrem;   // Զ�����ݿ����ӡ�

// ҵ������������
bool _syncincrement(bool &bcontinue);

// �ӱ��ر�starg.localtname��ȡ�����ֶε����ֵ�������maxkeyvalueȫ�ֱ�����
long maxkeyvalue=0;
bool findmaxkey();

void EXIT(int sig);

CPActive PActive;

int main(int argc,char *argv[])
{
  if (argc!=3) { _help(argv); return -1; }

  // �ر�ȫ�����źź������������������˳����źš�
  CloseIOAndSignal();  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
  
  if (logfile.Open(argv[1],"a+")==false)
  {
    printf("����־�ļ�ʧ�ܣ�%s����\n",argv[1]); return -1;
  }

  // ��xml����������starg�ṹ��
  if (_xmltoarg(argv[2])==false) return -1;

  PActive.AddPInfo(starg.timeout,starg.pname);
  // ע�⣬�ڵ��Գ����ʱ�򣬿��������������µĴ��룬��ֹ��ʱ��
  // PActive.AddPInfo(starg.timeout*100,starg.pname);

  if (connloc.connecttodb(starg.localconnstr,starg.charset) != 0)
  {
    logfile.Write("connect database(%s) failed.\n%s\n",starg.localconnstr,connloc.m_cda.message); EXIT(-1);
  }

  // logfile.Write("connect database(%s) ok.\n",starg.localconnstr);

  if (connrem.connecttodb(starg.remoteconnstr,starg.charset)!=0)
  {
    logfile.Write("connect database(%s) failed.\n%s\n",starg.remoteconnstr,connrem.m_cda.message);
    return false;    
  }

  // logfile.Write("connect database(%s) ok.\n",starg.remoteconnstr);

  // ���starg.remotecols��starg.localcolsΪ�գ�����starg.localtname���ȫ��������䡣
  if ( (strlen(starg.remotecols)==0) || (strlen(starg.localcols)==0) )
  {
    CTABCOLS TABCOLS;

    // ��ȡstarg.localtname���ȫ���С�
    if (TABCOLS.allcols(&connloc,starg.localtname)==false)
    {
      logfile.Write("��%s�����ڡ�\n",starg.localtname); EXIT(-1); 
    }

    if (strlen(starg.remotecols)==0)  strcpy(starg.remotecols,TABCOLS.m_allcols);
    if (strlen(starg.localcols)==0)   strcpy(starg.localcols,TABCOLS.m_allcols);
  }

  bool bcontinue;

  // ҵ������������
  while (true)
  {
    if (_syncincrement(bcontinue)==false) EXIT(-1);

    if (bcontinue==false) sleep(starg.timetvl);

    PActive.UptATime();
  }
}

// ��ʾ����İ���
void _help(char *argv[])
{
  printf("Using:/project/tools11/bin/syncincrement logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools11/bin/procctl 10 /project/tools11/bin/syncincrement /log/idc/syncincrement_ZHOBTMIND2.log \"<localconnstr>192.168.244.128,root,13251009668,test2,3306</localconnstr><remoteconnstr>192.168.244.128,root,13251009668,test2,3306</remoteconnstr><charset>utf8</charset><remotetname>T_ZHOBTMIND1</remotetname><fedtname>LK_ZHOBTMIND1</fedtname><localtname>T_ZHOBTMIND2</localtname><remotecols>obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</remotecols><localcols>stid,ddatetime,t,p,u,wd,wf,r,vis,upttime,recid</localcols><remotekeycol>keyid</remotekeycol><localkeycol>recid</localkeycol><maxcount>300</maxcount><timetvl>2</timetvl><timeout>50</timeout><pname>syncincrement_ZHOBTMIND2</pname>\"\n\n");
  
  printf("       /project/tools11/bin/procctl 10 /project/tools11/bin/syncincrement /log/idc/syncincrement_ZHOBTMIND3.log \"<localconnstr>192.168.244.128,root,13251009668,test2,3306</localconnstr><remoteconnstr>192.168.244.128,root,13251009668,test2,3306</remoteconnstr><charset>utf8</charset><remotetname>T_ZHOBTMIND1</remotetname><fedtname>LK_ZHOBTMIND1</fedtname><localtname>T_ZHOBTMIND3</localtname><remotecols>obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</remotecols><localcols>stid,ddatetime,t,p,u,wd,wf,r,vis,upttime,recid</localcols><where>and obtid like '54%%%%'</where><remotekeycol>keyid</remotekeycol><localkeycol>recid</localkeycol><maxcount>300</maxcount><timetvl>2</timetvl><timeout>50</timeout><pname>syncincrement_ZHOBTMIND3</pname>\"\n\n");
 

  printf("���������������ĵĹ�������ģ�飬���������ķ���ͬ��MySQL���ݿ�֮��ı�\n\n");

  printf("logfilename   ���������е���־�ļ���\n");
  printf("xmlbuffer     ���������еĲ�������xml��ʾ���������£�\n\n");

  printf("localconnstr  �������ݿ�����Ӳ�������ʽ��ip,username,password,dbname,port��\n");
  printf("charset       ���ݿ���ַ������������Ҫ��Զ�����ݿⱣ��һ�£�����������������������\n");

  printf("fedtname      Federated������\n");
  printf("localtname    ���ر�����\n");

  printf("remotecols    Զ�̱���ֶ��б����������select��from֮�䣬���ԣ�remotecols��������ʵ���ֶΣ�\n"\
         "              Ҳ�����Ǻ����ķ���ֵ���������������������Ϊ�գ�����localtname����ֶ��б���䡣\n");
  printf("localcols     ���ر���ֶ��б���remotecols��ͬ������������ʵ���ڵ��ֶΡ����������Ϊ�գ�\n"\
         "              ����localtname����ֶ��б���䡣\n");

  printf("where		ͬ�����ݵ������������select remotekeycol from remotetname where remotekeycol>:1֮��ע�⣬��Ҫ��where�ؼ��֡�\n");
 
  printf("remoteconnstr Զ�����ݿ�����Ӳ�������ʽ��localconnstr��ͬ��\n");
  printf("remotetname   Զ�̱�����\n");
  printf("remotekeycol  Զ�̱�������ֶ�����\n");
  printf("localkeycol   ���ر�������ֶ�����\n");

  printf("maxcount      ÿ��ִ��һ��ͬ�������ļ�¼�������ܳ���MAXPARAMS�꣨��_mysql.h�ж��壩����synctype==2ʱ��Ч��\n");

  printf("timetvl	ִ��ͬ����ʱ��������λ���룬ȡֵ1-30��\n");
  printf("timeout       ������ĳ�ʱʱ�䣬��λ���룬���������Ĵ�С��������������30���ϡ�\n");
  printf("pname         ����������ʱ�Ľ������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣\n\n");
} 

// ��xml����������starg�ṹ��
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  // �������ݿ�����Ӳ�������ʽ��ip,username,password,dbname,port��
  GetXMLBuffer(strxmlbuffer,"localconnstr",starg.localconnstr,100);
  if (strlen(starg.localconnstr)==0) { logfile.Write("localconnstr is null.\n"); return false; }

  // ���ݿ���ַ������������Ҫ��Զ�����ݿⱣ��һ�£�����������������������
  GetXMLBuffer(strxmlbuffer,"charset",starg.charset,50);
  if (strlen(starg.charset)==0) { logfile.Write("charset is null.\n"); return false; }

  // Federated������
  GetXMLBuffer(strxmlbuffer,"fedtname",starg.fedtname,30);
  if (strlen(starg.fedtname)==0) { logfile.Write("fedtname is null.\n"); return false; }

  // ���ر�����
  GetXMLBuffer(strxmlbuffer,"localtname",starg.localtname,30);
  if (strlen(starg.localtname)==0) { logfile.Write("localtname is null.\n"); return false; }

  // Զ�̱���ֶ��б����������select��from֮�䣬���ԣ�remotecols��������ʵ���ֶΣ�Ҳ�����Ǻ���
  // �ķ���ֵ���������������������Ϊ�գ�����localtname����ֶ��б���䡣\n");
  GetXMLBuffer(strxmlbuffer,"remotecols",starg.remotecols,1000);

  // ���ر���ֶ��б���remotecols��ͬ������������ʵ���ڵ��ֶΡ����������Ϊ�գ�����localtname����ֶ��б���䡣
  GetXMLBuffer(strxmlbuffer,"localcols",starg.localcols,1000);

  // ͬ�����ݵ���������select����where���֡�
  GetXMLBuffer(strxmlbuffer,"where",starg.where,1000);

  // Զ�����ݿ�����Ӳ�������ʽ��localconnstr��ͬ����synctype==2ʱ��Ч��
  GetXMLBuffer(strxmlbuffer,"remoteconnstr",starg.remoteconnstr,100);
  if (strlen(starg.remoteconnstr)==0) { logfile.Write("remoteconnstr is null.\n"); return false; }

  // Զ�̱�������synctype==2ʱ��Ч��
  GetXMLBuffer(strxmlbuffer,"remotetname",starg.remotetname,30);
  if (strlen(starg.remotetname)==0) { logfile.Write("remotetname is null.\n"); return false; }

  // Զ�̱�������ֶ�����
  GetXMLBuffer(strxmlbuffer,"remotekeycol",starg.remotekeycol,30);
  if (strlen(starg.remotekeycol)==0) { logfile.Write("remotekeycol is null.\n"); return false; }

  // ���ر�������ֶ�����
  GetXMLBuffer(strxmlbuffer,"localkeycol",starg.localkeycol,30);
  if (strlen(starg.localkeycol)==0) { logfile.Write("localkeycol is null.\n"); return false; }

  // ÿ��ִ��һ��ͬ�������ļ�¼�������ܳ���MAXPARAMS�꣨��_mysql.h�ж��壩����synctype==2ʱ��Ч��
  GetXMLBuffer(strxmlbuffer,"maxcount",&starg.maxcount);
  if (starg.maxcount==0) { logfile.Write("maxcount is null.\n"); return false; }
  if (starg.maxcount>MAXPARAMS) starg.maxcount=MAXPARAMS;

  // ִ��ͬ����ʱ��������λ���룬ȡֵ1-30��
  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if (starg.timetvl<=0) {logfile.Write("timetvl is null.\n"); return false;}
  if (starg.timetvl>30) starg.timetvl=30;

  // ������ĳ�ʱʱ�䣬��λ���룬���������Ĵ�С��������������30���ϡ�
  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if (starg.timeout==0) { logfile.Write("timeout is null.\n"); return false; }

  // ���´���timetvl��timeout�ķ�����Ȼ�е����⣬��Ҳ���ⲻ�󣬲��ó���ʱ�Ϳ����ˡ�
  if (starg.timeout<starg.timetvl+10) starg.timeout=starg.timetvl+10;

  // ����������ʱ�Ľ������������ܲ����׶��ġ����������̲�ͬ�����ƣ���������Ų顣
  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);
  if (strlen(starg.pname)==0) { logfile.Write("pname is null.\n"); return false; }

  return true;
}

void EXIT(int sig)
{
  logfile.Write("�����˳���sig=%d\n\n",sig);

  connloc.disconnect();

  connrem.disconnect();

  exit(0);
}

/*
create table LK_ZHOBTCODE1
(
   obtid                varchar(10) not null comment 'վ�����',
   cityname             varchar(30) not null comment '��������',
   provname             varchar(30) not null comment 'ʡ����',
   lat                  int not null comment 'γ�ȣ���λ��0.01�ȡ�',
   lon                  int not null comment '���ȣ���λ��0.01�ȡ�',
   height               int not null comment '���θ߶ȣ���λ��0.1�ס�',
   upttime              timestamp not null comment '����ʱ�䡣',
   keyid                int not null auto_increment comment '��¼��ţ��Զ������С�',
   primary key (obtid),
   unique key ZHOBTCODE1_KEYID (keyid)
)ENGINE=FEDERATED CONNECTION='mysql://root:13251009668@192.168.244.128:3306/test2/T_ZHOBTCODE1';

create table LK_ZHOBTMIND1
(
   obtid                varchar(10) not null comment 'վ����롣',
   ddatetime            datetime not null comment '����ʱ�䣬��ȷ�����ӡ�',
   t                    int comment 'ʪ�ȣ���λ��0.1���϶ȡ�',
   p                    int comment '��ѹ����λ��0.1������',
   u                    int comment '���ʪ�ȣ�0-100֮���ֵ��',
   wd                   int comment '����0-360֮���ֵ��',
   wf                   int comment '���٣���λ0.1m/s��',
   r                    int comment '��������0.1mm��',
   vis                  int comment '�ܼ��ȣ�0.1�ס�',
   upttime              timestamp not null comment '����ʱ�䡣',
   keyid                bigint not null auto_increment comment '��¼��ţ��Զ������С�',
   primary key (obtid, ddatetime),
   unique key ZHOBTMIND1_KEYID (keyid)
)ENGINE=FEDERATED CONNECTION='mysql://root:13251009668@192.168.244.128:3306/test2/T_ZHOBTMIND1';
*/

//ҵ����������
bool _syncincrement(bool &bcontinue)
{
  CTimer Timer;

  bcontinue=false;
  // �ӱ��ر�starg.localtname��ȡ�����ֶε����ֵ�������maxkeyvalueȫ�ֱ����С�
  if (findmaxkey()==false) return false;
  
  // ��Զ�̱���������ֶε�ֵ����maxkeyvalue�ļ�¼��
  // ��Զ�̱���ҵ���Ҫͬ����¼��key�ֶε�ֵ��
  char remkeyvalue[51];     // ��Զ�̱�鵽����Ҫͬ����¼��key�ֶε�ֵ��
  sqlstatement stmtsel(&connrem);
  stmtsel.prepare("select %s from %s where %s>:1 %s order by %s",starg.remotekeycol,starg.remotetname,starg.remotekeycol,starg.where,starg.remotekeycol);
  stmtsel.bindin(1,&maxkeyvalue);
  stmtsel.bindout(1,remkeyvalue,50);

 
  // ƴ�Ӱ�ͬ��SQL���������ַ��� (:1,:2,:3,...,:starg.maxcount)��
  char bindstr[2001];                   // ��ͬ��SQL���������ַ�����
  char strtemp[11];

  memset(bindstr,0,sizeof(bindstr));

  for (int ii=0;ii<starg.maxcount;ii++)
  {
    memset(strtemp,0,sizeof(strtemp));
    sprintf(strtemp,":%lu,",ii+1);
    strcat(bindstr,strtemp);
  }
  bindstr[strlen(bindstr)-1]=0;         // ���һ�������Ƕ���ġ�

  char keyvalues[starg.maxcount][51];   // ���key�ֶε�ֵ��
   
  // ׼�����뱾�ر����ݵ�SQL��䣬һ�β���starg.maxcount����¼��
  // insert into T_ZHOBTMIND2(stid,ddatetime,t,p,u,wd,wf,r,vis,upttime,recid)
  //     select obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid from LK_ZHOBTMIND1
  //     where keyid in (:1,:2,:3);
  sqlstatement stmtins(&connloc);    // ִ���򱾵ر��в������ݵ�SQL��䡣

  stmtins.prepare("insert into %s(%s) select %s from %s where %s in (%s)",starg.localtname,starg.localcols,starg.remotecols,starg.fedtname,starg.remotekeycol,bindstr);
  for (int ii=0;ii<starg.maxcount;ii++)
  {
    stmtins.bindin(ii+1,keyvalues[ii],50);
  }

  int ccount=0;        // ��¼�ӽ�������ѻ�ȡ��¼�ļ�������

  memset(keyvalues,0,sizeof(keyvalues));

  if (stmtsel.execute()!=0)
  {
    logfile.Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message);   return false;
  }
  while(true)
  {
    // ��ȡ��Ҫͬ�����ݵĽ������
    if (stmtsel.next()!=0)  break;

    strcpy(keyvalues[ccount],remkeyvalue);

    ccount++;

    // ÿstarg.maxcount����¼ִ��һ��ͬ����
    if (ccount==starg.maxcount)
    {
      // �򱾵ر�����¼��
      if (stmtins.execute()!=0)
      {
        // ִ���򱾵ر��в����¼�Ĳ���һ�㲻�����
        // ��������Ϳ϶������ݿ�������ͬ���Ĳ������ò���ȷ�����̲��ؼ�����
        logfile.Write("stmtins exexute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);   return false;
      }

      logfile.Write("sync %s to %s(%d rows) in %.2fsec.\n",starg.fedtname,starg.localtname,ccount,Timer.Elapsed());

      connloc.commit();

      ccount=0;   // ��¼�ӽ�������ѻ�ȡ��¼�ļ�������

      memset(keyvalues,0,sizeof(keyvalues));

      PActive.UptATime();
    }
  }

  // ���ccount>0����ʾ����ûͬ���ļ�¼����ִ��һ��ͬ����
  if (ccount>0)
  {
    // �ӱ��ر�����¼��
    if (stmtins.execute()!=0)
    {
      logfile.Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);     return false;
    }

    logfile.Write("sync %s to %s(%d rows) in %.2fsec.\n",starg.fedtname,starg.localtname,ccount,Timer.Elapsed());

    connloc.commit();
  }
 
  if (stmtsel.m_cda.rpc>0)
  {
    //logfile.Write("sync %s to %s(%d rows) in %.2fsec.\n",starg.fedtname,starg.localtname,stmtsel.m_cda.rpc,Timer.Elapsed());
    bcontinue=true;
  } 

  return true;
}

// �ӱ��ر�starg.localtname��ȡ�����ֶε����ֵ�������maxkeyvalueȫ�ֱ����С�
bool findmaxkey()
{
  maxkeyvalue=0;

  sqlstatement stmt(&connloc);
  stmt.prepare("select max(%s) from %s",starg.localkeycol,starg.localtname);
  stmt.bindout(1,&maxkeyvalue);

  if (stmt.execute()!=0)
  {
    logfile.Write("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message);
    return false;
  }

  stmt.next();

  //logfile.Write("maxkeyvalue=%ld\n",maxkeyvalue);

  return true;

}
