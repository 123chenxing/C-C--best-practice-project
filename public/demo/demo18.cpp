/*
 *  ��������demo18.cpp���˳�����ʾ�������������ʾMatchStr������ʹ�á�
 *  ���ߣ������
*/
#include "../_public.h"

int main()
{
  char filename[301];  
  STRCPY(filename,sizeof(filename),"_public.h");
  
  // ���´��뽫���yes��
  if (MatchStr(filename,"*.h,*.cpp")==true) printf("yes\n");
  else printf("no\n");

  // ���´��뽫���yes��
  if (MatchStr(filename,"*.H")==true) printf("yes\n");
  else printf("no\n");

  // ���´��뽫���no��
  if (MatchStr(filename,"*.cpp")==true) printf("yes\n");
  else printf("no\n");

  return 0;
}

