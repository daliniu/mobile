cd ./classes
jarname=Java_TestServer_TestServant.jar
jarpath=./com/qq/java/testserver/testservant/
jarfiles=`find $jarpath -iname *.class | xargs -n 1 -i echo -n {}' '`
/usr/local/jdk/bin/jar cvf $jarname $jarfiles
echo "-- $jarname "
jcelibpath=/home/tafjce/Java/TestServer
if [[ ! -d $jcelibpath ]]
then
	mkdir -p $jcelibpath
fi
cp ./$jarname $jcelibpath/
rm -f ./$jarname
cp ../src/*.jce $jcelibpath/
echo "-- $jcelibpath/$jarname"


