/*************************************************/
/*  ��������idcapp.h���˳���������������Ŀ���ú�������������ļ���*/
/*  ���ߣ��¹�
/******************************************************************/

#ifndef IDCAPP_H
#define IDCAPP_H

#include "_public.h"
#include "_mysql.h"
struct st_zhobtmind{
 char obtid[11];          //վ�����
 char ddatetime[21];      //����ʱ�䣬��ȷ�����ӡ�
 char t[11];              //�¶ȣ���λ��0.1���϶ȡ�
 char p[11];              //��ѹ����λ��0.1������
 char u[11];              //���ʪ�ȣ�0-100֮���ֵ��
 char wd[11];             //����0-360֮���ֵ��
 char wf[11];             //���٣���λ0.1m/s��
 char r[11];              //��������0.1mm��
 char vis[11];            //�ܼ��ȣ�0.1�ס�
};

// ȫ��վ����ӹ۲����ݲ����ࡣ
 class CZHOBTMIND{
 public:
 connection *m_conn;                  // ���ݿ����ӡ�
 CLogFile *m_logfile;                  //��־��

 sqlstatement m_stmt;                 // ����������sql��

 char m_buffer[1024];                 // ���ļ��ж�ȡ����һ�С�
 struct st_zhobtmind m_zhobtmind;     // ȫ��վ����ӹ۲����ݽṹ��
 CZHOBTMIND();
 CZHOBTMIND(connection *conn,CLogFile *logfile);

 ~CZHOBTMIND();
 void bindConnLog(connection *conn,CLogFile *logfile);  // ��connection��CLogFile�Ĵ���ȥ��
 bool SplitBuffer(char *strBuffer,bool bisxml);  // �Ѵ��ļ�������һ�����ݲ�ֵ�m_zhobtmind�ṹ���С�
 bool InsertTable();               // ��m_zhobtmind�ṹ���е����ݲ��뵽T_ZHOBTMIND���С�
 };




#endif
