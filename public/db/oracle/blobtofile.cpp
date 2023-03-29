/*
 *  ��������bolbtofile.cpp���˳�����ʾ������ܲ���Oracle���ݿ⡣
 *  ��Oracle��BLOB�ֶε�������ȡ��ĿǰĿ¼��pic_out.jpeg�С�
 *  ���ߣ�����ܡ�
*/
#include "_ooci.h"   // ������ܲ���Oracle��ͷ�ļ���

int main(int argc,char *argv[])
{
  connection conn; // ���ݿ�������

  // �������ݿ⣬����ֵ0-�ɹ�������-ʧ��
  // ʧ�ܴ�����conn.m_cda.rc�У�ʧ��������conn.m_cda.message�С�
  if (conn.connecttodb("scott/tiger@snorcl11g_132","Simplified Chinese_China.AL32UTF8") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }

  sqlstatement stmt(&conn); // SQL��������

  // ����Ҫ�жϷ���ֵ
  stmt.prepare("select pic from girls where id=1");
  stmt.bindblob();

  // ִ��SQL��䣬һ��Ҫ�жϷ���ֵ��0-�ɹ�������-ʧ�ܡ�
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }

  // ��ȡһ����¼��һ��Ҫ�жϷ���ֵ��0-�ɹ���1403-�޼�¼������-ʧ�ܡ�
  if (stmt.next() != 0) return 0;

  // ��BLOB�ֶ��е�����д������ļ���һ��Ҫ�жϷ���ֵ��0-�ɹ�������-ʧ�ܡ�
  if (stmt.lobtofile((char *)"pic_out.jpeg") != 0)
  {
    printf("stmt.lobtofile() failed.\n%s\n",stmt.m_cda.message); return -1;
  }
}
