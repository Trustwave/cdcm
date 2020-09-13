sudo yum -y erase cdcm
sudo yum install -y build/RPMS/x86_64/*.rpm
cd cdcm_testing/functional_tests
sed -i "s@/var/log/cdcm/testsRunner/@"$PWD"/log/@g" testsRunner_settings.json
source /etc/profile.d/rvm.sh
rvm reload
export testFiles=(full.xml error_code_tests.xml qa_4_permissions.xml)
for i in ${testFiles[@]};do ruby mdclient2.rb $i 2>&1 | tee $REPORT_FNAME.functional.$i.log ; mv log/cdcm_client1.log log/$REPORT_FNAME.cdcm_client.$i.log ;done
for i in ${testFiles[@]};do sshpass -f /root/pf  scp -P 2222 $REPORT_FNAME.functional.$i.log root@10.88.240.20://opt/tests_results ;done
for i in ${testFiles[@]};do sshpass -f /root/pf  scp -P 2222  log/$REPORT_FNAME.cdcm_client.$i.log root@10.88.240.20://opt/tests_results ;done
export FAILED=0
for i in ${testFiles[@]};do if [ $(cat $REPORT_FNAME.functional.$i.log  | grep 'failed \[0'  | wc -l) = "1" ]; then echo success; else ((FAILED++)) ; echo failed; fi ;done
if [[ "$FAILED" -gt 0 ]]; then exit 1 ;fi