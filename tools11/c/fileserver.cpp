/*
 * ��������fileserver.cpp���ļ�����ķ���ˡ�
 * ���ߣ��¹�
*/
#include "_public.h"

// �������еĲ����ṹ�塣
struct st_arg
{
  int  clienttype;          // �ͻ������ͣ�1-�ϴ��ļ���2-�����ļ���
  char ip[31];              // ����˵�IP��ַ��
  int  port;                // ����˵Ķ˿ڡ�
  int  ptype;               // �ļ��ɹ������Ĵ���ʽ��1-ɾ���ļ���2-�ƶ�������Ŀ¼��
  char clientpath[301];     // �ͻ����ļ���ŵĸ�Ŀ¼��
  bool andchild;            // �Ƿ��������Ŀ¼���ļ���true-�ǣ�false-��
  char matchname[301];      // �������ļ�����ƥ�������"*.TXT,*.XML"��
  char srvpath[301];        // ������ļ���ŵĸ�Ŀ¼��
  char srvpathbak[301];     // ������ļ���ŵĸ�Ŀ¼��
  
  int  timetvl;             // ɨ��Ŀ¼�ļ���ʱ��������λ���롣
  int  timeout;             // ���������ĳ�ʱʱ�䡣
  char pname[51];           // ��������������"tcpgetfiles_��׺"�ķ�ʽ��
} starg;

// ��xml����������starg�ṹ�С�
bool _xmltoarg(char *strxmlbuffer);

CLogFile logfile;      // ��������������־��
CTcpServer TcpServer;  // ��������˶���

void FathEXIT(int sig);  // �������˳�������
void ChldEXIT(int sig);  // �ӽ����˳�������


bool ActiveTest();  //������
char strrecvbuffer[1024];    //���ͱ��ĵ�buffer��
char strsendbuffer[1024];    //���ձ��ĵ�buffer��

//�ļ����ص���������ִ��һ���ļ����ص�����
bool _tcpputfiles();
bool bcontinue=true;    //�������_tcpputfiles�������ļ���bcontinueΪtrue,��ʼ��Ϊtrue��

//���ļ������ݷ��͸��Զˡ�
bool SendFile(const int sockfd,const char *filename,const int filesize);
//ɾ������ת�汾�ص��ļ���
bool AckMessage(const char *strrecvbuffer);

// ��¼ҵ��������
bool ClientLogin();

// �ϴ��ļ�����������
void RecvFilesMain();

// �����ļ�����������
void SendFilesMain();
// �����ϴ��ļ������ݡ�
bool RecvFile(const int sockfd,const char *filename,const char *mtime,int filesize);
CPActive PActive;   //����������
int main(int argc,char *argv[])
{
  if (argc!=3)
  {
 printf("Using:./fileserver port logfile\n");
 printf("Example:./fileserver 5007 /log/idc/fileserver.log\n"); return -1;
 printf("/project/tools11/bin/procctl 10 /project/tools11/bin/fileserver 5007 /log/idc/fileserver.log\n"); 
 return -1;
  }

  // �ر�ȫ�����źź����������
  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ����
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ
  CloseIOAndSignal(); signal(SIGINT,FathEXIT); signal(SIGTERM,FathEXIT);

  if (logfile.Open(argv[2],"a+")==false) { printf("logfile.Open(%s) failed.\n",argv[2]); return -1; }

  // ����˳�ʼ����
  if (TcpServer.InitServer(atoi(argv[1]))==false)
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); return -1;
  }

  while (true)
  {
    // �ȴ��ͻ��˵���������
    if (TcpServer.Accept()==false)
    {
      logfile.Write("TcpServer.Accept() failed.\n"); FathEXIT(-1);
    }

    logfile.Write("�ͻ��ˣ�%s�������ӡ�\n",TcpServer.GetIP());

 
    if (fork()>0) { TcpServer.CloseClient(); continue; }  // �����̼����ص�Accept()��
   
    // �ӽ������������˳��źš�
    signal(SIGINT,ChldEXIT); signal(SIGTERM,ChldEXIT);

    TcpServer.CloseListen();

    // �ӽ�����ͻ��˽���ͨѶ������ҵ��

    // �����¼�ͻ��˵ĵ�¼���ġ�
    if (ClientLogin()==false) ChldEXIT(-1);

    // ���clienttype==1�������ϴ��ļ�����������
    if (starg.clienttype==1) RecvFilesMain();

    // ���clienttype==2�����������ļ�����������
    if (starg.clienttype==2) SendFilesMain();
    ChldEXIT(0);
  }
}

