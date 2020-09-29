source ci_scripts/common.bash
sudo yum -y erase cdcm
sudo yum install -y build/RPMS/x86_64/*.rpm
cd cdcm_testing/functional_tests || exit
sed -i "s@/var/log/cdcm/testsRunner/@""$PWD""/log/@g" testsRunner_settings.json
source /etc/profile.d/rvm.sh
rvm reload
set -e
export FAILED=0
export testFiles=(full.xml error_code_tests.xml qa_4_permissions.xml)
for i in ${testFiles[@]}; do
  ruby mdclient2.rb "$i" 2>&1 | tee "$REPORT_FNAME".functional."$i".log
  mv log/cdcm_client1.log log/"$REPORT_FNAME".cdcm_client."$i".log
  upload_file "$REPORT_FNAME".functional."$i".log
  upload_file log/"$REPORT_FNAME".cdcm_client."$i".log
  if [ $(cat "$REPORT_FNAME".functional."$i".log | grep -c 'failed \[0' ) = "1" ]; then
    echo success
  else
    ((FAILED++))
    echo failed
  fi
done
if [[ "$FAILED" -gt 0 ]]; then exit 1; fi
