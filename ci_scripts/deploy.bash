set -e -x
cd build || exit
ARTIFACT_VERSION=${VERSION}.${ITERATION}${POM_SNAPSHOT}
RPM_RELEASE=$(grep -r 'Release:' ../build/cdcm.spec | cut -d':' -f3 | xargs)
if [${POM_SNAPSHOT} = '']; then
  RPM_FILE=cdcm-${VERSION}-${RPM_RELEASE}.x86_64.rpm
  else
  RPM_FILE=cdcm-${VERSION}${POM_SNAPSHOT}_${ITERATION}.x86_64.rpm
fi
sed -e "s/%{version}/${ARTIFACT_VERSION}/" \
 -e "s/%{rpm_file}/RPMS\/x86_64\/${RPM_FILE}/" pom-template.xml >pom.xml
export MVN_CMD="mvn -B -s maven-settings.xml deploy"
echo "$MVN_CMD"
$MVN_CMD
rm pom.xml
