/*
 *  ��������demo29.cpp���˳�����ʾ��������е�CTimer�ࣨ��ʱ�������÷���
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  CTimer Timer;

  printf("elapsed=%lf\n",Timer.Elapsed());
  sleep(1);
  printf("elapsed=%lf\n",Timer.Elapsed());
  sleep(1);
  printf("elapsed=%lf\n",Timer.Elapsed());
  usleep(1000);
  printf("elapsed=%lf\n",Timer.Elapsed());
  usleep(100);
  printf("elapsed=%lf\n",Timer.Elapsed());
  sleep(10);
  printf("elapsed=%lf\n",Timer.Elapsed());
}

