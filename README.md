![Trustwave Logo]( https://www.trustwave.com/img/logo/logo-trustwave-white.svg)

# CDCM - Credentialed Data Collection Module

CDCM Will act as an intermediate entity between Carrier and Windows assets. Its responsibility is to allow querying the assets to validate that there are no vulnerabilities in it. The CDCM lets the client connect to the asset in variety of protocols like SMB, RPC over SMB, WMI and WinRM.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

OS should be CentOS7 or greater, The OS should be updated.

```
yum install -y https://centos7.iuscommunity.org/ius-release.rpm
yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
yum -y update
```

Third party libraries that we use with our source:

* [CPPZMQ](https://github.com/zeromq/cppzmq) - MDP.
* [samba](https://github.com/samba-team/samba) - RPC.
* [JSON](https://github.com/taocpp/json) - Proprietari protocol.
* [libpe](https://github.com/merces/libpe) - File version.
* [BOOST](https://www.boost.org) - Process, IPC(Shared memory), multi index, Testin framework  and some other trivial libs.

```
yum  install --nogpgcheck -y \
        bzip2 \
        boost169.x86_64 \
        bzip2-libs \
        which \
        gnutls.x86_64 \
        wget \
        git \
        cmake3 \
        centos-release-scl scl-utils scl-utils-build.x86_64 \
        cppzmq-devel.x86_64 \
        czmq-devel.x86_64 \
        czmq.x86_64 \
        boost169-devel.x86_64 \
        devtoolset-8-gcc-c++.x86_64 \
        devtoolset-8-gcc.x86_64 \
        devtoolset-8-gdb.x86_64 \
        devtoolset-8-make.x86_64 \
        openssl-devel.x86_64 \
        libarchive-devel.x86_64 \
        openldap-devel.x86_64 \
        popt-devel.x86_64 \
        libtevent-devel.x86_64 \
        pam-devel.x86_64
```


### Installing Building and preparing the developer environment

download the source
```
git clone https://github.com/Trustwave/cdcm.git
```

build the source
```
cd cdcm/build
scl enable devtoolset-8 "make deps"
cd ..
mkdir build_cdcm
cd build_cdcm
scl enable devtoolset-8 "cmake3 .."
scl enable devtoolset-8 "make -j4"
```

Install the binaries and prepare the environment
```
scl enable devtoolset-8 "make install"
cd ..
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/output/libs/:/opt/output/libs/plugins/:$PWD/deps/samba-4.10.6/bin/shared/private/:$PWD/deps/samba-4.10.6/bin/shared
mkdir -p /usr/share/cdcm/lib
cd  /usr/share/cdcm/lib/
ln -sf  /opt/output/libs/plugins
```

## Support

If you found a problem with the software, please [create an
issue](https://github.com/trustwave/cdcm/issues) on GitHub. If you
are a Greenbone customer you may alternatively or additionally forward your
issue to the Greenbone Support Portal.


## Maintainer

This project is maintained by [Trustwave](https://www.trustwave.com/).

## Contributing

Your contributions are highly appreciated. Please [create a pull
request](https://github.com/trustwave/cdcm/pulls) on GitHub. Bigger
changes need to be discussed with the development team via the [issues section
at GitHub](https://github.com/trustwave/cdcm/issues) first.

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 3 License - see the [LICENSE](LICENSE) file for details
