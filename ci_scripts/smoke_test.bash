sudo yum update -y
yum -y install "$(CARRIER_REPO_BASE_URL)/$(curl --silent "$(CARRIER_REPO_BASE_URL)/" | \
  grep --only-matching -e 'tw-carrier-\([0-9\.-]*\).rpm' | tail --lines=1)"
sudo yum erase -y cdcm
sudo yum install -y build/RPMS/x86_64/*.rpm
export FAILED=0
IFS="," SMOKE_TEST_SERVERS_ARRAY=(${SMOKE_TEST_SERVERS})
for HOST in "${SMOKE_TEST_SERVERS_ARRAY[@]}"; do
  sed -e "s/%{username}/${SMOKE_TEST_USERNAME}/" -e "s/%{password}/${SMOKE_TEST_PASSWORD}/" \
    -e "s/%{host}/${HOST}/" build/carrier-input.xml > build/${HOST}-input.xml
  sudo /opt/carrier/bin/carrier -t full -p 445 -x "build/${HOST}-input.xml" --asset-meta-path /dev/null 2>&1 | \
    tee "/opt/test_results/$REPORT_FNAME".${HOST}.smoke.log
  if grep -q AUTHENTICATION_SUCCESS_EVENT "/opt/test_results/$REPORT_FNAME".${HOST}.smoke.log; then
    echo success
    else
    ((FAILED++))
    echo failed
  fi
done
echo $FAILED
if [[ "$FAILED" -gt 0 ]]; then exit 1; fi

