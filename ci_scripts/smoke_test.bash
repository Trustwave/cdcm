sudo yum update -y --nogpgcheck
sudo yum erase -y cdcm
sudo yum install -y build/RPMS/x86_64/*.rpm
export PATH=/opt/carrier/bin:$PATH
export FAILED=0
IFS="," SMOKE_TEST_SERVERS_ARRAY=(${SMOKE_TEST_SERVERS}); for i in "${SMOKE_TEST_SERVERS_ARRAY[@]}";do  sudo PATH=/opt/carrier/bin:$PATH /opt/carrier/bin/carrier -t full -p 445 -x /root/input_file_$i.xml --asset-meta-path output_meta.xml  2>&1 | tee $REPORT_FNAME.$i.smoke.log ;done
IFS="," SMOKE_TEST_SERVERS_ARRAY=(${SMOKE_TEST_SERVERS}); for i in "${SMOKE_TEST_SERVERS_ARRAY[@]}";do  sshpass -f /root/pf  scp -P 2222  $REPORT_FNAME.$i.smoke.log root@10.88.240.20://opt/tests_results ;done
IFS="," SMOKE_TEST_SERVERS_ARRAY=(${SMOKE_TEST_SERVERS}); for i in "${SMOKE_TEST_SERVERS_ARRAY[@]}";do  if  grep -q AUTHENTICATION_SUCCESS_EVENT $REPORT_FNAME.$i.smoke.log; then echo success;else ((FAILED++)) ; echo failed; fi ;done
IFS="," SMOKE_TEST_SERVERS_ARRAY=(${SMOKE_TEST_SERVERS}); for i in "${SMOKE_TEST_SERVERS_ARRAY[@]}";do  if [ $(cat $REPORT_FNAME.$i.smoke.log  | grep ERROR  | wc -l) = "0" ]; then echo success;else ((FAILED++)) ; echo failed; fi ;done
if [[ "$FAILED" -gt 0 ]]; then exit 1 ;fi