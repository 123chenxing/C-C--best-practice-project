/*
 *  ��������demo2.cpp���˳�����ʾ���������STRNCPY������ʹ�á�
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  char str[11];   // �ַ���str�Ĵ�С��11�ֽڡ�

  STRNCPY(str,sizeof(str),"google",5);  // �����Ƶ�����û�г���str���Դ���ַ����Ĵ�С��
  printf("str=%s=\n",str);    // ��������str=googl=

  STRNCPY(str,sizeof(str),"www.google.com",8);  // �����Ƶ�����û�г���str���Դ���ַ����Ĵ�С��
  printf("str=%s=\n",str);    // ��������str=www.goog=

  STRNCPY(str,sizeof(str),"www.google.com",17);  // �����Ƶ����ݳ�����str���Դ���ַ����Ĵ�С��
  printf("str=%s=\n",str);    // ��������str=www.google=
}

