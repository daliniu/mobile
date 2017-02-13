#!/bin/sh

killall -9 TransServer

ulimit -c unlimited

./TransServer --config=trans.conf &
