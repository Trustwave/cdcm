FROM centos:centos7
SHELL ["/bin/bash", "-c"]
ENV container docker
ENV PYTHON_VERSION "3.6.3"
RUN yum install -y https://centos7.iuscommunity.org/ius-release.rpm
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm ; exit 0
RUN yum -y update
RUN yum  install --nogpgcheck -y \
	    bzip2 \
	    boost169.x86_64 \
        bzip2-libs \
	    which \
        gnutls.x86_64 \
        wget