// �������˳�������
void FathEXIT(int sig)  
{
  // ���´�����Ϊ�˷�ֹ�źŴ�������ִ�еĹ����б��ź��жϡ�
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("�������˳���sig=%d��\n",sig);

  TcpServer.CloseListen();    // �رռ�����socket��

  kill(0,15);     // ֪ͨȫ�����ӽ����˳���

  exit(0);
}

// �ӽ����˳�������
void ChldEXIT(int sig)  
{
  // ���´�����Ϊ�˷�ֹ�źŴ�������ִ�еĹ����б��ź��жϡ�
  signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);

  logfile.Write("�ӽ����˳���sig=%d��\n",sig);

  TcpServer.CloseClient();    // �رտͻ��˵�socket��

  exit(0);
}

// ��¼��
bool ClientLogin()
{
  memset(strrecvbuffer,0,sizeof(strrecvbuffer));
  memset(strsendbuffer,0,sizeof(strsendbuffer));

  if (TcpServer.Read(strrecvbuffer,20)==false)
  {
    logfile.Write("TcpServer.Read() failed.\n"); return false;
  }
 // logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);

  // �����ͻ��˵�¼���ġ�
  _xmltoarg(strrecvbuffer);

  if ( (starg.clienttype!=1) && (starg.clienttype!=2) )
    strcpy(strsendbuffer,"failed");
  else
    strcpy(strsendbuffer,"ok");

  if (TcpServer.Write(strsendbuffer)==false)
  {
    logfile.Write("TcpServer.Write() failed.\n"); return false;
  }

  logfile.Write("%s login %s.\n",TcpServer.GetIP(),strsendbuffer);
  
  return true;
}

// ��xml����������starg�ṹ��
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  // ����Ҫ�Բ������Ϸ����жϣ��ͻ����Ѿ��жϹ��ˡ�
  GetXMLBuffer(strxmlbuffer,"clienttype",&starg.clienttype);
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath);
  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);
  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname);
  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath);
  GetXMLBuffer(strxmlbuffer,"srvpathbak",starg.srvpathbak);

  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);
  if (starg.timetvl>30) starg.timetvl=30;

  GetXMLBuffer(strxmlbuffer,"timeout",&starg.timeout);
  if (starg.timeout<50) starg.timeout=50;

  GetXMLBuffer(strxmlbuffer,"pname",starg.pname,50);
  strcat(starg.pname,"_srv");

  return true;
}

// �ϴ��ļ�����������
void RecvFilesMain()
{
 PActive.AddPInfo(starg.timeout,starg.pname);
  while (true)
  {
    memset(strsendbuffer,0,sizeof(strsendbuffer));
    memset(strrecvbuffer,0,sizeof(strrecvbuffer));
    PActive.UptATime();
    // ���տͻ��˵ı��ġ�
    // �ڶ���������ȡֵ�������starg.timetvl��С��starg.timeout��
    if (TcpServer.Read(strrecvbuffer,starg.timetvl+10)==false)
    {
      logfile.Write("TcpServer.Read() failed.\n"); return;
    }
   // logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);

    // �����������ġ�
    if (strcmp(strrecvbuffer,"<activetest>ok</activetest>")==0)
    {
      strcpy(strsendbuffer,"ok");
      // logfile.Write("strsendbuffer=%s\n",strsendbuffer);
      if (TcpServer.Write(strsendbuffer)==false)
      {
        logfile.Write("TcpServer.Write() failed.\n"); return;
      }
    }

    // �����ϴ��ļ��������ġ�
    if (strncmp(strrecvbuffer,"<filename>",10)==0)
    {
      // �����ϴ��ļ������ĵ�xml��
      char clientfilename[301];  memset(clientfilename,0,sizeof(clientfilename));
      char mtime[21];            memset(mtime,0,sizeof(mtime));
      int  filesize=0;
      GetXMLBuffer(strrecvbuffer,"filename",clientfilename,300);
      GetXMLBuffer(strrecvbuffer,"mtime",mtime,19);
      GetXMLBuffer(strrecvbuffer,"size",&filesize);

      // �ͻ��˺ͷ�����ļ���Ŀ¼�ǲ�һ���ģ����´������ɷ���˵��ļ�����
      // ���ļ����е�clientpath�滻��srvpath��ҪС�ĵ���������
      char serverfilename[301];  memset(serverfilename,0,sizeof(serverfilename));
      strcpy(serverfilename,clientfilename);
      UpdateStr(serverfilename,starg.clientpath,starg.srvpath,false);

      // �����ϴ��ļ������ݡ�
      logfile.Write("recv %s(%d) ...",serverfilename,filesize);
      if (RecvFile(TcpServer.m_connfd,serverfilename,mtime,filesize)==true)
      {
        logfile.WriteEx("ok.\n");
        SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><result>ok</result>",clientfilename);
      }
      else
      {
        logfile.WriteEx("failed.\n");
        SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><result>failed</result>",clientfilename);
      }

      // �ѽ��ս�����ظ��Զˡ�
      // logfile.Write("strsendbuffer=%s\n",strsendbuffer);
      if (TcpServer.Write(strsendbuffer)==false)
      {
        logfile.Write("TcpServer.Write() failed.\n"); return;
      }
    }
  }
}

