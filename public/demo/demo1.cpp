/*
 * ��������demo1.cpp���˳�����ʾ���������STRCPY������ʹ�á�
 * ���ߣ������
 */


#include "../_public.h"

int main(int argc,char *argv[])
{
  char str[11];   // �ַ���str�Ĵ�С��11�ֽڡ�

  STRCPY(str,sizeof(str),"google");  // �����Ƶ�����û�г���str���Դ���ַ����Ĵ�С��
  printf("str=%s=\n",str);    // ��������str=google=

  STRCPY(str,sizeof(str),"www.google.com");  // �����Ƶ����ݳ�����str���Դ���ַ����Ĵ�С��
  printf("str=%s=\n",str);    // ��������str=www.google=
}

