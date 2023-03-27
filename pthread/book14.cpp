// ��������ʾ�߳�ͬ��-��д����
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_rwlock_t rwlock=PTHREAD_RWLOCK_INITIALIZER;   // ������д������ʼ����

void *thmain(void *arg);    // �߳���������

void handle(int sig);       // �ź�15�Ĵ�������

int main(int argc,char *argv[])
{
  signal(15,handle);       // �����ź�15�Ĵ�������

  pthread_t thid1,thid2,thid3;

  // �����̡߳�
  if (pthread_create(&thid1,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
sleep(1);
  if (pthread_create(&thid2,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }
sleep(1);
  if (pthread_create(&thid3,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  pthread_join(thid1,NULL);  pthread_join(thid2,NULL);  pthread_join(thid3,NULL);  

  pthread_rwlock_destroy(&rwlock);  // ��������
}

void *thmain(void *arg)    // �߳���������
{
  for (int ii=0;ii<100;ii++)
  {
    printf("�߳�%lu��ʼ�������...\n",pthread_self());
    pthread_rwlock_rdlock(&rwlock);    // ������
    printf("�߳�%lu��ʼ��������ɹ���\n\n",pthread_self());
    sleep(5);
    pthread_rwlock_unlock(&rwlock);    // ������
    printf("�߳�%lu���ͷŶ�����\n\n",pthread_self());

    if (ii==3) sleep(8);
  }
}

void handle(int sig)       // �ź�15�Ĵ�������
{
  printf("��ʼ����д��...\n");
  pthread_rwlock_wrlock(&rwlock);    // ������
  printf("����д���ɹ���\n\n");
  sleep(10);
  pthread_rwlock_unlock(&rwlock);    // ������
  printf("д�����ͷš�\n\n");
}
