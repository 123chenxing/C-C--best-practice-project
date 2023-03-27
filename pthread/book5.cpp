// ��������ʾ�߳��߳��˳�����ֹ����״̬��
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg);    // �̵߳���������

struct st_ret
{
  int  retcode;          // ���ش��롣
  char message[1024];    // �������ݡ�
};

int main(int argc,char *argv[])
{
  pthread_t thid=0;

  // �����̡߳�
  if (pthread_create(&thid,NULL,thmain,NULL)!=0) { printf("pthread_create failed.\n"); exit(-1); }

  // �ȴ����߳��˳���
  printf("join...\n");
  struct st_ret *pst=0;
  pthread_join(thid,(void **)&pst);  
  printf("retcode=%d,message=%s\n",pst->retcode,pst->message);
  delete pst;
  printf("join ok.\n");
}

void *thmain(void *arg)    // �߳���������
{
  printf("�߳̿�ʼ���С�\n");

  // ע�⣬����ýṹ��ĵ�ַ��Ϊ�̵߳ķ���ֵ�����뱣�����߳��������������ַ������Ч�ġ�
  // ���ԣ�Ҫ���ö�̬�����ڴ�ķ����������þֲ�������
  struct st_ret *ret=new struct st_ret;
  ret->retcode=1121;
  strcpy(ret->message,"�������ݡ�");
  pthread_exit((void *)ret);
}

