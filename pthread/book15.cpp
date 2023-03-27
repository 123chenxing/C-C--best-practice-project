// ��������ʾ�߳�ͬ��-����������
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_cond_t cond=PTHREAD_COND_INITIALIZER;     // ����������������ʼ����
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;  // ��������������ʼ����

void *thmain(void *arg);    // �߳���������

void handle(int sig);       // �ź�15�Ĵ�������

int main(int argc,char *argv[])
{
  signal(15,handle);       // �����ź�15�Ĵ�������

  pthread_t thid1,thid2,thid3;

  // �����̡߳�
  if (pthread_create(&thid1,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid2,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
  if (pthread_create(&thid3,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  pthread_join(thid1,NULL);  pthread_join(thid2,NULL);  pthread_join(thid3,NULL);  

  pthread_cond_destroy(&cond);    // ��������������
  pthread_mutex_destroy(&mutex);  // ���ٻ�������
}

void *thmain(void *arg)    // �߳���������
{
  while (true)
  {
    printf("�߳�%lu��ʼ�ȴ������ź�...\n",pthread_self());
    pthread_cond_wait(&cond,&mutex);    // �ȴ������źš�
    printf("�߳�%lu�ȴ������źųɹ���\n\n",pthread_self());
  }
}

void handle(int sig)       // �ź�15�Ĵ�������
{
  printf("���������ź�...\n");
  // pthread_cond_signal(&cond);      // ���ѵȴ�����������һ���̡߳�
  pthread_cond_broadcast(&cond);   // ���ѵȴ�����������ȫ���̡߳�
}
