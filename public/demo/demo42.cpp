/*
 *  ��������demo42.cpp���˳�����ʾ���ÿ�����ܵ�CLogFile���¼�����������־��
 *  �������޸�demo40.cpp�������printf����Ϊд��־�ļ���
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  CLogFile logfile;

  // ����־�ļ������"/tmp/log"�����ڣ��ʹ�����������Ҫȷ����ǰ�û��߱�����Ŀ¼��Ȩ�ޡ�
  if (logfile.Open("/tmp/log/demo42.log")==false)
  { printf("logfile.Open(/tmp/log/demo42.log) failed.\n"); return -1; }

  logfile.Write("demo42����ʼ���С�\n");

  CDir Dir;

  // ɨ��/tmp/dataĿ¼���ļ���ƥ��"surfdata_*.xml"���ļ���
  if (Dir.OpenDir("/tmp/data","surfdata_*.xml")==false)
  { logfile.Write("Dir.OpenDir(/tmp/data) failed.\n"); return -1; }

  CFile File;

  while (Dir.ReadDir()==true)
  {
    logfile.Write("�����ļ�%s...",Dir.m_FullFileName);

    if (File.Open(Dir.m_FullFileName,"r")==false)
    { logfile.WriteEx("failed.File.Open(%s) failed.\n",Dir.m_FullFileName); return -1; }

    char strBuffer[301];

    while (true)
    {
      memset(strBuffer,0,sizeof(strBuffer));
      if (File.FFGETS(strBuffer,300,"<endl/>")==false) break; // ��������"<endl/>"������

      // logfile.Write("strBuffer=%s",strBuffer);

      // ������Բ������xml�ַ�����������д�����ݿ�Ĵ��롣
    }


    // �������ļ��е����ݺ󣬹ر��ļ�ָ�룬��ɾ���ļ���
    File.CloseAndRemove();

    logfile.WriteEx("ok\n");
  }

  logfile.Write("demo42�������н�����\n");
}

