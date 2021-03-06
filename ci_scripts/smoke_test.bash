source ci_scripts/common.bash
sudo yum update -y --nogpgcheck
sudo yum erase -y cdcm
sudo yum install -y build/RPMS/x86_64/*.rpm
export PATH=/opt/carrier/bin:$PATH
export FAILED=0
for i in {10.70.32.143,10.70.32.160}; do
  sudo PATH=/opt/carrier/bin:"$PATH" /opt/carrier/bin/carrier -t full -p 445 -x /root/input_file_$i.xml --asset-meta-path output_meta.xml 2>&1 | sudo tee "$REPORT_FNAME".$i.smoke.log
  upload_file "$REPORT_FNAME".$i.smoke.log sudo
  if grep -q AUTHENTICATION_SUCCESS_EVENT "$REPORT_FNAME".$i.smoke.log; then
    echo success
    else
    ((FAILED++))
    echo failed
  fi
  if [ $(cat "$REPORT_FNAME".$i.smoke.log | grep -c ERROR ) = "0" ]; then
    echo success
    else
    ((FAILED++))
    echo failed
  fi
done
echo $FAILED
if [[ "$FAILED" -gt 0 ]]; then exit 1; fi

