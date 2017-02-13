#!/bin/sh

bin="/data/app/taf/bin/tafnode"

PID=`ps -eopid,cmd | grep "$bin"| grep "tafnode" |  grep -v "grep" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
    kill -9 $PID
    echo "kill -9 $PID"
fi

ulimit -c unlimited

$bin  --config=/data/app/taf/tafnode/conf/node.config.conf &

