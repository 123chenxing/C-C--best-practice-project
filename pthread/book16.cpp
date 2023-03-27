// ��������ʾ�߳�ͬ��-�ź�����
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int var;

sem_t sem;      // �����ź�����

void *thmain(void *arg);    // �߳���������

int main(int argc,char *argv[])
{
  sem_init(&sem,0,1);   // ��ʼ���ź�����

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

  sem_destroy(&sem);  // �����ź�����
}

void *thmain(void *arg)    // �߳���������
{
  for (int ii=0;ii<1000000;ii++)
  {
    sem_wait(&sem);    // ������
    var++;
    sem_post(&sem);  // ������
  }
}
