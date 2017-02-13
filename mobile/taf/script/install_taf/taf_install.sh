#!/bin/sh
base_dir=`dirname $0`
def_dir=/usr/local
user=`whoami`
Iflag=0
iflag=0
uflag=0
Localip=`/sbin/ifconfig eth1|grep "inet addr:"|cut -d: -f 2|cut -d" " -f1`
#用法
usage()
{
	cat <<EOF
USAGE:	$0 [OPTIONS]
FOR EXAMPLE 
	$0 -i -u "huanli"
	$0 -I
	$0 -d
DESCRIPTION
	1,必需用root或admin管理员权限运行
	2,脚本运行后，将安装一整套taf环境，包括框架服务和web管理平台，请耐心等待
	3,脚本将初化安装本地的mysql,resin,taf等，如有需要请备份原来的mysql和resin
	4,为了保证脚本正常运行，请确保该服务器环境是干净的，没有运行其它非系统的服务.
	5,如果安装oa认证方式，必须同时输入-i和-u参数
	6,支持32bit和64bit操作系统安装
OPTIONS
	-i 安装一整套taf环境，包括框架服务和web管理平台，以oa方式认证
	-u 如指定了-i以oa方式认证，初始化安装的时候必需指定用户ID为超级用户，如“huanli"，否则你将没有添加应用服务的权限
	-I 安装一整套taf环境，包括框架服务和web管理平台，以非oa方式认证，登陆用户名和密码均为superman
	-d 清理现有的环境，确保运行环境干净，将删除taf环境
	-h 帮忙信息
EOF
exit 1
}
#初始化安装环境
init_var()
{
	echo "[`date +%F\ %T`] starting init variables"
	if test $user != "root" -a $user != "admin"
	then
        	echo "[`date +%F\ %T`] error: you must run by user root or admin"
        	usage
	fi
	grep -q ^mqq: /etc/passwd
	if test $? = 1
	then
		useradd mqq -d $def_dir/app
		echo "[`date +%F\ %T`] add user mqq"
	fi
	grep -q ^mqq: /etc/group
	if test $? = 1
	then
		groupadd mqq
	fi
	export PATH=$PATH:/usr/local/mysql/bin:/usr/local/jdk/bin
	grep -q "passport.oa.com" /etc/hosts
	if test $? = 1
	then
		echo "10.6.12.14  passport.oa.com" >>/etc/hosts
	fi
	grep -q "indigo.oa.com" /etc/hosts
	if test $? = 1
	then
		echo "10.6.12.14  indigo.oa.com" >>/etc/hosts
	fi
	uname -a|grep -v grep|grep -q x86_64
	if test $? = 0
	then
		bitnum=64
		echo "[`date +%F\ %T`] the OS is $bitnum bit"
	else
		bitnum=32
		echo "[`date +%F\ %T`] the OS is $bitnum bit"
	fi
	echo "[`date +%F\ %T`] end init variables"
}
#mysql启动
mysql_start()
{
	ps -ef|grep -v grep|grep -q /usr/local/mysql-4.1.15
	if test $? = 0
	then
		echo "[`date +%F\ %T`] mysql haved running"
	else 
		$def_dir/mysql/bin/mysqld_safe --user=mqq &
        	sleep 1
        	ps -ef|grep -v grep|grep -q /usr/local/mysql-4.1.15
        	if test $? = 0
        	then
                	echo "[`date +%F\ %T`] start mysql success by mqq"     
        	else
                	echo "[`date +%F\ %T`] start mysql fail, please check /usr/local/mysql-4.1.15/mysqld.log"
			exit 1
		fi
        fi
        echo "/usr/local/mysql/lib/mysql" >> /etc/ld.so.conf
        ldconfig
}
#mysql安装
mysql_install()
{
	if test -s $def_dir/mysql/bin/mysql
	then
		echo "[`date +%F\ %T`] warn: $def_dir/mysql/bin is exists, skip install mysql"
		mysql_start
	else
		echo "[`date +%F\ %T`] starting install mysql"
		tar xzf $base_dir/mysql${bitnum}.tgz -C $def_dir
		ln -s $def_dir/mysql-4.1.15 $def_dir/mysql
		cp -f $def_dir/mysql/my.cnf /etc/my.cnf
		chown mqq.root /etc/my.cnf
		sed -i "s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" /etc/my.cnf
		if test ! -d /data/data_mysql-4.1.15
		then
			mkdir -p /data/data_mysql-4.1.15
		fi
		rm -rf /data/data_mysql-4.1.15/*
		$def_dir/mysql/bin/mysql_install_db >/dev/null
		chown mqq.mqq $def_dir/mysql/ -R
		chown mqq.mqq /data/data_mysql-4.1.15 -R
		chmod 764 $def_dir/mysql/* -R
		echo "[`date +%F\ %T`] success install mysql"
		mysql_start
	fi
}
#初始化DB内容
init_db()
{
	if test -d $def_dir/mysql/var/db_taf
	then
		echo "[`date +%F\ %T`] error: the DB db_taf is exists, break out, if you want to continue install, please del db db_taf"
		exit 1
	fi
	echo "[`date +%F\ %T`] start init DB data for taf"
	mysql -uroot -e"GRANT ALL PRIVILEGES ON *.* TO root@$Localip identified by '' with grant option"
	mysql -uroot -e"GRANT ALL PRIVILEGES ON *.* TO 'root'@'172.27.194.147' identified by '' WITH GRANT OPTION"
	mysql -uroot -e"create database db_system"
	mysql -uroot -e"create database db_taf"
	mysql -uroot -e"create database db_taf_stat"
	mysql -uroot -e"create database db_config"	
	tar xzf $base_dir/sql.tgz -C $base_dir
	sed -i "s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $base_dir/sql/db_taf.sql
	mysql -uroot db_system < $base_dir/sql/db_system.sql
	mysql -uroot db_taf < $base_dir/sql/db_taf.sql
	mysql -uroot db_taf_stat < $base_dir/sql/db_taf_stat.sql
	mysql -uroot db_config < $base_dir/sql/db_config.sql
	if test $uflag = 1
	then
		mysql -uroot db_system -e"insert into t_app_users (app,userid,lastuser) values ('taf','$userid','$userid');"
	fi
	mysqladmin flush-privileges
	mysqladmin flush-hosts
	echo "[`date +%F\ %T`] end init DB data"
}
#安装taf框架服务
taf_install()
{
	echo "[`date +%F\ %T`] starting install taf framework"
	mkdir -p /usr/local/app
        ps -ef|grep -v grep|grep -q bin/taf >/dev/null
	if test $? = 0
	then
		echo "[`date +%F\ %T`] error: some taf framework server is running, break out"
		exit 1
	fi
	if test -d $def_dir/app/taf
	then
		echo "[`date +%F\ %T`] $def_dir/app/taf is exists, mv to $def_dir/app/taf.`date +%F`"
		rm -rf $def_dir/app/taf.`date +%F`
		mv -f $def_dir/app/taf $def_dir/app/taf.`date +%F`
	fi
	tar xzf $base_dir/taf.tgz -C $def_dir/app/
	mkdir -p /data/tafnode/data
	chown mqq.mqq /data/tafnode/data
	echo "[`date +%F\ %T`] starting modify taf framework config"
	sed -i "s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $def_dir/app/taf/tafregistry/conf/registry.config.conf
	sed -i "s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $def_dir/app/taf/tafnode/util/execute.sh
	sed -i "s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $def_dir/app/taf/tafadmin/conf/admin.config.conf
	echo "[`date +%F\ %T`] success install taf framework"
	
}
#taf框架服务启动
taf_start()
{
	if test $bitnum = 32
	then
		$def_dir/app/taf/tafregistry/util/start.sh >/dev/null
	else
		if [ -d $def_dir/app/taf/tafregistry/util ]
		then
			OLDIR=`pwd`
			cd $def_dir/app/taf/tafregistry/util
			sudo -u mqq /bin/sh start.sh >/dev/null
			cd $OLDIR
		fi
	fi
	sleep 1
	ps -ef|grep -v grep|grep -q tafregistry >/dev/null
	if test $? = 0
	then
		echo "[`date +%F\ %T`] starting tafregistry success"
		if test $bitnum = 32
		then
			$def_dir/app/taf/tafnode/util/start.sh >/dev/null
		else
			sudo -u mqq $def_dir/app/taf/tafnode/util/start.sh >/dev/null	
		fi
		sleep 1
		ps -ef|grep -v grep|grep -q tafnode >/dev/null
		if test $? = 0
		then
			echo "[`date +%F\ %T`] starting tafnode success"
			for servername in tafconfig tafpatch tafnotify tafstat tafproperty taflog
			do
				$def_dir/app/taf/tafnode/util/start_server.sh taf.$servername >/dev/null
				sleep 1
				ps -ef|grep -v grep|grep -q bin/$servername >/dev/null
				if test $? = 0
				then
					echo "[`date +%F\ %T`] starting $servername success"
				else
					echo "[`date +%F\ %T`] starting $servername fail, please check log, continue"
				fi
			done

		else
			echo "[`date +%F\ %T`] starting tafnode fail, skip out"
			exit 1
		fi
	else
		echo "[`date +%F\ %T`] starting tafregistry fail, skip out"
		exit 1
	fi
}
#安装resin
resin_install()
{
	if test -s $def_dir/jdk/bin/java
	then
		echo "[`date +%F\ %T`] warn: $def_dir/jdk/bin is exists, skip install jdk"
	else
		echo "[`date +%F\ %T`] starting install jdk"
		tar xzf $base_dir/jdk${bitnum}.tgz -C $def_dir
		ln -s $def_dir/jdk1.6.0_12 $def_dir/jdk
		chown mqq.mqq $def_dir/jdk -R
	fi
	if test -s $def_dir/resin_taf/bin/httpd.sh
	then
		echo "[`date +%F\ %T`] error: $def_dir/resin_taf/bin is exists, skip install resin"
	else
		echo "[`date +%F\ %T`] starting install resin"
		tar xzf $base_dir/resin${bitnum}.tgz -C $def_dir
		ln -s $def_dir/resin-3.0.25_2 $def_dir/resin_taf
		chown mqq.mqq $def_dir/resin_taf
		chown mqq.mqq $def_dir/resin_taf/* -R
		confdir=$def_dir/resin_taf/conf
		sed -i "s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{2,3\}/$Localip/g" $confdir/resin.conf
	fi
}
#安装web管理平台
web_install()
{
	echo "[`date +%F\ %T`] starting install webapps"
	rm -rf $def_dir/resin_taf/webapps/system
	tar xzf $base_dir/system.tgz -C $def_dir/resin_taf/webapps/
	chown mqq.mqq $def_dir/resin_taf/* -R
	confdir=$def_dir/resin_taf/webapps/system/WEB-INF
	sed -i "/locator/ s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $confdir/classes/taf.conf 
	sed -i "/3306/ s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $confdir/classes/db-connections.xml
	sed -i "/locator/ s/[0-9]\{2,3\}\.[0-9]\{2,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}/$Localip/g" $confdir/src/taf.conf
	if test $Iflag = 1
        then
                sed -i '/login_type/ s/oa/not_oa/' $def_dir/resin_taf/webapps/system/WEB-INF/classes/system.xml
        fi
	echo "[`date +%F\ %T`] end install webapps"
	$def_dir/resin_taf/bin/mqq_start.sh >/dev/null
	/usr/local/resin-3.0.25_2/bin/httpd.sh stop >/dev/null
	/usr/local/resin-3.0.25_2/bin/httpd.sh start >/dev/null
	ps -ef|grep -v grep|grep -q resin_taf/bin
	if test $? = 0
	then
		echo "[`date +%F\ %T`] start resin success"
	else
		echo "[`date +%F\ %T`] start resin fail"
	fi
}
#清理已经安装的taf平台
install_clean()
{
	init_var
	ps -ef|grep -v grep|grep -q mysql
	if test $? = 0
	then
		mysqladmin -uroot shutdown >/dev/null
	fi
	if test -d $def_dir/mysql/bin
	then
		rm -f $def_dir/mysql
		rm -f /etc/my.cnf
		rm -rf $def_dir/mysql-4.1.15/var/*
		rm -rf $def_dir/mysql-4.1.15
		rm -rf $base_dir/sql
	fi
	echo "[`date +%F\ %T`] clean mysql success"
	ps -ef|grep -v grep|grep -q bin/taf
	if test $? = 0
	then
		ps -ef|grep -v grep|grep bin/taf|awk '{print $2}'|xargs kill -9
	fi
	if test -d $def_dir/app/taf
	then
		rm -rf $def_dir/app/taf
	fi
	echo "[`date +%F\ %T`] clean taf success"
	if test -d $def_dir/resin_taf/bin
	then
		$def_dir/resin_taf/bin/kill.sh
		rm -f $def_dir/resin_taf
		rm -rf $def_dir/resin-3.0.25_2
	fi
	echo "[`date +%F\ %T`] clean resin success"
	exit 0
}
############TAF安装#######
main()
{
echo "[`date +%F\ %T`] install begin"
init_var
mysql_install
init_db
taf_install
taf_start
resin_install
web_install
echo "[`date +%F\ %T`] install finished"
echo "[`date +%F\ %T`] please visit http://$Localip:8080"
exit 0
}
#############参数判断并运行##########
while getopts iu:Ihd i
do
        case $i in
		i)	iflag=1;;
		u)	uflag=1
			userid=$OPTARG;;
		I)	Iflag=1 
			main;;
                h)      usage;;
		d)	install_clean;;
                ?)      usage;;
        esac
done
if test $# = 0
then
	usage
fi
if test $iflag = 1 -a $uflag = 1
then
	main
else
	echo "[`date +%F\ %T`] error: -i -u must input together"
	usage
fi
