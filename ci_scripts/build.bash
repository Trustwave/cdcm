set -e
mkdir -p build_cdcm
cd build_cdcm || exit
scl enable devtoolset-8 "cmake3 $CMAKE_FLAGS .."
scl enable devtoolset-8 "make -j4"
rm -fr /opt/output/"$CI_COMMIT_REF_SLUG"/libs/*
rm -fr /opt/output/"$CI_COMMIT_REF_SLUG"/bin/*
scl enable devtoolset-8 "make install"
