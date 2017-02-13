nun=$1;
i=0;
while [ $i -lt $nun ]
do
	echo $i
	/usr/local/app/taf/tafnode/data/Java.Test/conf/pressTest.sh;
	let i=$i+1;
done;