// �����ϴ��ļ������ݡ�
bool RecvFile(const int sockfd,const char *filename,const char *mtime,int filesize)
{
  // ������ʱ�ļ�����
  char strfilenametmp[301];
  SNPRINTF(strfilenametmp,sizeof(strfilenametmp),300,"%s.tmp",filename);

  int  totalbytes=0;        // �ѽ����ļ������ֽ�����
  int  onread=0;            // ���δ�����յ��ֽ�����
  char buffer[1000];        // �����ļ����ݵĻ�������
  FILE *fp=NULL;

  // ������ʱ�ļ���
  if ( (fp=FOPEN(strfilenametmp,"wb"))==NULL ) return false;

  while (true)
  {
    memset(buffer,0,sizeof(buffer));

    // ���㱾��Ӧ�ý��յ��ֽ�����
    if (filesize-totalbytes>1000) onread=1000;
    else onread=filesize-totalbytes;

    // �����ļ����ݡ�
    if (Readn(sockfd,buffer,onread)==false) { fclose(fp); return false; }

    // �ѽ��յ�������д���ļ���
    fwrite(buffer,1,onread,fp);

    // �����ѽ����ļ������ֽ���������ļ������꣬����ѭ����
    totalbytes=totalbytes+onread;

    if (totalbytes==filesize) break;
  }

  // �ر���ʱ�ļ���
  fclose(fp);

  // �����ļ���ʱ�䡣
  UTime(strfilenametmp,mtime);

  // ����ʱ�ļ�RENAMEΪ��ʽ���ļ���
  if (RENAME(strfilenametmp,filename)==false) return false;

  return true;
}
// �����ļ�����������
void SendFilesMain(){
 PActive.AddPInfo(starg.timeout,starg.pname);
 while (true)
  {
 // �����ļ����ص���������ִ��һ���ļ����ص�����
 if (_tcpputfiles()==false) { logfile.Write("_tcpputfiles() failed.\n"); return; }
 if(bcontinue==false)
 {
 sleep(starg.timetvl);
 if (ActiveTest()==false) break;
  }
 PActive.UptATime();
 }

}

// ������ 
 bool ActiveTest()
 {
memset(strsendbuffer,0,sizeof(strsendbuffer));
memset(strrecvbuffer,0,sizeof(strrecvbuffer));

SPRINTF(strsendbuffer,sizeof(strsendbuffer),"<activetest>ok</activetest>");
// logfile.Write("���ͣ�%s\n",strsendbuffer);
 if (TcpServer.Write(strsendbuffer)==false) return false; // �����˷��������ġ�

 if (TcpServer.Read(strrecvbuffer,20)==false) return false; // ���շ���˵Ļ�Ӧ���ġ�
// logfile.Write("���գ�%s\n",strrecvbuffer);

return true;
   }

