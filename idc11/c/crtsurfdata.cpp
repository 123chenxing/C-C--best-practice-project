//����������ʷ�����ļ��Ĺ��ܣ�Ϊѹ���ļ��������ļ�ģ��׼����ʷ�����ļ�
//�����źŴ�����������2��15���ź�
//�������exit�����˳�ʱ�ֲ�����û�е�����������������
//��������Ϣд�빲���ڴ�
#include "_public.h"
CPActive PActive;   //��������
// ȫ������վ������ṹ�塣
struct st_stcode
{
  char provname[31]; // ʡ
  char obtid[11];    // վ��
  char obtname[31];  // վ��
  double lat;        // γ��
  double lon;        // ����
  double height;     // ���θ߶�
};

vector<struct st_stcode> vstcode; // ���ȫ������վ�������������

// ��վ������ļ��м��ص�vstcode�����С�
bool LoadSTCode(const char *inifile);

// ȫ������վ����ӹ۲����ݽṹ
struct st_surfdata
{
  char obtid[11];      // վ����롣
  char ddatetime[21];  // ����ʱ�䣺��ʽyyyymmddhh24miss
  int  t;              // ���£���λ��0.1���϶ȡ�
  int  p;              // ��ѹ��0.1������
  int  u;              // ���ʪ�ȣ�0-100֮���ֵ��
  int  wd;             // ����0-360֮���ֵ��
  int  wf;             // ���٣���λ0.1m/s
  int  r;              // ��������0.1mm��
  int  vis;            // �ܼ��ȣ�0.1�ס�
};

vector<struct st_surfdata> vsurfdata;  // ���ȫ������վ����ӹ۲����ݵ�����

char strddatetime[21]; // �۲����ݵ�ʱ�䡣

// ģ������ȫ������վ����ӹ۲����ݣ������vsurfdata�����С�
void CrtSurfData();
CFile File;
// ������vsurfdata�е�ȫ������վ����ӹ۲�����д���ļ���
bool CrtSurfFile(const char *outpath,const char *datafmt);

CLogFile logfile;    // ��־�ࡣ

void EXIT(int sig);   //�����˳����ź�2��15�Ĵ�����

int main(int argc,char *argv[])
{
  if ((argc!=5) && (argc!=6))
  {
    // ��������Ƿ������������ĵ���
    printf("Using:./crtsurfdata inifile outpath logfile datafmt [datetime]\n");
    printf("Example:/project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv\n");
    printf("Example:/project/idc11/bin/crtsurfdata /project/idc11/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv 202209191907\n\n");

    printf("inifile ȫ������վ������ļ�����\n");
    printf("outpath ȫ������վ�������ļ���ŵ�Ŀ¼��\n");
    printf("logfile ���������е���־�ļ�����\n");
    printf("datafmt ���������ļ��ĸ�ʽ��֧��xml��json��csv���ָ�ʽ���м��ö��ŷָ���\n\n");
	printf("datetime ����һ����ѡ��������ʾ����ָ��ʱ������ݺ��ļ���\n\n\n");
    return -1;
  }
//�ر�ȫ�����źź��������
//�����źţ���shell״̬�¿���"kill+���̺�"������ֹЩ����
//���벻Ҫ��"kill -9 +���̺�"ǿ����ֹ
CloseIOAndSignal(true);   signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
  // �򿪳������־�ļ���
  if (logfile.Open(argv[3],"a+",false)==false)
  {
     printf("logfile.Open(%s) failed.\n",argv[3]); return -1;
  }

  logfile.Write("crtsurfdata ��ʼ���С�\n");

PActive.AddPInfo(20,"crtsurfdata");

  // ��վ������ļ��м��ص�vstcode�����С� 
  if (LoadSTCode(argv[1])==false) return -1;
  // ��ȡ��ǰʱ�䣬�����۲�ʱ�䡣
  memset(strddatetime,0,sizeof(strddatetime));
	if(argc==5)
  LocalTime(strddatetime,"yyyymmddhh24miss");
else
	STRCPY(strddatetime,sizeof(strddatetime),argv[5]);
  // ģ������ȫ������վ����ӹ۲����ݣ������vsurfdata�����С�
  CrtSurfData();

  // ������vsurfdata�е�ȫ������վ����ӹ۲�����д���ļ���
  if (strstr(argv[4],"xml")!=0) CrtSurfFile(argv[2],"xml");
  if (strstr(argv[4],"json")!=0) CrtSurfFile(argv[2],"json");
  if (strstr(argv[4],"csv")!=0) CrtSurfFile(argv[2],"csv");

  logfile.Write("crtsurfdata ���н�����\n");

  return 0;
}

