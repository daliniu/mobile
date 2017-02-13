cd /home/tafjce/release
./clean_obj.sh
rm *.jce [Mm]akefile *.h *.cpp *.o *.a
JCE=`find /home/tafjce -name "*.jce" -follow|xargs -n1 dirname |sort |uniq`
for j in $JCE
do
    echo $j
    cp $j/*jce .
    cp $j/[Mm]akefile .
    make release
    rm *.jce [Mm]akefile *.h *.cpp *.o *.a
done

#重新编译UI库
echo "release uilib begin..."
UI_VOB="/home/svn/mqq_superqq_rep/SuperGold2009_proj/tags"
if [ -d $UI_VOB ]
then
	cd $UI_VOB
	DEST_TAG=`ls -lt|head -2|grep -v "总计"|awk '{print $8}'`
	SRC_PATH=$UI_VOB/$DEST_TAG/src/COMM/SmsUiLib-1.3/src
	RELEASE_PATH=$UI_VOB/$DEST_TAG/src/COMM/SmsUiLib-1.3

	if [ -d $SRC_PATH ]
	then
		cd $SRC_PATH
		make cleanall;make all
			if [ -d $RELEASE_PATH ]
			then
				cd $RELEASE_PATH
				chmod 4755 release.sh
				./release.sh
				echo "release uilib ok..."
			fi
	fi
fi