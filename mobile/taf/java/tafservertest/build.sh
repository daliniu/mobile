#/usr/local/app/maven/bin/mvn -B -Dtype=server -DoutputDirectory=./lib clean dependency:copy-dependencies              

rm -fr classes
mkdir classes

classpath=./lib`find ./lib -iname '*.jar' | xargs -n 1 -i echo -n :{}`
srcfiles=`find ./src/ -iname '*.java' | xargs -n 1 -i echo -n {}' '`
/usr/local/jdk/bin/javac -encoding utf-8 -sourcepath ./src -d ./classes -cp $classpath $srcfiles

if cp ./src/*.property ./classes > /dev/null 2>&1; then echo -n ''; fi
if cp ./src/*.properties ./classes > /dev/null 2>&1; then echo -n ''; fi
if cp ./src/*.conf ./classes > /dev/null 2>&1; then echo -n ''; fi
if cp ./src/*.xml ./classes > /dev/null 2>&1; then echo -n ''; fi


