/*
 *  ��������demo10.cpp���˳�����ʾ���������ɾ���ַ������ҡ�����ָ���ַ���ʹ�á�
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  char str[11];   // �ַ���str�Ĵ�С��11�ֽڡ�

  STRCPY(str,sizeof(str),"  ��ʩ  ");
  DeleteLChar(str,' ');  // ɾ��str��ߵĿո�
  printf("str=%s=\n",str);    // ��������str=��ʩ  =

  STRCPY(str,sizeof(str),"  ��ʩ  ");
  DeleteRChar(str,' ');  // ɾ��str�ұߵĿո�
  printf("str=%s=\n",str);    // ��������str=  ��ʩ=

  STRCPY(str,sizeof(str),"  ��ʩ  ");
  DeleteLRChar(str,' ');  // ɾ��str���ߵĿո�
  printf("str=%s=\n",str);    // ��������str=��ʩ=
}

