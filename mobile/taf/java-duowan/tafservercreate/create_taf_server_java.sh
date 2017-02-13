#!/bin/sh
if [[ $# -ne 3 ]]
then
	echo "[usage] ./create_taf_server_java.sh <app> <server> <servant>"
	exit
fi

app=$1
server=$2
servant=$3
package=$4
fromhome=/usr/local/taf/taf_server_java
tohome=$app/$server
applowwer=`echo $app | tr '[:upper:]' '[:lower:]'`
serverlowwer=`echo $server | tr '[:upper:]' '[:lower:]'`
servantlowwer=`echo $servant | tr '[:upper:]' '[:lower:]'`

mkdir -p $tohome/src/com/qq/$applowwer/$serverlowwer/$servantlowwer
cp -r $fromhome/lib $tohome
cp -r $fromhome/classes $tohome
cat $fromhome/makefile | sed "s/_APP_/$app/g; s/_SERVER_/$server/g" > $tohome/makefile
cat $fromhome/build.sh | sed "s/_SERVERLOWWER_/$serverlowwer/g; s/_APPLOWWER_/$applowwer/g; s/_SERVANT_/$servant/g" > $tohome/build.sh
cat $fromhome/genjce.sh | sed "s/_SERVERLOWWER_/$serverlowwer/g; s/_APPLOWWER_/$applowwer/g; " > $tohome/genjce.sh
cat $fromhome/jarjce.sh | sed "s/_APP_/$app/g; s/_SERVER_/$server/g; s/_SERVANT_/$servant/g; s/_APPLOWWER_/$applowwer/g; s/_SERVERLOWWER_/$serverlowwer/g; s/_SERVANTLOWWER_/$servantlowwer/g" > $tohome/jarjce.sh
cat $fromhome/src/_SERVANT_.jce | sed "s/_SERVANTLOWWER_/$servantlowwer/g; s/_SERVANT_/$servant/g" > $tohome/src/$servant.jce
cat $fromhome/src/_SERVANT_Impl.java | sed "s/_SERVANTLOWWER_/$servantlowwer/g; s/_APPLOWWER_/$applowwer/g; s/_SERVERLOWWER_/$serverlowwer/g; s/_SERVANT_/$servant/g" > $tohome/src/com/qq/$applowwer/$serverlowwer/${servant}Impl.java
cat $fromhome/src/_SERVER_.java | sed "s/_APPLOWWER_/$applowwer/g; s/_SERVERLOWWER_/$serverlowwer/g; s/_SERVER_/$server/g; s/_SERVANT_/$servant/g" > $tohome/src/com/qq/$applowwer/$serverlowwer/$server.java
cp $fromhome/src/j4log.property $tohome/src/
cp $fromhome/pom.xml $tohome
cp $fromhome/makefile.target $tohome

cd $tohome
/bin/sh genjce.sh
