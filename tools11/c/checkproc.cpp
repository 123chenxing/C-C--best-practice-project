#include "_public.h"

// �������е���־��
CLogFile logfile;

int main(int argc,char *argv[])
{
  // ����İ�����
  if (argc != 2)
  {
    printf("\n");
    printf("Using:./checkproc logfilename\n");

    printf("Example:/project/tools1/bin/procctl 10 /project/tools1/bin/checkproc /tmp/log/checkproc.log\n\n");

    printf("���������ڼ���̨��������Ƿ�ʱ������ѳ�ʱ������ֹ����\n");
    printf("ע�⣺\n");
    printf("  1����������procctl�������������ڽ���Ϊ10�롣\n");
    printf("  2��Ϊ�˱��ⱻ��ͨ�û���ɱ��������Ӧ����root�û�������\n");
    printf("  3�����Ҫֹͣ������ֻ����killall -9 ��ֹ��\n\n\n");

    return 0;
  }

  // ����ȫ�����źź�IO����ϣ�����򱻸��š�
  CloseIOAndSignal(true);

  // ����־�ļ���
  if (logfile.Open(argv[1],"a+")==false)
  { printf("logfile.Open(%s) failed.\n",argv[1]); return -1; }

  int shmid=0;

  // ����/��ȡ�����ڴ棬��ֵΪSHMKEYP����СΪMAXNUMP��st_procinfo�ṹ��Ĵ�С��
  if ( (shmid = shmget((key_t)SHMKEYP, MAXNUMP*sizeof(struct st_procinfo), 0666|IPC_CREAT)) == -1)
  {
    logfile.Write("����/��ȡ�����ڴ�(%x)ʧ�ܡ�\n",SHMKEYP); return false;
  }

  // �������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ䡣
  struct st_procinfo *shm=(struct st_procinfo *)shmat(shmid, 0, 0);

  // ���������ڴ���ȫ���ļ�¼��
  for (int ii=0;ii<MAXNUMP;ii++)
  {
    // �����¼��pid==0����ʾ�ռ�¼��continue;
    if (shm[ii].pid==0) continue;

    // �����¼��pid!=0����ʾ�Ƿ�������������¼��

    // �����ȶ����к��������д������ע�͵���
    //logfile.Write("ii=%d,pid=%d,pname=%s,timeout=%d,atime=%d\n",\
    //               ii,shm[ii].pid,shm[ii].pname,shm[ii].timeout,shm[ii].atime);

    // ����̷����ź�0���ж����Ƿ񻹴��ڣ���������ڣ��ӹ����ڴ���ɾ���ü�¼��continue;
    int iret=kill(shm[ii].pid,0);
    if (iret==-1)
    {
      logfile.Write("����pid=%d(%s)�Ѿ������ڡ�\n",(shm+ii)->pid,(shm+ii)->pname);
      memset(shm+ii,0,sizeof(struct st_procinfo)); // �ӹ����ڴ���ɾ���ü�¼��
      continue;
    }

    time_t now=time(0);   // ȡ��ǰʱ�䡣

    // �������δ��ʱ��continue;
    if (now-shm[ii].atime<shm[ii].timeout) continue;

    // ����ѳ�ʱ��
    logfile.Write("����pid=%d(%s)�Ѿ���ʱ��\n",(shm+ii)->pid,(shm+ii)->pname);

    // �����ź�15������������ֹ���̡�
    kill(shm[ii].pid,15);     

    // ÿ��1���ж�һ�ν����Ƿ���ڣ��ۼ�5�룬һ����˵��5���ʱ���㹻�ý����˳���
    for (int jj=0;jj<5;jj++)
    {
      sleep(1);
      iret=kill(shm[ii].pid,0);     // ����̷����ź�0���ж����Ƿ񻹴��ڡ�
      if (iret==-1) break;     // �������˳���
    } 

    // ��������Դ��ڣ��ͷ����ź�9��ǿ����ֹ����
    if (iret==-1)
      logfile.Write("����pid=%d(%s)�Ѿ�������ֹ��\n",(shm+ii)->pid,(shm+ii)->pname);
    else
    {
      kill(shm[ii].pid,9);  // ��������Դ��ڣ��ͷ����ź�9��ǿ����ֹ����
      logfile.Write("����pid=%d(%s)�Ѿ�ǿ����ֹ��\n",(shm+ii)->pid,(shm+ii)->pname);
    }
    
    // �ӹ����ڴ���ɾ���ѳ�ʱ���̵�������¼��
    memset(shm+ii,0,sizeof(struct st_procinfo)); // �ӹ����ڴ���ɾ���ü�¼��
  }

  // �ѹ����ڴ�ӵ�ǰ�����з��롣
  shmdt(shm);

  return 0;
}
