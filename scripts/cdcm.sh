#!/bin/sh
ulimit -c 0
ulimit -n 131072
TODO=start
RUN_USER=root
RUN_GROUP=root
CDCM_EXE=cdcm_broker
CDCM_LOG_DIR=/var/cdcm/log
G_TMP=/var/cdcm/run
START_CMD="exec start-stop-daemon --start -c $RUN_USER -g $RUN_GROUP -k 0002 -d $G_TMP"
STOP_CMD="start-stop-daemon --stop --oknodo --retry=TERM/10/KILL/5"
KILL_CMD="start-stop-daemon --stop --oknodo --signal KILL --retry 5"
STATUS_CMD="start-stop-daemon --status"

# Common functions
usage(){
    echo -e "\nUsage: $(basename $0) [--help|start|stop|status]\n"
    exit 0
}

PID="-1"
BASENAME=`basename $0`
while [ $# -gt 0 ];do
    case $1 in
        --help|-h)  usage;;
        start|stop|status) TODO=$1;;
        --pid)      shift; PID=$1;;
        *) echo "Unknow argument $1";usage;;
    esac
    shift
done

clean(){
    TOREMOVE=""

    for f in $TOREMOVE;do
        find $G_TMP/ -type f -name "${f}*" -delete
    done
    rm -f /dev/shm/sessions_lock
}
start(){
    [ -d "$G_TMP" ]  || install -o $RUN_USER -g $RUN_GROUP -m 770 -d $G_TMP
    [ -d "$CDCM_LOG_DIR" ]  || install -o $RUN_USER -g $RUN_GROUP -m 770 -d $CDCM_LOG_DIR
    $START_CMD -p /tmp/cdcm_broker.pid --exec /usr/bin/cdcm_broker >> $CDCM_LOG_DIR/cdcmbroker.log 2>&1
}
stop(){
    if [ $PID -gt 0 ];then
        kill -9 $PID > /dev/null 2>&1
    else
        $KILL_CMD --exec /usr/bin/CDCM_EXE
    fi
    exit $?
}
status(){
    if [ $PID -gt 0 ];then
        if ps -p $PID;then
            echo "${CDCM_EXE} is running."
        else
            echo "${CDCM_EXE} is NOT running."
        fi
    else
        if $STATUS_CMD --exec /usr/bin/$CDCM_EXE;then
            echo "${CDCM_EXE} is running."
        else
            echo "${CDCM_EXE} is NOT running."
        fi
    fi
}
case $TODO in
    start)
        unset LANG
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/cdcm-1.0
        clean
        start
    ;;
    stop) stop;;
    status) status;;
esac
