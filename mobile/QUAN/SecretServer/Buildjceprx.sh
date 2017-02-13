export JAR_DIR=/home/zhangyongsheng/QUAN/SecretServer/SecretClientJava/lib
echo JAR_DIR
mkdir class
javac -classpath  $JAR_DIR/jutil-1.0.2.jar:$JAR_DIR/netTool-1.0.0.jar:$JAR_DIR/taf-proxy-1.0.0.jar com/qq/QUAN/*.java -d ./class
cd class
jar  cvf   $JAR_DIR/SecretServant.jar  com/qq/QUAN/*class

