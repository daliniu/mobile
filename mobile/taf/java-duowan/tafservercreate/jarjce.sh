cd ./classes
jarname=_APP___SERVER___SERVANT_.jar
jarpath=./com/qq/_APPLOWWER_/_SERVERLOWWER_/_SERVANTLOWWER_/
jarfiles=`find $jarpath -iname *.class | xargs -n 1 -i echo -n {}' '`
/usr/local/jdk/bin/jar cvf $jarname $jarfiles
echo "-- $jarname "
jcelibpath=/home/tafjce/_APP_/_SERVER_
if [[ ! -d $jcelibpath ]]
then
	mkdir -p $jcelibpath
fi
cp ./$jarname $jcelibpath/
rm -f ./$jarname
cp ../src/*.jce $jcelibpath/
echo "-- $jcelibpath/$jarname"


