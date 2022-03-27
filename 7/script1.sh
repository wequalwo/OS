#!/bin/bash
g++ -pthread -o p1 OS_lab7_1.cpp -lrt
xattr -l p1
sudo setcap cap_sys_resource=eip p1
xattr -l p1
./p1