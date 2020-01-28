#!/bin/bash
# run as root only
if ( ! id | grep root > /dev/null 2>&1 );
then
    echo "You can run this script only as root !!!";
    exit 1;
fi
LAUNCHER=
while getopts dmcht o
do      case "$o" in
        t)      LAUNCHER="strace -ff -o wasp_strace ";;
        d)      LAUNCHER="cgdb --args";;
        m)      LAUNCHER="valgrind -v --num-callers=40 --leak-check=full --show-reachable=yes --log-file=cdcm_valgrind_output";;
        c)      LAUNCHER="valgrind -v --tool=callgrind";;
        h|[?])    echo -e >&2 "Usage: $0 [-dmh]\n-d\trun with debugger"
                echo -e >&2 "-m\trun with valgrind"
                echo -e >&2 "-c\trun with callgrind"
                echo -e >&2 "-t\trun with strace"
                echo -e >&2 "-h\tprint this message"
                exit 1;;
        esac
done
export LOG_OPTIONS
G_ROOT=/var/cdcm
G_TMP=${G_ROOT}/run/
G_LIB_DIR=/usr/lib
G_BIN_DIR=/usr/bin
G_LOG_DIR=${G_ROOT}/log
MD=mkdir
CP="cp -f"
RM="rm -f"
unset_locales()
{
    env | grep -E '^LC_|^LANGUAGE' | cut -d '=' -f 1 | while read line;do unset $line;done
}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/share/cdcm/lib
 if ! [ -a $procpath ]
      then
         echo "process is stopped" >> $filename
         exit 0
 fi
# create TrPManager socket directory
mkdir -p $G_ROOT
if  [ -d ${G_LOG_DIR} ]
  then
      rm -rf ${G_LOG_DIR}
fi
unset LC_CTYPE
export PATH=/usr/lib:/bin:/usr/bin
echo core.%p > /proc/sys/kernel/core_pattern
ulimit -c unlimited
ulimit -n 131072
mkdir -p $G_TMP
unset LANG
unset_locales
mount -a
if [ ! -f /dev/shm/mount.lock ]; then
    mount /dev/shm
    touch /dev/shm/mount.lock
fi
echo ${LAUNCHER} cdcm_broker
${LAUNCHER} cdcm_broker
