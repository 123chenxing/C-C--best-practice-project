// ��������ʾ�߳���Դ�Ļ��գ��߳�����������
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg);    // �߳���������

void  thcleanup1(void *arg);    // �߳�������1��
void  thcleanup2(void *arg);    // �߳�������2��
void  thcleanup3(void *arg);    // �߳�������3��

int main(int argc,char *argv[])
{
  pthread_t thid;

  // �����̡߳�
  if (pthread_create(&thid,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  int result=0;
  void *ret;
  printf("join...\n");
  result=pthread_join(thid,&ret);   printf("thid result=%d,ret=%ld\n",result,ret);
  printf("join ok.\n");
}

void *thmain(void *arg)    // �߳���������
{
  pthread_cleanup_push(thcleanup1,NULL);  // ���߳�������1��ջ���ر��ļ�ָ�룩��
  pthread_cleanup_push(thcleanup2,NULL);  // ���߳�������2��ջ���ر�socket����
  pthread_cleanup_push(thcleanup3,NULL);  // ���߳�������3��ջ���ع����ݿ����񣩡� 

  for (int ii=0;ii<3;ii++)
  {
    sleep(1); printf("pthmain sleep(%d) ok.\n",ii+1);
  }

  pthread_cleanup_pop(3);  // ���߳�������3��ջ��
  pthread_cleanup_pop(2);  // ���߳�������2��ջ��
  pthread_cleanup_pop(1);  // ���߳�������1��ջ��
}

void  thcleanup1(void *arg)
{
  // �������ͷŹر��ļ����Ͽ��������ӡ��ع����ݿ������ͷ����ȵȡ�
  printf("cleanup1 ok.\n");
};

void  thcleanup2(void *arg)
{
  // �������ͷŹر��ļ����Ͽ��������ӡ��ع����ݿ������ͷ����ȵȡ�
  printf("cleanup2 ok.\n");
};

void  thcleanup3(void *arg)
{
  // �������ͷŹر��ļ����Ͽ��������ӡ��ع����ݿ������ͷ����ȵȡ�
  printf("cleanup3 ok.\n");
};

