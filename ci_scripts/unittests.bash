cd build_cdcm/
make install
cd ../cdcm_testing/tests
sed -i "s@/var/log/cdcm/testsRunner/@"$PWD"/log/@g" ../functional_tests/testsRunner_settings.json
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/output/libs/:/opt/output/libs/plugins/:$GIT_CLONE_PATH/deps/samba-4.10.6/bin/shared/private/:$GIT_CLONE_PATH/deps/samba-4.10.6/bin/shared
cd ../../build_cdcm/src/cdcm_testing
./tests_run  --log_sink=$REPORT_FNAME.xml  --log_format=JUNIT  --log_level=all || code=$?; if [ "$code" -eq "0" ]; then echo "success"; else echo 'tests failure'; fi
java -jar /opt/xunit-to-html/saxon9he.jar -o:$REPORT_FNAME.html -s:$REPORT_FNAME.xml -xsl:/opt/xunit-to-html/xunit_to_html.xsl
sshpass -f /root/pf  scp -P 2222  $REPORT_FNAME.html root@10.88.240.20://opt/tests_results