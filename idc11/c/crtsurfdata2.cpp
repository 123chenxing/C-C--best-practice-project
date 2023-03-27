

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

CLogFile logfile;    // ��־�ࡣ

int main(int argc,char *argv[])
{
  if (argc!=4) 
  {
    // ��������Ƿ������������ĵ���
    printf("Using:./crtsurfdata2 inifile outpath logfile\n");
    printf("Example:/project/idc11/bin/crtsurfdata2 /project/idc11/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata2.log\n\n");

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

  logfile.Write("crtsurfdata2 ��ʼ���С�\n");

  // ��վ������ļ��м��ص�vstcode�����С� 
  if (LoadSTCode(argv[1])==false) return -1;


  logfile.WriteEx("crtsurfdata2 ���н�����\n");

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

