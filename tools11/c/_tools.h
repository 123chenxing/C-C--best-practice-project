#ifndef _TOOLS_H
#define _TOOLS_H

#include "_public.h"
#include "_mysql.h"

// �����(�ֶ�)��Ϣ�Ľṹ�塣
struct st_columns
{
  char  colname[31];  // ������
  char  datatype[31]; // �е��������ͣ���Ϊnumber��date��char�����ࡣ
  int   collen;       // �еĳ��ȣ�number�̶�20��date�̶�19��char�ĳ����ɱ�ṹ������
  int   pkseq;        // ��������������ֶΣ���������ֶε�˳�򣬴�1��ʼ����������ȡֵ0��
};

// ��ȡ��ȫ�����к���������Ϣ���ࡣ
class CTABCOLS
{
public:
  CTABCOLS();

  int m_allcount;   // ȫ���ֶεĸ�����
  int m_pkcount;    // �����ֶεĸ�����
  int m_maxcollen;  // ȫ���������ĳ��ȣ������Ա�Ǻ������ӵġ�

  vector<struct st_columns> m_vallcols;  // ���ȫ���ֶ���Ϣ��������
  vector<struct st_columns> m_vpkcols;   // ��������ֶ���Ϣ��������

  char m_allcols[3001];  // ȫ�����ֶ����б����ַ�����ţ��м��ð�ǵĶ��ŷָ���
  char m_pkcols[301];    // �����ֶ����б����ַ�����ţ��м��ð�ǵĶ��ŷָ���

  void initdata();  // ��Ա������ʼ����

  // ��ȡָ�����ȫ���ֶ���Ϣ��
  bool allcols(connection *conn,char *tablename);
  
  // ��ȡָ����������ֶ���Ϣ��
  bool pkcols(connection *conn,char *tablename);
};






















#endif
