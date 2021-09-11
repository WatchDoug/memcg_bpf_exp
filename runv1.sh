#!/bin/sh
PID=$$
sudo mkdir /sys/fs/cgroup/memory/$1
sudo sh -c "echo $PID > /sys/fs/cgroup/memory/$1/cgroup.procs"
./$1 &

