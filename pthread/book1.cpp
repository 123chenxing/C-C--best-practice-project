// ��������ʾ�̵߳Ĵ�������ֹ��
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int var=0;

void *thmain1(void *arg);    // �߳���������
void *thmain2(void *arg);    // �߳���������

int main(int argc,char *argv[])
{
  pthread_t thid1=0;   // �߳�id typedef unsigned long pthread_t
  pthread_t thid2=0;   // �߳�id typedef unsigned long pthread_t

  // �����̡߳�
  if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid2,NULL,thmain2,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  printf("join...\n");
  pthread_join(thid1,NULL);  
  pthread_join(thid2,NULL);  
  printf("join ok.\n");
}

void fun1()
{ return;}

void *thmain1(void *arg)    // �߳���������
{
  for (int ii=0;ii<5;ii++)
  {
    var=ii+1;
    sleep(1); printf("pthmain1 sleep(%d) ok.\n",var);
    if (ii==2) fun1();
  }
}

void fun2()
{ pthread_exit(0);}

void *thmain2(void *arg)    // �߳���������
{
  for (int ii=0;ii<5;ii++)
  {
    sleep(1); printf("pthmain2 sleep(%d) ok.\n",var);
    if (ii==2) fun2();
  }
}
