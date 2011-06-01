#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/sched.h>
#include <errno.h>

void *thread_decode_ext_iop(void *null)
{
    while (1);
    return NULL;
}

int main()
{
    int policy;
    pthread_t ext_iop_tid;
    struct sched_param sparam;

    if (pthread_create(&ext_iop_tid, NULL, thread_decode_ext_iop, NULL) != 0)
        goto error;
    pthread_getschedparam(ext_iop_tid, &policy, &sparam);
    sparam.sched_priority = sched_get_priority_max(SCHED_OTHER);
    if (pthread_setschedparam(ext_iop_tid, SCHED_OTHER, &sparam) != 0) {
        printf("doremi set iop: %s", strerror(errno));
    }

    pthread_join(ext_iop_tid, NULL);

error:
    return 0;
}
