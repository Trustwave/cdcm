set -e -x
cd build || exit
RPM_RELEASE=$(grep -r 'define release' ../build/cdcm.spec | cut -d' ' -f3 | xargs)
if [ "${ACTION}" == 'deploy_release' ] && [ "${POM_SNAPSHOT}" = '' ]; then
  ARTIFACT_VERSION=${VERSION}.${ITERATION}
  RPM_FILE=cdcm-${VERSION}-${RPM_RELEASE}.x86_64.rpm
else
  ARTIFACT_VERSION=${VERSION}.${ITERATION}-SNAPSHOT
  RPM_FILE=cdcm-${VERSION}-SNAPSHOT_${ITERATION}.x86_64.rpm
fi
sed -e "s/%{hostname}/${NEXUS_HOSTNAME}/" -e "s/%{version}/${ARTIFACT_VERSION}/" \
 -e "s/%{rpm_file}/RPMS\/x86_64\/${RPM_FILE}/" pom-template.xml >pom.xml
sed -i -e "s/%{hostname}/${NEXUS_HOSTNAME}/" maven-settings.xml
sed -i -e "s/%{username}/${NEXUS_USERNAME}/" maven-settings.xml
sed -i -e "s/%{password}/${NEXUS_PASSWORD}/" maven-settings.xml
cat pom.xml
cat maven-settings.xml
mvn -B -s maven-settings.xml deploy
rm pom.xml
