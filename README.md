# memcg_bpf_exp
The host platform is set as x86_64 system with 8GB RAM. The Linux distribution is Debian bullseye with the build-essential package installed. The Linux kernel version is v5.10. We believe the experiment also works on other debian and kernel version. For the virtualization engine, we use qemu-kvm v5.2.90. Once you enter the VM environemnt, you can repeat our experiment by simply run:
```shell
# for cgroup v1 users
./runv1.sh
```
or
```shell
# for cgroup v2 users
./runv2.sh
```

The script does the following things:
- Compile the PoC code. (normal privilege)
- Create a new cgroup named bpf. (root privilege)
- Set the memory usage limit to **1G** for the bpf cgroup. (root privilege)
- Add current process to bpf cgroup. (root privilege)
- Run the PoC code. (**normal** privilege)

Wait a few minutes, take a cup of coffee, and you can observe OOM crash on the VM. 

The PoC program run out all the 8GB RAM under 1GB memcg limit, default rlimit settings, and default sysctl settings. Note that although the memcg setting needs root privilege and require sudo password, the PoC program itself is run under normal privilege.