// ��վ������ļ��м��ص�vstcode�����С� 
bool LoadSTCode(const char *inifile)
{
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
    CmdStr.GetValue(2, stcode.obtname,30);  // վ��
    CmdStr.GetValue(3,&stcode.lat);         // γ��
    CmdStr.GetValue(4,&stcode.lon);         // ����
    CmdStr.GetValue(5,&stcode.height);      // ���θ߶�

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

// ģ������ȫ������վ����ӹ۲����ݣ������vsurfdata�����С�
void CrtSurfData()
{
  // ����������ӡ�
  srand(time(0));
  struct st_surfdata stsurfdata;

  // ��������վ�������vstcode������
  for (int ii=0;ii<vstcode.size();ii++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));

    // ������������ӹ۲����ݵĽṹ�塣
    strncpy(stsurfdata.obtid,vstcode[ii].obtid,10); // վ����롣
    strncpy(stsurfdata.ddatetime,strddatetime,14);  // ����ʱ�䣺��ʽyyyymmddhh24miss
    stsurfdata.t=rand()%351;       // ���£���λ��0.1���϶�
    stsurfdata.p=rand()%265+10000; // ��ѹ��0.1����
    stsurfdata.u=rand()%100+1;     // ���ʪ�ȣ�0-100֮���ֵ��
    stsurfdata.wd=rand()%360;      // ����0-360֮���ֵ��
    stsurfdata.wf=rand()%150;      // ���٣���λ0.1m/s
    stsurfdata.r=rand()%16;        // ��������0.1mm
    stsurfdata.vis=rand()%5001+100000;  // �ܼ��ȣ�0.1��

    // �ѹ۲����ݵĽṹ�����vsurfdata������
    vsurfdata.push_back(stsurfdata);
  }
}

// ������vsurfdata�е�ȫ������վ����ӹ۲�����д���ļ���
bool CrtSurfFile(const char *outpath,const char *datafmt)
{

  // ƴ���������ݵ��ļ��������磺/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strddatetime,getpid(),datafmt);

  // ���ļ���
  if (File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.OpenForRename(%s) failed.\n",strFileName); return false;
  }

  if (strcmp(datafmt,"csv")==0) File.Fprintf("վ�����,����ʱ��,����,��ѹ,���ʪ��,����,����,������,�ܼ���\n");
  if (strcmp(datafmt,"xml")==0) File.Fprintf("<data>\n");
  if (strcmp(datafmt,"json")==0) File.Fprintf("{\"data\":[\n");

  // ������Ź۲����ݵ�vsurfdata������
  for (int ii=0;ii<vsurfdata.size();ii++)
  {
    // д��һ����¼��
    if (strcmp(datafmt,"csv")==0)
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);

    if (strcmp(datafmt,"xml")==0)
      File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p>"\
                   "<u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);

    if (strcmp(datafmt,"json")==0)
{
      File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\","\
                   "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    if (ii<vsurfdata.size()-1) File.Fprintf(",\n");
    else   File.Fprintf("\n");
    // ���ϼ��д�����bug��Ӧ���������£�
    /*
    if (strcmp(datafmt,"json")==0)
    
      File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\","\
                   "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
      if (ii<vsurfdata.size()-1) File.Fprintf(",\n");
      else   File.Fprintf("\n");

    }
    */
}
  }

  if (strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");
  if (strcmp(datafmt,"json")==0) File.Fprintf("]}\n");
  // �ر��ļ���
  File.CloseAndRename();
	UTime(strFileName,strddatetime);    //�޸��ļ���ʱ������
  logfile.Write("���������ļ�%s�ɹ�������ʱ��%s����¼��%d��\n",strFileName,strddatetime,vsurfdata.size());

  return true;
}
//�����˳����ź�2��15�Ĵ�����
void EXIT(int sig){
logfile.Write("�����˳���sig=%d\n\n",sig);
exit(0);
}
