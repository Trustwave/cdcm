FROM centos:centos7
ENV container docker
ENV PYTHON_VERSION "3.6.3"
ADD boost_1_70_0.tar.bz2
RUN yum -y install \
            git \
            patch \
            gnutls.x86_64 \
            gnutls-devel.x86_64 \
            libarchive-devel.x86_64 \
            pam-devel.x86_64 \
            centos-release-scl \ 
            rh-python36 \
            devtoolset-7-gcc.x86_64 \
            devtoolset-7-gcc-c++.x86_64 \           
            wget 
            
RUN  cd /boost_1_70_0 ; \
    ./bootstrap.sh; \
    ./b2 --layout=tagged --build-dir=./build --build-type=complete address-model=64; \
    rm -rf ./build; \
    yum clean all;

#RUN yum install -y https://centos7.iuscommunity.org/ius-release.rpm
#RUN yum install -y python36u python36u-libs python36u-devel python36u-pip
