set -e -x
function upload_file {
    SERVER=10.88.240.20
    PORT=2222
    echo uploading "$1" to server "$SERVER"
    sshpass -f /root/pf scp -P "$PORT" "$1" root@"$SERVER"://opt/tests_results
}
