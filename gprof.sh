#!/bin/sh
rm -rf a.out gmon.out test.dot test.png
g++ -g -pg -pthread gprof-helper.so -std=gnu++0x Buff.cpp Buff_Manager.cpp Config.cpp Socket.cpp Log.cpp
./Robot /home/project/robot/config.txt
gprof a.out gmon.out > report.txt
gprof2dot report.txt > test.dot
dot test.dot -Tpng -o test.png
