#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
  if (argc<3)
  {
    printf("Using:./procctl timetvl program argv ...\n");
    printf("Example:/project/tools11/bin/procctl 5 /usr/bin/tar zcvf /tmp/tmp.tgz /usr/include\n\n");

    printf("�������Ƿ������ĵ��ȳ���������������������shell�ű���\n");
    printf("timetvl �������ڣ���λ���롣�����ȵĳ������н�������timetvl���ᱻprocctl����������\n");
    printf("program �����ȵĳ�����������ʹ��ȫ·����\n");
    printf("argvs   �����ȵĳ���Ĳ�����\n");
    printf("ע�⣬�����򲻻ᱻkillɱ������������kill -9ǿ��ɱ����\n\n\n");

    return -1;
  }

  // �ر��źź�IO��������ϣ�������š�
  for (int ii=0;ii<64;ii++)
  {
    signal(ii,SIG_IGN); close(ii);
  }

  // �����ӽ��̣��������˳����ó��������ں�̨����ϵͳ1�Ž����йܡ�
  if (fork()!=0) exit(0);

  // ����SIGCHLD�źţ��ø����̿���wait�ӽ����˳���״̬��
  signal(SIGCHLD,SIG_DFL);

  char *pargv[argc];
  for (int ii=2;ii<argc;ii++)
    pargv[ii-2]=argv[ii];

  pargv[argc-2]=NULL;

  while (true)
  {
    if (fork()==0)
    {
      execv(argv[2],pargv);
      exit(0);
    }
    else
    {
      int status;
      wait(&status);
      sleep(atoi(argv[1]));
    }
  }
}
