/*
 *  ��������createtable.cpp���˳�����ʾ������ܲ���MySQL���ݿ⣨��������
 *  ���ߣ��¹ڡ�
*/
#include "_mysql.h"   // ������ܲ���MySQL��ͷ�ļ���

int main(int argc,char *argv[])
{
  connection conn; // ���ݿ�������

  // ��¼���ݿ⣬����ֵ��0-�ɹ�������-ʧ�ܡ�
  // ʧ�ܴ�����conn.m_cda.rc�У�ʧ��������conn.m_cda.message�С�
  if (conn.connecttodb("127.0.0.1,root,13251009668,mysql,3306","utf8") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
    sqlstatement stmt(&conn);     //����SQL���Ķ���  
    //׼���������MYSQL��䡣
    //��Ů��girls����Ů���id����Ů����name������weight������ʱ��btime����Ů˵��memo����ŮͼƬpic��
    stmt.prepare("create table girls(id           bigint(10),\    
                   name         varchar(30),\
                   weight       decimal(8,2),\   
                   btime        datetime,\  
                   memo         longtext,\      
                   pic          longblob,\       
                   primary key(id))");

/*
  1��int prepare(const char *fmt,...)��SQL�����Զ�����д��
  2��SQL������ķֺſ��п��ޣ����鲻Ҫд�������Կ��ǣ���
  3��SQL����в�����˵�����֡�
  4�����Բ����ж�stmt.prepare()�ķ���ֵ��stmt.execute()ʱ���жϡ�
 */
  // ִ��SQL��䣬һ��Ҫ�жϷ���ֵ��0-�ɹ�������-ʧ�ܡ�
  // ʧ�ܴ�����stmt.m_cda.rc�У�ʧ��������stmt.m_cda.message�С�
  if(stmt.execute()!=0){

    printf("stmt.execute() failed.\n%s\n%d\n%s\n",stmt.m_sql,stmt.m_cda.rc,stmt.m_cda.message);
    return -1;
}
    return 0;
}
/*
--��Ů������Ϣ��
create table girls(id           bigint(10),	--��Ů��š�    
                   name         varchar(30),    --��Ů������
                   weight       decimal(8,2),   --��Ů���ء�
                   btime        datetime,       --����ʱ�䡣
                   memo         longtext,       --��ע��
                   pic          longblob,        --��Ƭ��
                   primary key(id));
*/



