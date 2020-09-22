set -e -x
cd build || exit
export RPM_RELEASE=$(grep -r 'Release:' cdcm.spec | cut -d':' -f3 | xargs)
if [ "$RELEASE" = "SNAPSHOT" ]; then
  export RELEASE_DEF="SNAPSHOT_$ITERATION"
else
  export RELEASE_DEF="$RPM_RELEASE"
fi
export RPMBUILD_CMD="rpmbuild --define \"pkg_version ${VERSION}\" --define \"release $RELEASE_DEF\"  -bb cdcm.spec"
echo rpmbuild --define "pkg_version ${VERSION}" --define "release ${RELEASE_DEF}" -bb cdcm.spec
rpmbuild --define "pkg_version ${VERSION}" --define "release ${RELEASE_DEF}" -bb cdcm.spec
