#!/bin/bash
g++ -pthread -o p2 OS_lab7_2.cpp -lrt
xattr -l p2
sudo setcap cap_sys_resource=eip p2
xattr -l p2
./p2