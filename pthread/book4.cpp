// ��������ʾ�̲߳����Ĵ��ݣ��ýṹ��ĵ�ַ���ݶ����������
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg);    // �̵߳���������

struct st_args
{
  int  no;        // �̱߳�š�
  char name[51];  // �߳�����
};

int main(int argc,char *argv[])
{
  pthread_t thid=0;

  // �����̡߳�
  struct st_args *stargs=new struct st_args;
  stargs->no=15;   strcpy(stargs->name,"�����߳�");
  if (pthread_create(&thid,NULL,thmain,stargs)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  printf("join...\n");
  pthread_join(thid,NULL);  
  printf("join ok.\n");
}

void *thmain(void *arg)    // �߳���������
{
  struct st_args *pst=(struct st_args *)arg;
  printf("no=%d,name=%s\n",pst->no,pst->name);
  delete pst;
  printf("�߳̿�ʼ���С�\n");
}

