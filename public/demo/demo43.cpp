/*
 *  ��������demo43.cpp���˳�����ʾ������ܵ�CLogFile�����־�ļ����л���
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  CLogFile logfile;

  // ����־�ļ������"/tmp/log"�����ڣ��ʹ�����������Ҫȷ����ǰ�û��߱�����Ŀ¼��Ȩ�ޡ�
  if (logfile.Open("/tmp/log/demo43.log")==false)
  { printf("logfile.Open(/tmp/log/demo43.log) failed.\n"); return -1; }

  logfile.Write("demo43����ʼ���С�\n");

  // �ó���ѭ��10000000�������㹻�����־��
  for (int ii=0;ii<10000000;ii++)
  {
    logfile.Write("��������ʾ��־�ļ����л������ǵ�%010d����¼��\n",ii);
  }

  logfile.Write("demo43�������н�����\n");
}

