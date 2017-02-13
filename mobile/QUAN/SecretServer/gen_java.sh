/usr/local/taf/bin/jce2java  --base-package="com.duowan"  --no-holder SecretBase.jce
FILE=`grep qq com/duowan/QUAN/*.java | awk -F":" '{print $1}' |  sort  | uniq`
echo "$FILE" | while read line
 do
     echo $line
     `sed -i "s/qq/duowan/g" $line`
 done
