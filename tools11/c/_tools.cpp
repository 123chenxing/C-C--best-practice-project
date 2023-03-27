#include "_tools.h"

// ��ȡ��ȫ�����к���������Ϣ���ࡣ
CTABCOLS::CTABCOLS()
{
  initdata();  // ���ó�Ա������ʼ��������
}

void CTABCOLS::initdata()  // ��Ա������ʼ����
{
  m_allcount=m_pkcount=0;
  m_vallcols.clear();
  m_vpkcols.clear();
  memset(m_allcols,0,sizeof(m_allcols));
  memset(m_pkcols,0,sizeof(m_pkcols));
}

// ��ȡָ�����ȫ���ֶ���Ϣ��
bool CTABCOLS::allcols(connection *conn,char *tablename)
{
  m_allcount=0;
  m_maxcollen=0;
  m_vallcols.clear();
  memset(m_allcols,0,sizeof(m_allcols));

  struct st_columns stcolumns;

  sqlstatement stmt;
  stmt.connect(conn);
  stmt.prepare("select lower(column_name),lower(data_type),character_maximum_length from information_schema.COLUMNS where table_name=:1");
  stmt.bindin(1,tablename,30);
  stmt.bindout(1, stcolumns.colname,30);
  stmt.bindout(2, stcolumns.datatype,30);
  stmt.bindout(3,&stcolumns.collen);

  if (stmt.execute()!=0) return false;

while (true)
{
  memset(&stcolumns,0,sizeof(struct st_columns));

  if (stmt.next()!=0) break;

  // �е��������ͣ���Ϊnumber��date��char�����ࡣ
  if (strcmp(stcolumns.datatype,"char")==0)    strcpy(stcolumns.datatype,"char");
  if (strcmp(stcolumns.datatype,"varchar")==0) strcpy(stcolumns.datatype,"char");

  if (strcmp(stcolumns.datatype,"datetime")==0)  strcpy(stcolumns.datatype,"date");
  if (strcmp(stcolumns.datatype,"timestamp")==0) strcpy(stcolumns.datatype,"date");

  if (strcmp(stcolumns.datatype,"tinyint")==0)   strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"smallint")==0)  strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"mediumint")==0) strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"int")==0)       strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"integer")==0)   strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"bigint")==0)    strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"numeric")==0)   strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"decimal")==0)   strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"float")==0)     strcpy(stcolumns.datatype,"number");
  if (strcmp(stcolumns.datatype,"double")==0)    strcpy(stcolumns.datatype,"number");

  // ���ҵ������Ҫ�������޸�����Ĵ��룬���ӶԸ����������͵�֧�֡�
  // ����ֶε��������Ͳ��������г������У���������
  if ( (strcmp(stcolumns.datatype,"char")!=0) &&
  (strcmp(stcolumns.datatype,"date")!=0) &&
  (strcmp(stcolumns.datatype,"number")!=0) ) continue;
    
  // ����ֶ�������date���ѳ�������Ϊ19��yyyy-mm-dd hh:mi:ss
  if (strcmp(stcolumns.datatype,"date")==0) stcolumns.collen=19;
    
  // ����ֶ�������number���ѳ�������Ϊ20��
  if (strcmp(stcolumns.datatype,"number")==0) stcolumns.collen=20;
    
  strcat(m_allcols,stcolumns.colname); strcat(m_allcols,",");
    
  m_vallcols.push_back(stcolumns);

  if (m_maxcollen<stcolumns.collen) m_maxcollen=stcolumns.collen;
    
  m_allcount++;
 }
    
  // ɾ��m_allcols���һ������Ķ��š�
  if (m_allcount>0) m_allcols[strlen(m_allcols)-1]=0;
    
  return true;
}

// ��ȡָ����������ֶ���Ϣ��
bool CTABCOLS::pkcols(connection *conn,char *tablename)
{
  m_pkcount=0;
  memset(m_pkcols,0,sizeof(m_pkcols));
  m_vpkcols.clear();

  struct st_columns stcolumns;

  sqlstatement stmt;
  stmt.connect(conn);
  stmt.prepare("select lower(column_name),seq_in_index from information_schema.STATISTICS where table_name=:1 and index_name='primary' order by seq_in_index");
  stmt.bindin(1,tablename,30);
  stmt.bindout(1, stcolumns.colname,30);
  stmt.bindout(2,&stcolumns.pkseq);

  if (stmt.execute() != 0) return false;

  while (true)
  {
    memset(&stcolumns,0,sizeof(struct st_columns));

    if (stmt.next() != 0) break;

    strcat(m_pkcols,stcolumns.colname); strcat(m_pkcols,",");

    m_vpkcols.push_back(stcolumns);

    m_pkcount++;
   }

  if (m_pkcount>0) m_pkcols[strlen(m_pkcols)-1]=0;    // ɾ��m_pkcols���һ������Ķ��š�

  return true;
}
                                                        
