

#include "_public.h"

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

// ���ȫ������վ�������������
vector<struct st_stcode> vstcode;

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

// ģ������ȫ������վ����ӹ۲����ݣ������vsurfdata�����С�
void CrtSurfData();

CLogFile logfile;    // ��־�ࡣ

int main(int argc,char *argv[])
{
  if (argc!=4) 
  {
    // ��������Ƿ������������ĵ���
    printf("Using:./crtsurfdata3 inifile outpath logfile\n");
    printf("Example:/project/idc11/bin/crtsurfdata3 /project/idc11/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata3.log\n\n");

    printf("inifile ȫ������վ������ļ�����\n");
    printf("outpath ȫ������վ�������ļ���ŵ�Ŀ¼��\n");
    printf("logfile ���������е���־�ļ�����\n\n");

    return -1;
  }

  // �򿪳������־�ļ���
  if (logfile.Open(argv[3],"a+",false)==false)
  {
    printf("logfile.Open(%s) failed.\n",argv[3]); return -1;
  }

  logfile.Write("crtsurfdata3 ��ʼ���С�\n");

  // ��վ������ļ��м��ص�vstcode�����С� 
  if (LoadSTCode(argv[1])==false) return -1;

  // ģ������ȫ������վ����ӹ۲����ݣ������vsurfdata�����С�
  CrtSurfData();

  logfile.WriteEx("crtsurfdata3 ���н�����\n");

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

  // ��ȡ��ǰʱ�䣬�����۲�ʱ�䡣
  char strddatetime[21];
  memset(strddatetime,0,sizeof(strddatetime));
  LocalTime(strddatetime,"yyyymmddhh24miss");

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
