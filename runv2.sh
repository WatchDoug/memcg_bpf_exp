#!/bin/sh
BIN_NAME=bpf
gcc main.c -o $BIN_NAME
mkdir ./cgroup
sudo mount -t cgroup2 none ./cgroup
sudo mkdir ./cgroup/$BIN_NAME/
echo '1G' | sudo tee ./cgroup/$BIN_NAME/memory.max > /dev/null
echo $$ | sudo tee ./cgroup/$BIN_NAME/cgroup.procs > /dev/null
./$BIN_NAME &

