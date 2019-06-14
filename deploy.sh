#!/bin/bash

#get samba
git clone https://github.com/samba-team/samba.git samba
#patch samba
patch -p1 samba.patch
#put trustwave dir
cp -r trustwave samba/ 
#get pev
https://github.com/merces/pev.git samba/trustwave/3rdparty/ 
#patch pev
patch -p1 pev.patch
#build pev
cd samba/trustwave/3rdparty/pev
make
#get json
https://github.com/taocpp/json.git samba/trustwave/3rdparty/
#configure
#build


