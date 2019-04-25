#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cutils/log.h>
#include <fcntl.h>

#define LOG_TAG "RKNpuMonitor"

#define SYS_TEMPERATURE_NODE "/sys/class/thermal/thermal_zone0/temp"
#define SYS_GPU_POLICY_NODE "/sys/devices/platform/ff9a0000.gpu/power_policy"
#define THRESHOLD_TEMPERATURE (long)10000
#define SLEEP_SECOND_TIME 3

#define DEBUG 0
static int gpuStatus = 0;

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

int simple_cat(char *path, char *buf, size_t bufsize) {
    int fd = 0;
    fd = open (path, O_RDONLY);
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

/**
 * policy0: coarse_demand, policy1: always_on
 * */
int changeGPUPolicy(int policy) {
    if(DEBUG) ALOGD("==== gpuStatus: %d, policy: %d ====", gpuStatus, policy);
    if ((policy == 0) && (gpuStatus == 1)) {
        simple_echo(SYS_GPU_POLICY_NODE, "coarse_demand");
        gpuStatus = 0;
        ALOGD("==== coarse_demand ====");
        return 0;
    }
    if ((policy == 1) && (gpuStatus == 0)) {
        simple_echo(SYS_GPU_POLICY_NODE, "always_on");
        gpuStatus = 1;
        ALOGD("==== always_on ====");
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    ALOGD("==== NPU Monitor start ====");
    int sizeMaxOfStatus = strlen("[coarse_demand] always_on");
    char strGpuStatus[sizeMaxOfStatus];
    if (simple_cat(SYS_GPU_POLICY_NODE, strGpuStatus, sizeMaxOfStatus) == 0) {
        ALOGD("==== gpu node = %s ====", strGpuStatus);
        gpuStatus = strstr(strGpuStatus, "\[always_on\]")? 1 : 0;
        if (DEBUG) ALOGD("current gpu policy(%d): %s", gpuStatus, strGpuStatus);
    }
    char strTemperature[sizeof(long)];
    while(1) {
        if (simple_cat(SYS_TEMPERATURE_NODE, strTemperature, sizeof(long)) == 0) {
            if(DEBUG) ALOGD(" start monitoring... temp is %s", strTemperature);
            long temperature = atol(strTemperature);
            if (temperature > THRESHOLD_TEMPERATURE) {
                changeGPUPolicy(0);
            } else {
                changeGPUPolicy(1);
            }
        }
        //if(DEBUG) ALOGD("sleeping ...");
        sleep(SLEEP_SECOND_TIME);
    }

    ALOGD("==== NPU Monitor stop ====");
    return 0;
}
