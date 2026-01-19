#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

static pid_t orig_ppid;
static uid_t orig_uid;
static char orig_tty[256];

void die(const char *reason) {
    fprintf(stderr, "SESSION GUARD TRIGGERED: %s\n", reason);
    kill(getpid(), SIGKILL);
}

void get_tty(char *buf, size_t len) {
    char *t = ttyname(STDIN_FILENO);
    if (!t) die("No controlling TTY");
    strncpy(buf, t, len - 1);
    buf[len - 1] = '\0';
}

int main(void) {
    orig_ppid = getppid();
    orig_uid  = getuid();
    get_tty(orig_tty, sizeof(orig_tty));

    /* Detect DYLD injection */
    if (getenv("DYLD_INSERT_LIBRARIES"))
        die("DYLD_INSERT_LIBRARIES present");

    while (1) {
        if (getppid() != orig_ppid)
            die("Parent PID changed");

        if (getuid() != orig_uid)
            die("UID changed");

        char cur_tty[256];
        get_tty(cur_tty, sizeof(cur_tty));
        if (strcmp(cur_tty, orig_tty) != 0)
            die("TTY changed");

        /* Detect debugger attach (best effort under SIP) */
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
        struct kinfo_proc kp;
        size_t len = sizeof(kp);

        if (sysctl(mib, 4, &kp, &len, NULL, 0) == 0) {
            if (kp.kp_proc.p_flag & P_TRACED)
                die("Debugger attached");
        }

        sleep(3);
    }
}
