##############################################################
#启动数据中心后台服务程序的脚本
##############################################################

#检查服务程序是否超时，配置在/etc/rc.local中由root用户执行
#/project/tools11/bin/procctl 30 /project/tools11/bin/checkproc



#压缩数据中心后台服务程序的备份日志
/project/tools11/bin/procctl 300 /project/tools11/bin/gzipfiles /log/idc "*.log.20*" 0.04


#生成用于测试的全国气象站点观测的分钟数据
/project/tools11/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv


#清理原始的全国气象站点观测的分钟数据目录/tmp/idc/surfdata中的历史数据文件
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /tmp/idc/surfdata "*" 0.04


# 采集全国气象站点观测的分钟数据
/project/tools11/bin/procctl 30 /project/tools11/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>chenguan</username><password>13251009668</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"


#上传全国气象站点观测的分钟数据的xml文件

/project/tools11/bin/procctl 30 /project/tools11/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>chenguan</username><password>13251009668</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputest</remotepath><matchname>SURF_ZH*.JSON </matchname><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>"

#清理采集的全国气象站点观测的分钟数据目录/idcdata/surfdata中的历史数据文件
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /idcdata/surfdata "*" 0.04

#清理采集的全国气象站点观测的分钟数据目录/tmp/ftpputest中的历史数据文件
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /tmp/ftpputest "*" 0.04

# 文件传输的服务端程序。
/project/tools11/bin/procctl 10 /project/tools11/bin/fileserver 5007 /log/idc/fileserver.log

# 把目录/tmp/ftpputest中的文件上传到/tmp/tcpputest目录中。
/project/tools11/bin/procctl 20 /project/tools11/bin/tcpputfiles /log/idc/tcpputfiles_surfdata.log "<ip>127.0.0.1</ip><port>5007</port><ptype>1</ptype><clientpath>/tmp/ftpputest</clientpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><srvpath>/tmp/tcpputest</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>"

# 把目录/tmp/tcpputest中的文件下载到/tmp/tcpgetest目录中。
/project/tools11/bin/procctl 20 /project/tools11/bin/tcpgetfiles /log/idc/tcpgetfiles_surfdata.log "<ip>127.0.0.1</ip><port>5007</port><ptype>1</ptype><srvpath>/tmp/tcpputest</srvpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcpgetest</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>"

# 清理采集的全国气象站点观测的分钟数据目录/tmp/tcpgetest中的历史数据文件。
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /tmp/tcpgetest "*" 0.04

# 把全国站点参数数据保存到数据库表中，如果站点不存在则插入，站点已存在则更新。 
/project/tools11/bin/procctl 120 /project/idc11/bin/obtcodetodb /project/idc11/ini/stcode.ini "127.0.0.1,root,13251009668,test2,3306" utf8 /log/idc/obtcodetodb.log

# 把全国站点分钟观测数据保存到数据库的T_ZHOBTMIND表中，数据只插入，不更新。
/project/tools11/bin/procctl 10 /project/idc11/bin/obtmindtodb /idcdata/surfdata "127.0.0.1,root,13251009668,test2,3306" utf8 /log/idc/obtmindtodb.log

# 清理T_ZHOBTMIND表中120分之前的数据，防止磁盘空间被撑满。
/project/tools11/bin/procctl 120 /project/tools11/bin/execsql /project/idc/sql/cleardata.sql"127.0.0.1,root,13251009668,test2,3306" utf8 /log/idc/execsql.log 

# 每隔1小时把T_ZHOBTCODE表中全部的数据抽取出来。
/project/tools11/bin/procctl 3600 /project/tools11/bin/dminingmysql /log/idc/dminingmysql_ZHOBTCODE.log "<connstr>127.0.0.1,root,13251009668,test2,3306</connstr><charset>utf8</charset><selectsql>select obtid,cityname,provname,lat,lon,height from T_ZHOBTCODE</selectsql><fieldstr>obtid,cityname,provname,lat,lon,height</fieldstr><fieldlen>10,30,30,10,10,10</fieldlen><bfilename>ZHOBTCODE</bfilename><efilename>HYCZ</efilename><outpath>/idcdata/dmindata</outpath><timeout>30</timeout><pname>dminingmysql_ZHOBTCODE</pname>"

# 每30秒从T_ZHOBTMIND表中增量抽取出来。
       /project/tools11/bin/procctl   30 /project/tools11/bin/dminingmysql /log/idc/dminingmysql_ZHOBTMIND.log "<connstr>127.0.0.1,root,13251009668,test2,3306</connstr><charset>utf8</charset><selectsql>select obtid,date_format(ddatetime,'%%Y-%%m-%%d %%H:%%i:%%s'),t,p,u,wd,wf,r,vis,keyid from T_ZHOBTMIND where keyid>:1 and ddatetime>timestampadd(minute,-120,now())</selectsql><fieldstr>obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>10,19,8,8,8,8,8,8,8,15</fieldlen><bfilename>ZHOBTMIND</bfilename><efilename>HYCZ</efilename><outpath>/idcdata/dmindata</outpath><incfield>keyid</incfield><timeout>30</timeout><pname>dminingmysql_ZHOBTMIND_HYCZ</pname><maxcount>1000</maxcount><connstr1>127.0.0.1,root,13251009668,test2,3306</connstr1>"

# 清理/idcdata/dmindata目录中文件，防止把空间撑满。
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /idcdata/dmindata "*" 0.04

