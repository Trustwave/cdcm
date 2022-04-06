sudo yum update -y
sudo yum erase -y cdcm
sudo yum install -y build/RPMS/x86_64/*.rpm
export FAILED=0
IFS="," SMOKE_TEST_SERVERS_ARRAY=(${SMOKE_TEST_SERVERS}); for i in "${SMOKE_TEST_SERVERS_ARRAY[@]}"; do
  sed -i -e "s/%{username}/${SMOKE_TEST_USERNAME}/" build/${i}_CDCM.xml
  sed -i -e "s/%{password}/${SMOKE_TEST_PASSWORD}/" build/${i}_CDCM.xml
  sudo /opt/carrier/bin/carrier -t full -p 445 -x "build/${i}_CDCM.xml" --asset-meta-path /dev/null 2>&1 | \
    tee "/opt/test_results/$REPORT_FNAME".${i}.smoke.log
  if grep -q AUTHENTICATION_SUCCESS_EVENT "/opt/test_results/$REPORT_FNAME".${i}.smoke.log; then
    echo success
    else
    ((FAILED++))
    echo failed
  fi
done
echo $FAILED
if [[ "$FAILED" -gt 0 ]]; then exit 1; fi

