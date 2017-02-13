cd ./classes
jarname=Java_Test_Basic.jar
jarpath=./com/qq/java/test/basic/
jarfiles=`find $jarpath -iname *.class | xargs -n 1 -i echo -n {}' '`
/usr/local/jdk/bin/jar cvf $jarname $jarfiles
echo "-- $jarname "
jcelibpath=/home/tafjce/Java/Test
if [[ ! -d $jcelibpath ]]
then
	mkdir -p $jcelibpath
fi
cp ./$jarname $jcelibpath/
echo "-- $jcelibpath/$jarname"


