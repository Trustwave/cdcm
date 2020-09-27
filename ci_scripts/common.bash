set -e -x
function upload_file {
    SERVER=10.88.240.20
    PORT=2222
    echo uploading "$1" to server "$SERVER"
    if [ $# -eq 2 ]; then
    "$2" sshpass -f /root/pf scp -P "$PORT" "$1" root@"$SERVER"://opt/tests_results
    else
    sshpass -f /root/pf scp -P "$PORT" "$1" root@"$SERVER"://opt/tests_results
    fi

}