# 把/idcdata/dmindata目录中的xml发送到/idcdata/xmltodb/vip1，交由xmltodb程序去入库。
/project/tools11/bin/procctl 20 /project/tools11/bin/tcpputfiles /log/idc/tcpputfiles_dmindata.log "<ip>127.0.0.1</ip><port>5007</port><ptype>1</ptype><clientpath>/idcdata/dmindata</clientpath><andchild>false</andchild><matchname>*.XML</matchname><srvpath>/idcdata/xmltodb/vip1</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_dmindata</pname>"

# 把/idcdata/xmltodb/vip1目录中的xml文件入库。
/project/tools11/bin/procctl 10 /project/tools11/bin/xmltodb /log/idc/xmltodb_vip1.log "<connstr>127.0.0.1,root,13251009668,test2,3306</connstr><charset>utf8</charset><inifilename>/project/idc11/ini/xmltodb.xml</inifilename><xmlpath>/idcdata/xmltodb/vip1</xmlpath><xmlpathbak>/idcdata/xmltodb/vip1bak</xmlpathbak><xmlpatherr>/idcdata/xmltodb/vip1err</xmlpatherr><timetvl>5</timetvl><timeout>50</timeout><pname>xmltodb_vip1</pname>"

# 清理/idcdata/xmltodb/vip1bak和/idcdata/xmltodb/vip1err目录中文件，防止把空间撑满。
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /idcdata/xmltodb/vip1bak "*" 0.04
/project/tools11/bin/procctl 300 /project/tools11/bin/deletefiles /idcdata/xmltodb/vip1err "*" 0.04

# 采用全表刷新同步的方法，把表T_ZHOBTCODE1中的数据同步到表T_ZHOBTCODE2
/project/tools11/bin/procctl 10 /project/tools11/bin/syncupdate /log/idc/syncupdate_ZHOBTCODE2.log "<localconnstr>192.168.244.128,root,13251009668,test2,3306</localconnstr><charset>utf8</charset><fedtname>LK_ZHOBTCODE1</fedtname><localtname>T_ZHOBTCODE2</localtname><remotecols>obtid,cityname,provname,lat,lon,height/10,upttime,keyid</remotecols><localcols>stid,cityname,provname,lat,lon,altitude,upttime,keyid</localcols><synctype>1</synctype><timeout>50</timeout><pname>syncupdate_ZHOBTCODE2</pname>"

# 采用分批同步的方法，把表T_ZHOBTCODE1中obtid like '54%'的记录同分布到T_ZHOBTCODE3
/project/tools11/bin/procctl 10 /project/tools11/bin/syncupdate /log/idc/syncupdate_ZHOBTCODE3.log "<localconnstr>192.168.244.128,root,13251009668,test2,3306</localconnstr><charset>utf8</charset><fedtname>LK_ZHOBTCODE1</fedtname><localtname>T_ZHOBTCODE3</localtname><remotecols>obtid,cityname,provname,lat,lon,height/10,upttime,keyid</remotecols><localcols>stid,cityname,provname,lat,lon,altitude,upttime,keyid</localcols><where>where obtid like '54%%%%'</where><synctype>2</synctype><remoteconnstr>192.168.244.128,root,13251009668,test2,3306</remoteconnstr><remotetname>T_ZHOBTCODE1</remotetname><remotekeycol>obtid</remotekeycol><localkeycol>obtid</localkeycol><maxcount>10</maxcount><timeout>50</timeout><pname>syncupdate_ZHOBTCODE3</pname>"


# 采用增量同步的方法，把表T_ZHOBTMIND1中全部的记录同步到表T_ZHOBTMIND2
/project/tools11/bin/procctl 10 /project/tools11/bin/syncincrement /log/idc/syncincrement_ZHOBTMIND2.log "<localconnstr>192.168.244.128,root,13251009668,test2,3306</localconnstr><remoteconnstr>192.168.244.128,root,13251009668,test2,3306</remoteconnstr><charset>utf8</charset><remotetname>T_ZHOBTMIND1</remotetname><fedtname>LK_ZHOBTMIND1</fedtname><localtname>T_ZHOBTMIND2</localtname><remotecols>obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</remotecols><localcols>stid,ddatetime,t,p,u,wd,wf,r,vis,upttime,recid</localcols><remotekeycol>keyid</remotekeycol><localkeycol>recid</localkeycol><maxcount>300</maxcount><timetvl>2</timetvl><timeout>50</timeout><pname>syncincrement_ZHOBTMIND2</pname>"

# 采用增量同步的方法，把表T_ZHOBTMIND1中obtid like '54%'的记录同步到表T_ZHOBTMIND3
/project/tools11/bin/procctl 10 /project/tools11/bin/syncincrement /log/idc/syncincrement_ZHOBTMIND3.log "<localconnstr>192.168.244.128,root,13251009668,test2,3306</localconnstr><remoteconnstr>192.168.244.128,root,13251009668,test2,3306</remoteconnstr><charset>utf8</charset><remotetname>T_ZHOBTMIND1</remotetname><fedtname>LK_ZHOBTMIND1</fedtname><localtname>T_ZHOBTMIND3</localtname><remotecols>obtid,ddatetime,t,p,u,wd,wf,r,vis,upttime,keyid</remotecols><localcols>stid,ddatetime,t,p,u,wd,wf,r,vis,upttime,recid</localcols><where>and obtid like '54%%%%'</where><remotekeycol>keyid</remotekeycol><localkeycol>recid</localkeycol><maxcount>300</maxcount><timetvl>2</timetvl><timeout>50</timeout><pname>syncincrement_ZHOBTMIND3</pname>"

