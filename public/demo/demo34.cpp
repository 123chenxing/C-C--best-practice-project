/*
 *  ��������demo34.cpp���˳�����ʾ������ܵ��ļ������������÷�
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  // ɾ���ļ���
  if (REMOVE("/project/public/lib_public.a")==false)
  {
    printf("REMOVE(/project/public/lib_public.a) %d:%s\n",errno,strerror(errno));
  }

  // �������ļ���
  if (RENAME("/project/public/lib_public.so","/tmp/aaa/bbb/ccc/lib_public.so")==false)
  {
    printf("RENAME(/project/public/lib_public.so) %d:%s\n",errno,strerror(errno));
  }

  // �����ļ���
  if (COPY("/project/public/libftp.a","/tmp/root/aaa/bbb/ccc/libftp.a")==false)
  {
    printf("COPY(/project/public/libftp.a) %d:%s\n",errno,strerror(errno));
  }

  // ��ȡ�ļ��Ĵ�С��
  printf("size=%d\n",FileSize("/project/public/_public.h"));

  // �����ļ���ʱ�䡣
  UTime("/project/public/_public.h","2020-01-05 13:37:29");

  // ��ȡ�ļ���ʱ�䡣
  char mtime[21]; memset(mtime,0,sizeof(mtime));
  FileMTime("/project/public/_public.h",mtime,"yyyy-mm-dd hh24:mi:ss");
  printf("mtime=%s\n",mtime);   // ���mtime=2020-01-05 13:37:29
}

