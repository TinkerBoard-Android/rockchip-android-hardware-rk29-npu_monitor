#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cutils/log.h>

#define LOG_TAG "RKNpuMonitor"

#define SYS_TEMPERATURE_NODE "/sys/class/thermal/thermal_zone0/temp"
#define SYS_GPU_POLICY_NODE "/sys/devices/platform/ff9a0000.gpu/power_policy"
#define THRESHOLD_TEMPERATURE (long)0
#define SLEEP_SECOND_TIME 3

#define DEBUG 1

static void simple_echo(char *path, char *s)
{
    char buf[80];
    int len;
    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
    }

    close(fd);
}

int simple_cat(char *buf, size_t bufsize) {
    int fd = 0;
    fd = open (SYS_TEMPERATURE_NODE, O_RDONLY);
    if (fd < 0) {
        ALOGE("can't open node");
        close(fd);
        return -1;
    } else {
        size_t n_read;
        //n_read = safe_read(fd, buf, bufsize);
        n_read = read(fd, buf, bufsize);
        /*if (n_read == SAFE_READ_ERROR) {
        SLOGE("cat %s error", SYS_TEMPERATURE_NODE);
        return -1;
    }
        if (n_read == 0)
        return 0;
        {
        size_t n = n_read;
        if (full_write (STDOUT_FILENO, buf, n) != n)
        SLOGE("write error");
    }*/
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    ALOGD("==== NPU Monitor start ====");
    char strTemperature[sizeof(long)];
    while(1) {
        if (simple_cat(strTemperature, sizeof(long)) == 0) {
            if(DEBUG) ALOGD(" start monitoring... temp is %s", strTemperature);
            long temperature = atol(strTemperature);
            if (temperature > THRESHOLD_TEMPERATURE) {
                simple_echo(SYS_GPU_POLICY_NODE, "coarse_demand");
                if(DEBUG) ALOGD("==== coarse_demand ====");
            } else {
                simple_echo(SYS_GPU_POLICY_NODE, "always_on");
                if(DEBUG) ALOGD("==== always_on ====");
            }
        }

        if(DEBUG) ALOGD("sleeping ...");
        sleep(SLEEP_SECOND_TIME);
    }

    ALOGD("==== NPU Monitor stop ====");
    return 0;
}
