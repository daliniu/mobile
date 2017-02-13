cd src
jcefiles=`ls *.jce`
/usr/local/taf-version/taf-1.6.0/bin/jce2java --with-servant --base-package=com.qq.java.testcase. $jcefiles 



