/*
 *  ��������demo7.cpp���˳�����ʾ���������SPRINTF������ʹ�á�
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  char str[21];   // �ַ���str�Ĵ�С��21�ֽڡ�

  SPRINTF(str,sizeof(str),"name:%s,no:%d","messi",10);
  printf("str=%s=\n",str);    // ��������str=name:messi,no:10=

  SPRINTF(str,sizeof(str),"name:%s,no:%d,job:%s","messi",10,"striker");
  printf("str=%s=\n",str);    // ��������str=name:messi,no:10,job=
}

