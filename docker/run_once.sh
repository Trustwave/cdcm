#!/bin/bash -x
ROOT_DIR=${PWD}
SAMBA_DIR=${ROOT_DIR}/samba
#get samba
git clone https://github.com/samba-team/samba.git samba
cd ${SAMBA_DIR}
#patch samba
patch -p1  -i ${ROOT_DIR}/samba.patch
#put trustwave dir
cp -r ${ROOT_DIR}/trustwave ${SAMBA_DIR}
#get pev
git clone --recursive https://github.com/merces/pev.git trustwave/3rdparty/pev
#patch pev
cd trustwave/3rdparty/pev
patch -p1 -i ${ROOT_DIR}/pev.patch
cd lib/libpe
patch -p1 -i ${ROOT_DIR}/libpe.patch
cd ../..
#build pev
make
#get json
git clone https://github.com/taocpp/json.git trustwave/3rdparty/json
#configure
cd ${SAMBA_DIR}
./buildtools/bin/waf configure  --without-ad-dc --without-json --without-ldap --without-ldap --enable-developer --without-ads  --without-acl-support --disable-python --boost-libs=/usr/lib
#build
./buildtools/bin/waf build


