/*
 *  ��������demo8.cpp���˳�����ʾ���������SNPRINTF������ʹ�á�
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  char str[26];   // �ַ���str�Ĵ�С��11�ֽڡ�

  SNPRINTF(str,sizeof(str),5,"messi");
  printf("str=%s=\n",str);    // ��������str=mess=

  SNPRINTF(str,sizeof(str),9,"name:%s,no:%d,job:%s","messi",10,"striker");
  printf("str=%s=\n",str);    // ��������str=name:mes=

  SNPRINTF(str,sizeof(str),30,"name:%s,no:%d,job:%s","messi",10,"striker");
  printf("str=%s=\n",str);    // ��������str=name:messi,no:10,job:stri=
}

