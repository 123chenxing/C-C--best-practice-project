/*
 *  ��������demo28.cpp���˳�����ʾ��������в���AddTime����ʱ������㡣
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  time_t ltime;
  char strtime[20];

  memset(strtime,0,sizeof(strtime));
  strcpy(strtime,"2020-01-01 12:35:22");

  AddTime(strtime,strtime,0-1*24*60*60); // ��һ�졣
  printf("strtime=%s\n",strtime);     // ���strtime=2019-12-31 12:35:22
  
  AddTime(strtime,strtime,2*24*60*60); // �����졣
  printf("strtime=%s\n",strtime);     // ���strtime=2020-01-02 12:35:22
}

