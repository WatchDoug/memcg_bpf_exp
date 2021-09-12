#!/bin/sh
BIN_NAME=bpf
gcc main.c -o $BIN_NAME
sudo mkdir /sys/fs/cgroup/memory/$BIN_NAME 2>/dev/null
echo '1G' | sudo tee /sys/fs/cgroup/memory/$BIN_NAME/memory.limit_in_bytes > /dev/null
echo $$ | sudo tee /sys/fs/cgroup/memory/$BIN_NAME/cgroup.procs > /dev/null
./$BIN_NAME &

