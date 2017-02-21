#!/bin/sh
g++ -g -pg -pthread gprof-helper.so -std=gnu++0x Buff.cpp Buff_Manager.cpp Config.cpp Socket.cpp Log.cpp
./Robot
gprof a.out gmon.out > report.txt
gprof2dot report.txt > test.dot
dot test.dot -Tpng -o test.png
