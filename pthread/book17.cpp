// ��������ʾ�߳�ͬ��-����������pthread_cond_wait(&cond,&mutex)�����з�����ʲô��
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_cond_t cond=PTHREAD_COND_INITIALIZER;     // ����������������ʼ����
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;  // ��������������ʼ����

void *thmain1(void *arg);    // �߳�1��������
void *thmain2(void *arg);    // �߳�2��������

int main(int argc,char *argv[])
{
  pthread_t thid1,thid2;

  // �����̡߳�
  if (pthread_create(&thid1,NULL,thmain1,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
sleep(1);
  if (pthread_create(&thid2,NULL,thmain2,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  pthread_join(thid1,NULL);  pthread_join(thid2,NULL);  

  pthread_cond_destroy(&cond);    // ��������������
  pthread_mutex_destroy(&mutex);  // ���ٻ�������
}

void *thmain1(void *arg)    // �߳�1����������
{
  printf("�߳�һ���뻥����...\n");
  pthread_mutex_lock(&mutex);
  printf("�߳�һ���뻥�����ɹ���\n");

  printf("�߳�һ��ʼ�ȴ������ź�...\n");
  pthread_cond_wait(&cond,&mutex);    // �ȴ������źš�
  printf("�߳�һ�ȴ������źųɹ���\n");
}

void *thmain2(void *arg)    // �߳�2����������
{
  printf("�̶߳����뻥����...\n");
  pthread_mutex_lock(&mutex);
  printf("�̶߳����뻥�����ɹ���\n");

  pthread_cond_signal(&cond); 

  sleep(5);

  printf("�̶߳�������\n");
  pthread_mutex_unlock(&mutex);

  return 0;

  printf("�̶߳����뻥����...\n");
  pthread_mutex_lock(&mutex);
  printf("�̶߳����뻥�����ɹ���\n");

  printf("�̶߳���ʼ�ȴ������ź�...\n");
  pthread_cond_wait(&cond,&mutex);    // �ȴ������źš�
  printf("�̶߳��ȴ������źųɹ���\n");
}

