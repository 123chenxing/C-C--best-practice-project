/*
 *  ��������demo50.cpp���˳�����ʾ���ÿ�����ܵ�Cftp���ȡ�������ļ��б�ʱ��ʹ�С��
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

  // ��ȡ��������/home/wucz/*.h�ļ��б������ڱ��ص�/tmp/list/tmp.list�ļ��С�
  // ���/tmp/listĿ¼�����ڣ��ʹ�������
  if (ftp.nlist("/home/wucz/*.h","/tmp/list/tmp.list")==false) 
  { printf("ftp.nlist() failed.\n"); return -1; }

  CFile File;    // ���ÿ�����ܵ�CFile��������list�ļ���
  char strFileName[301];

  File.Open("/tmp/list/tmp.list","r");  // ��list�ļ���

  while(true)    // ��ȡÿ���ļ���ʱ��ʹ�С��
  {
    if (File.Fgets(strFileName,300,true)==false) break;

    ftp.mtime(strFileName); // ��ȡ�ļ�ʱ�䡣
    ftp.size(strFileName);  // ��ȡ�ļ���С��
  
    printf("filename=%s,mtime=%s,size=%d\n",strFileName,ftp.m_mtime,ftp.m_size);   
  }

}

