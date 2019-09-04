#!/bin/bash

if [ ! -d /opt/cores ]; then
    mkdir -p /opt/cores
fi
# set cores destination directory and core's pattern
echo /opt/cores/core.%p > /proc/sys/kernel/core_pattern

ulimit -c unlimited
ulimit -n 4096

if [ ! -d /var/cdcm/log ]; then
        mkdir -p /var/cdcm/log
fi

mount -a
if [ ! -f /dev/shm/mount.lock ]; then
        mount /dev/shm
        touch /dev/shm/mount.lock
fi

if [ ! -d /dev/shm/cdcm ]; then
        mkdir -p /dev/shm/cdcm
fi

cdcm_broker
