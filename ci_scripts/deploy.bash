cd build
sed -e "s/%{version}/${ARTIFACT_VERSION}/" -e "s/%{rpm_file}/RPMS\/x86_64\/cdcm-${VERSION}${POM_SNAPSHOT}_${ITERATION}.x86_64.rpm/" pom-template.xml > pom.xml
export MVN_CMD="mvn -B -s maven-settings.xml deploy"
echo $MVN_CMD
$MVN_CMD
rm pom.xml