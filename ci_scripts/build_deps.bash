set -e -x
cd build || exit
scl enable devtoolset-8 'make deps'
