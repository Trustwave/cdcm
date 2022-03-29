set -e
cd build || exit
export RPM_RELEASE=$(grep -r 'Release:' cdcm.spec | cut -d':' -f3 | xargs)
if [ "$RELEASE" = "SNAPSHOT" ]; then
  export RELEASE_DEF="SNAPSHOT_$ITERATION"
else
  export RELEASE_DEF="$RPM_RELEASE"
fi
rpmbuild --define "_specdir $( dirname "$0" )" --define "pkg_version ${VERSION}" \
  --define "release ${RELEASE_DEF}" -bb cdcm.spec
