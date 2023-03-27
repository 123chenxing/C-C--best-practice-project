// ��������ʾ�߳���Դ�Ļ��գ������̣߳���
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg);    // �߳���������

int main(int argc,char *argv[])
{
  pthread_t thid;

  pthread_attr_t attr;         // �����߳����Ե����ݽṹ��
  pthread_attr_init(&attr);    // ��ʼ����
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);  // �����̵߳����ԡ�
  // �����̡߳�
  if (pthread_create(&thid,&attr,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  pthread_attr_destroy(&attr); // �������ݽṹ��

  // pthread_detach(pthread_self());

  sleep(5);

  int result=0;
  void *ret;
  printf("join...\n");
  result=pthread_join(thid,&ret);   printf("thid result=%d,ret=%ld\n",result,ret);
  printf("join ok.\n");
}

void *thmain(void *arg)    // �߳���������
{
  // pthread_detach(pthread_self());

  for (int ii=0;ii<3;ii++)
  {
    sleep(1); printf("pthmain sleep(%d) ok.\n",ii+1);
  }
  return (void *) 1;
}
