/*
 *  ��������demo52.cpp���˳�����ʾ���ÿ�����ܵ�Cftp�������ļ���
 *  ���ߣ������
*/
#include "../_ftp.h"

int main(int argc,char *argv[])
{
  Cftp ftp;

  // ��¼Զ��FTP�����������Ϊ���Լ���������ip��ַ��
  if (ftp.login("172.16.0.15:21","wucz","freecpluspwd",FTPLIB_PASSIVE) == false)
  {
    printf("ftp.login(172.16.0.15:21(wucz/freecpluspwd)) failed.\n"); return -1;
  }

  // �ѷ������ϵ�/home/wucz/tmp/demo51.cpp���ص����أ���Ϊ/tmp/test/demo51.cpp��
  // ������ص�/tmp/testĿ¼�����ڣ��ʹ�������
  if (ftp.get("/home/wucz/tmp/demo51.cpp","/tmp/test/demo51.cpp")==false)
  { printf("ftp.get() failed.\n"); return -1; }

  printf("get /home/wucz/tmp/demo51.cpp ok.\n");  

  // ɾ�������ϵ�/home/wucz/tmp/demo51.cpp�ļ���
  if (ftp.ftpdelete("/home/wucz/tmp/demo51.cpp")==false) 
  { printf("ftp.ftpdelete() failed.\n"); return -1; }

  printf("delete /home/wucz/tmp/demo51.cpp ok.\n");  

  // ɾ���������ϵ�/home/wucz/tmpĿ¼�����Ŀ¼�ǿգ�ɾ����ʧ�ܡ�
  if (ftp.rmdir("/home/wucz/tmp")==false) { printf("ftp.rmdir() failed.\n"); return -1; }
}

