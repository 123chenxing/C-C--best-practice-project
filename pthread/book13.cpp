// ��������ʾ�߳�ͬ��-��������
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int var;

pthread_spinlock_t spin;      // ������������

void *thmain(void *arg);    // �߳���������

int main(int argc,char *argv[])
{
  pthread_spin_init(&spin,PTHREAD_PROCESS_PRIVATE);   // ��ʼ����������

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

  pthread_spin_destroy(&spin);  // ��������
}

void *thmain(void *arg)    // �߳���������
{
  for (int ii=0;ii<1000000;ii++)
  {
    pthread_spin_lock(&spin);    // ������
    var++;
    pthread_spin_unlock(&spin);  // ������
  }
}
