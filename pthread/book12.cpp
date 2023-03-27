// ��������ʾ�߳�ͬ��-��������
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int var;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;      // ������������

void *thmain(void *arg);    // �߳���������

int main(int argc,char *argv[])
{
  // pthread_mutex_init(&mutex,NULL);   // ��ʼ����������

  pthread_t thid1,thid2;

  // �����̡߳�
  if (pthread_create(&thid1,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid2,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  printf("join...\n");
  pthread_join(thid1,NULL);  
  pthread_join(thid2,NULL);  
  printf("join ok.\n");

  printf("var=%d\n",var);

  pthread_mutex_destroy(&mutex);  // ��������
}

void *thmain(void *arg)    // �߳���������
{
  for (int ii=0;ii<1000000;ii++)
  {
    pthread_mutex_lock(&mutex);    // ������
    var++;
    pthread_mutex_unlock(&mutex);  // ������
  }
}
