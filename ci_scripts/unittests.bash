set -e
cd build_cdcm/ || exit
make install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/output/lib/:/opt/output/lib/plugins/:$GIT_CLONE_PATH/deps/samba-4.10.6/bin/shared/private/:$GIT_CLONE_PATH/deps/samba-4.10.6/bin/shared
cd ../../build_cdcm/src/cdcm_testing || exit
rm -f /dev/shm/*
./tests_run --log_sink="$REPORT_FNAME".xml --log_format=JUNIT --log_level=all || code=$?
if [ $((code)) -eq 0 ]; then echo "success"; else echo 'tests failure'; fi
java -jar /usr/share/java/saxon.jar -o:"/opt/test_results/$REPORT_FNAME".html -s:"$REPORT_FNAME".xml -xsl:/opt/xunit-to-html/xunit_to_html.xsl
