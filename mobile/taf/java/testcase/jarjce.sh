cd ./classes
jarname=Java_TestCase_TestCaseServant.jar
jarpath=./com/qq/java/testcase/testcaseservant/
jarfiles=`find $jarpath -iname *.class | xargs -n 1 -i echo -n {}' '`
/usr/local/jdk/bin/jar cvf $jarname $jarfiles
echo "-- $jarname "
jcelibpath=/home/tafjce/Java/TestCase
if [[ ! -d $jcelibpath ]]
then
	mkdir -p $jcelibpath
fi
cp ./$jarname $jcelibpath/
echo "-- $jcelibpath/$jarname"


