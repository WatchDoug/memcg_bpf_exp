#!/bin/sh
BIN_NAME=bpf
gcc main.c -o $BIN_NAME
mkdir ./cgroup
# Environment setting needs root privilege
sudo mount -t cgroup2 none ./cgroup
sudo mkdir ./cgroup/$BIN_NAME/
echo '1G' | sudo tee ./cgroup/$BIN_NAME/memory.max > /dev/null
echo $$ | sudo tee ./cgroup/$BIN_NAME/cgroup.procs > /dev/null
# The PoC is run under normal privilege
./$BIN_NAME &

