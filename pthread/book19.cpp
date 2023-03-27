// ��������ʾֻ���ź���ʵ�ָ��ٻ��档
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>
#include <vector>

using namespace std;

// ���������Ϣ�Ľṹ�塣
struct st_message
{
  int  mesgid;          // ��Ϣ��id��
  char message[1024];   // ��Ϣ�����ݡ�
}stmesg;

vector<struct st_message> vcache;  // ��vector���������档

// pthread_cond_t  cond=PTHREAD_COND_INITIALIZER;     // ��������ʼ������������
// pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;   // ��������ʼ����������
sem_t notify,lock;     // �����ź�����

void  incache(int sig);      // �����ߡ�������ӡ�
void *outcache(void *arg);   // �����ߡ����ݳ����̵߳���������

int main()
{
  signal(15,incache);  // ����15���źţ����������ߺ�����

  sem_init(&notify,0,0);   // ��ʼ��֪ͨ���ź�������3������Ϊ0��
  sem_init(&lock,0,1);     // ��ʼ���������ź�������3������Ϊ1��

  // ���������������̡߳�
  pthread_t thid1,thid2,thid3;
  pthread_create(&thid1,NULL,outcache,NULL);
  pthread_create(&thid2,NULL,outcache,NULL);
  pthread_create(&thid3,NULL,outcache,NULL);

  pthread_join(thid1,NULL);
  pthread_join(thid2,NULL);
  pthread_join(thid3,NULL);

  sem_destroy(&notify);
  sem_destroy(&lock);

  return 0;
}

void incache(int sig)       // �����ߡ�������ӡ�
{
  static int mesgid=1;  // ��Ϣ�ļ�������

  struct st_message stmesg;      // ��Ϣ���ݡ�
  memset(&stmesg,0,sizeof(struct st_message));

  // pthread_mutex_lock(&mutex);    // ��������м�����
  sem_wait(&lock);

  // �������ݣ����뻺����С�
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  
  stmesg.mesgid=mesgid++; vcache.push_back(stmesg);  

  // pthread_mutex_unlock(&mutex);  // ��������н�����
  sem_post(&lock);

  // pthread_cond_signal(&cond);    // ���������źţ�����һ���̡߳�
  // pthread_cond_broadcast(&cond); // ���������źţ�����ȫ�����̡߳�

  sem_post(&notify);    // ���ź�����ֵ��1���������������̡߳�
  sem_post(&notify);    // ���ź�����ֵ��1���������������̡߳�
  sem_post(&notify);    // ���ź�����ֵ��1���������������̡߳�
  sem_post(&notify);    // ���ź�����ֵ��1���������������̡߳�
}

void *outcache(void *arg)    // �����ߡ����ݳ����̵߳���������
{
  struct st_message stmesg;  // ���ڴ�ų��ӵ���Ϣ��

  while (true)
  {
    sem_wait(&lock);    // ��������м�����

    // ����������Ϊ�գ��ȴ�����while��ֹ��ٻ��ѡ�
    while (vcache.size()==0)
    {
      // pthread_cond_wait(&cond,&mutex);

      sem_post(&lock);              // ��������н�����
      sem_wait(&notify);            // �ȴ��ź�����ֵ����0��
      sem_wait(&lock);              // ��������м�����
    }

    // �ӻ�������л�ȡ��һ����¼��Ȼ��ɾ���ü�¼��
    memcpy(&stmesg,&vcache[0],sizeof(struct st_message)); // �ڴ濽����
    vcache.erase(vcache.begin());

    sem_post(&lock);              // ��������н�����

    // �����Ǵ���ҵ��Ĵ��롣
    printf("phid=%ld,mesgid=%d\n",pthread_self(),stmesg.mesgid);
    usleep(100);
  }
}

