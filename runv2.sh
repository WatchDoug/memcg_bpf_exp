#!/bin/sh
PID=$$
mkdir ./cgroup
sudo mount -t none cgroup2 ./cgroup
sudo mkdir ./cgroup/$1/
sudo sh -c "echo $PID > ./cgroup/$1/cgroup.procs"
./$1 &

