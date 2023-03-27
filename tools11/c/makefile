# 开发框架头文件路径
PUBLIC = -I/project/public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
PUBCPP = /project/public/_public.cpp

# mysql头文件存放路径
MYSQLINCL = -I/usr/include/mysql -I/project/public/db/mysql2

# mysql库文件保存路径
MYSQLLIB = -L/usr/lib64/mysql

# mysql链接库
MYSQLLIBS = -lmysqlclient

# 开发框架mysql的cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
MYSQLCPP = /project/public/db/mysql2/_mysql.cpp

#编译参数
CFLAGS = -g

all:procctl checkproc gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles fileserver tcpgetfiles execsql dminingmysql xmltodb syncupdate syncincrement syncincrementex

procctl:procctl.cpp
	g++ -o procctl procctl.cpp
	cp procctl ../bin/.
checkproc:checkproc.cpp
	g++ -g -o checkproc checkproc.cpp -I/project/public /project/public/_public.cpp -lm -lc
	cp checkproc ../bin/.
gzipfiles:gzipfiles.cpp
	g++ -g -o gzipfiles gzipfiles.cpp -I/project/public /project/public/_public.cpp -lm -lc
	cp gzipfiles ../bin/.
deletefiles:deletefiles.cpp
	g++ -g -o deletefiles deletefiles.cpp -I/project/public /project/public/_public.cpp -lm -lc
	cp deletefiles ../bin/.
ftpgetfiles:ftpgetfiles.cpp
	g++ -g -o ftpgetfiles ftpgetfiles.cpp -I/project/public /project/public/_public.cpp /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp ftpgetfiles ../bin/.

ftpputfiles:ftpputfiles.cpp
	g++ -g -o ftpputfiles ftpputfiles.cpp -I/project/public /project/public/_public.cpp /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp ftpputfiles ../bin/.

tcpputfiles:tcpputfiles.cpp
	g++ -g -o tcpputfiles tcpputfiles.cpp -I/project/public /project/public/_public.cpp /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp tcpputfiles ../bin/.

fileserver:fileserver.cpp
	g++ -g -o fileserver fileserver.cpp -I/project/public /project/public/_public.cpp /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp fileserver ../bin/.

tcpgetfiles:tcpgetfiles.cpp
	g++ -g -o tcpgetfiles tcpgetfiles.cpp -I/project/public /project/public/_public.cpp /project/public/libftp.a /project/public/_ftp.cpp -lm -lc
	cp tcpgetfiles ../bin/.

execsql:execsql.cpp
	g++ -g -o execsql execsql.cpp $(PUBLIC) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp execsql ../bin/.

dminingmysql:dminingmysql.cpp
	g++ -g -o dminingmysql dminingmysql.cpp $(PUBLIC) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp dminingmysql ../bin/.

xmltodb:xmltodb.cpp _tools.h _tools.cpp
	g++ -g -o xmltodb xmltodb.cpp _tools.cpp $(PUBLIC) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp xmltodb ../bin/.

syncupdate:syncupdate.cpp _tools.h _tools.cpp
	g++ -g -o syncupdate syncupdate.cpp _tools.cpp $(PUBLIC) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp syncupdate ../bin/.

syncincrement:syncincrement.cpp _tools.h _tools.cpp
	g++ -g -o syncincrement syncincrement.cpp _tools.cpp $(PUBLIC) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp syncincrement ../bin/.

syncincrementex:syncincrementex.cpp _tools.h _tools.cpp
	g++ -g -o syncincrementex syncincrementex.cpp _tools.cpp $(PUBLIC) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp syncincrementex ../bin/.

clean:
	rm -f procctl checkproc gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles fileserver tcpgetfiles syncupdate syncincrement syncincrementex
	rm -f execsql dminingmysql xmltodb
