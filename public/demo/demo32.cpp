/*
 *  ��������demo32.cpp���˳�����ʾ��������в���CDir���ȡĳĿ¼������Ŀ¼�е��ļ��б���Ϣ��
 *  ���ߣ������
*/
#include "../_public.h"

int main(int argc,char *argv[])
{
  if (argc != 2) { printf("��ָ��Ŀ¼����\n"); return -1; }

  CDir Dir;

  if (Dir.OpenDir(argv[1],"*.h,*cpp",100,true,true)==false)
  { 
    printf("Dir.OpenDir(%s) failed.\n",argv[1]); return -1; 
  }

  while(Dir.ReadDir()==true)
  {
    printf("filename=%s,mtime=%s,size=%d\n",Dir.m_FullFileName,Dir.m_ModifyTime,Dir.m_FileSize);
  }
}

