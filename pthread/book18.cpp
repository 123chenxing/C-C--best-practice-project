// ��������ʾ�û���������������ʵ�ָ��ٻ��档
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <vector>

using namespace std;

// ���������Ϣ�Ľṹ�塣
struct st_message
{
  int  mesgid;          // ��Ϣ��id��
  char message[1024];   // ��Ϣ�����ݡ�
}stmesg;

vector<struct st_message> vcache;  // ��vector���������档

pthread_cond_t  cond=PTHREAD_COND_INITIALIZER;     // ��������ʼ������������
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;   // ��������ʼ����������

void  incache(int sig);      // �����ߡ�������ӡ�
void *outcache(void *arg);   // �����ߡ����ݳ����̵߳���������

int main()
{
  signal(15,incache);  // ����15���źţ����������ߺ�����

  // ���������������̡߳�
  pthread_t thid1,thid2,thid3;
  pthread_create(&thid1,NULL,outcache,NULL);
  pthread_create(&thid2,NULL,outcache,NULL);
  pthread_create(&thid3,NULL,outcache,NULL);

  pthread_join(thid1,NULL);
  pthread_join(thid2,NULL);
  pthread_join(thid3,NULL);

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);

  return 0;
}

void incache(int sig)       // �����ߡ�������ӡ�
{
  static int mesgid=1;  // ��Ϣ�ļ�������

  struct st_message stmesg;      // ��Ϣ���ݡ�
  memset(&stmesg,0,sizeof(struct st_message));

  pthread_mutex_lock(&mutex);    // ��������м�����

  //  �������ݣ����뻺����С�
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  

  pthread_mutex_unlock(&mutex);  // ��������н�����

  //pthread_cond_signal(&cond);    // ���������źţ�����һ���̡߳�
  pthread_cond_broadcast(&cond); // ���������źţ�����ȫ�����̡߳�
}

void  thcleanup(void *arg)
{
  // �������ͷŹر��ļ����Ͽ��������ӡ��ع����ݿ������ͷ����ȵȡ�
  printf("cleanup ok.\n");

  pthread_mutex_unlock(&mutex);

  /*
  A condition  wait  (whether  timed  or  not)  is  a  cancellation  point. When the cancelability type of a thread is set to PTHREAD_CAN_CEL_DEFERRED, a side-effect of acting upon a cancellation request while in a condition wait is that the mutex is (in effect)  re-acquired before  calling the first cancellation cleanup handler. The effect is as if the thread were unblocked, allowed to execute up to the point of returning from the call to pthread_cond_timedwait() or pthread_cond_wait(), but at that point notices  the  cancellation  request  and instead  of  returning to the caller of pthread_cond_timedwait() or pthread_cond_wait(), starts the thread cancellation activities, which includes calling cancellation cleanup handlers.
  ��˼������pthread_cond_waitʱִ��pthread_cancel��
  Ҫ�����߳���������Ҫ�Ƚ���������Ӧ���������󶨵�mutex��
  ������Ϊ�˱�֤pthread_cond_wait���Է��ص������̡߳�
  */
};

void *outcache(void *arg)    // �����ߡ����ݳ����̵߳���������
{
  pthread_cleanup_push(thcleanup,NULL);  // ���߳���������ջ��

  struct st_message stmesg;  // ���ڴ�ų��ӵ���Ϣ��

  while (true)
  {
    pthread_mutex_lock(&mutex);  // ��������м�����

    // ����������Ϊ�գ��ȴ�����while��ֹ����������ٻ��ѡ�
    while (vcache.size()==0)
    {
      pthread_cond_wait(&cond,&mutex);
    }

    // �ӻ�������л�ȡ��һ����¼��Ȼ��ɾ���ü�¼��
    memcpy(&stmesg,&vcache[0],sizeof(struct st_message)); // �ڴ濽����
    vcache.erase(vcache.begin());

    pthread_mutex_unlock(&mutex);  // ��������н�����

    // �����Ǵ���ҵ��Ĵ��롣
    printf("phid=%ld,mesgid=%d\n",pthread_self(),stmesg.mesgid);
    usleep(100);
  }

  pthread_cleanup_pop(1);  // ���߳���������ջ��
}