// �ļ����ص���������ִ��һ���ļ����ص�����
 bool _tcpputfiles()
 {
  CDir Dir;

   // ����OpenDir()��starg.srvpathĿ¼��
   if (Dir.OpenDir(starg.srvpath,starg.matchname,10000,starg.andchild)==false)
 {
   logfile.Write("Dir.OpenDir(%s) ʧ�ܡ�\n",starg.srvpath); return false;
       }
   int delayed=0;  //δ�յ��Զ�ȷ�ϱ��ĵ��ļ�������
   int buflen=0;   //���ڴ��strrecvbuffer�ĳ��ȡ�
   bcontinue=false;
   while (true)
          {
   memset(strsendbuffer,0,sizeof(strsendbuffer));
   memset(strrecvbuffer,0,sizeof(strrecvbuffer));

   // ����Ŀ¼�е�ÿ���ļ�������ReadDir()��ȡһ���ļ�����
   if (Dir.ReadDir()==false) break;
   bcontinue=true;
 // ���ļ������޸�ʱ�䡢�ļ���С��ɱ��ģ����͸��Զˡ�
      SNPRINTF(strsendbuffer,sizeof(strsendbuffer),1000,"<filename>%s</filename><mtime>%s</mtime><size>%d</size>",Dir.m_FullFileName,Dir.m_ModifyTime,Dir.m_FileSize);
 
  // logfile.Write("strsendbuffer=%s\n",strsendbuffer);
    if (TcpServer.Write(strsendbuffer)==false)
              {
   logfile.Write("TcpServer.Write() failed.\n"); return false;
      }
 
    // ���ļ������ݷ��͸��Զˡ�
   logfile.Write("send %s(%d) ...",Dir.m_FullFileName,Dir.m_FileSize);
 if (SendFile(TcpServer.m_connfd,Dir.m_FullFileName,Dir.m_FileSize)==true)
               {
   logfile.WriteEx("ok.\n"); delayed++;
      }
        else
          {
   logfile.WriteEx("failed.\n"); TcpServer.CloseClient();return false;
            }
   PActive.UptATime();

 // ���նԶ˵�ȷ�ϱ��ġ�
   while(delayed>0){
    memset(strrecvbuffer,0,sizeof(strrecvbuffer));
      if(TcpRead(TcpServer.m_connfd,strrecvbuffer,&buflen,-1)==false)
      break;
     // logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);
 
     // ɾ������ת�汾�ص��ļ���
        delayed--;
       AckMessage(strrecvbuffer);
                       }
              }
 
      // �������նԶ˵�ȷ�ϱ��ġ�
         while(delayed>0){
        memset(strrecvbuffer,0,sizeof(strrecvbuffer));
       if(TcpRead(TcpServer.m_connfd,strrecvbuffer,&buflen,10)==false)
       break;
      // logfile.Write("strrecvbuffer=%s\n",strrecvbuffer);
 
     // ɾ������ת�汾�ص��ļ���
     delayed--;
         AckMessage(strrecvbuffer);
                   }
        return true;
          }
   // ���ļ������ݷ��͸��Զˡ�
    bool SendFile(const int sockfd,const char *filename,const int filesize)
    {
     int  onread=0;        // ÿ�ε���freadʱ�����ȡ���ֽ����� 
     int  bytes=0;         // ����һ��fread���ļ��ж�ȡ���ֽ�����
     char buffer[1000];    // ��Ŷ�ȡ���ݵ�buffer��
     int  totalbytes=0;    // ���ļ����Ѷ�ȡ���ֽ�������
     FILE *fp=NULL;
   
       // ��"rb"��ģʽ���ļ���
      if ( (fp=fopen(filename,"rb"))==NULL )  return false;
   
     while (true)
                {
     memset(buffer,0,sizeof(buffer));
   
    // ���㱾��Ӧ�ö�ȡ���ֽ��������ʣ������ݳ���1000�ֽڣ��ʹ����1000�ֽڡ�
       if (filesize-totalbytes>1000) onread=1000;
      else onread=filesize-totalbytes;                                                           
// ���ļ��ж�ȡ���ݡ�
   bytes=fread(buffer,1,onread,fp);

 // �Ѷ�ȡ�������ݷ��͸��Զˡ�
       if (bytes>0)
                 {
       if (Writen(sockfd,buffer,bytes)==false) { fclose(fp); return false; }
                         }

          // �����ļ��Ѷ�ȡ���ֽ�����������ļ��Ѷ��꣬����ѭ����
       totalbytes=totalbytes+bytes;

       if (totalbytes==filesize) break;
                   }

       fclose(fp);

       return true;
         }

// ɾ������ת�汾�ص��ļ���
 bool AckMessage(const char *strrecvbuffer)
 {
   char filename[301];
   char result[11];

   memset(filename,0,sizeof(filename));
   memset(result,0,sizeof(result));

   GetXMLBuffer(strrecvbuffer,"filename",filename,300);
   GetXMLBuffer(strrecvbuffer,"result",result,10);

   // �������˽����ļ����ɹ���ֱ�ӷ��ء�
   if (strcmp(result,"ok")!=0) return true;

   // ptype==1��ɾ���ļ���
   if (starg.ptype==1)
        {
   if (REMOVE(filename)==false) { logfile.Write("REMOVE(%s) failed.\n",filename); return false; }
                 }

    // ptype==2���ƶ�������Ŀ¼��
    if (starg.ptype==2)
              {
    // ����ת���ı���Ŀ¼�ļ�����
    char bakfilename[301];
    STRCPY(bakfilename,sizeof(bakfilename),filename);
    UpdateStr(bakfilename,starg.srvpath,starg.srvpathbak,false);
    if (RENAME(filename,bakfilename)==false)
   { logfile.Write("RENAME(%s,%s) failed.\n",filename,bakfilename); return false; }
                               }
    return true;
                      }

                                                                                                                
