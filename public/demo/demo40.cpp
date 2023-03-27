/*
 *  ��������demo40.cpp���˳�����ʾ��������в���CDir���CFile�ദ�������ļ����÷���
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  CDir Dir;

  // ɨ��/tmp/dataĿ¼���ļ���ƥ��"surfdata_*.xml"���ļ���
  if (Dir.OpenDir("/tmp/data","surfdata_*.xml")==false)
  {
    printf("Dir.OpenDir(/tmp/data) failed.\n"); return -1;
  }

  CFile File;

  while (Dir.ReadDir()==true)
  {
    printf("�����ļ�%s...",Dir.m_FullFileName);

    if (File.Open(Dir.m_FullFileName,"r")==false)
    {
      printf("failed.File.Open(%s) failed.\n",Dir.m_FullFileName); return -1;
    }

    char strBuffer[301];

    while (true)
    {
      memset(strBuffer,0,sizeof(strBuffer));
      if (File.FFGETS(strBuffer,300,"<endl/>")==false) break; // ��������"<endl/>"������

      // printf("strBuffer=%s",strBuffer);

      // ������Բ������xml�ַ�����������д�����ݿ�Ĵ��롣
    }

    // �������ļ��е����ݺ󣬹ر��ļ�ָ�룬��ɾ���ļ���
    File.CloseAndRemove();

    printf("ok\n");
  }
}

