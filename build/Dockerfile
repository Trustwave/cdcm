FROM centos/systemd
SHELL ["/bin/bash", "-c"]
ENV container docker
ENV PYTHON_VERSION "3.6.3"
COPY scanner-dna-centos7-production.repo /etc/yum.repos.d
COPY scanner-signing-key-production.asc /
RUN rpm --import /scanner-signing-key-production.asc
RUN curl -L https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.rpm.sh | bash
RUN yum install -y https://repo.ius.io/ius-release-el7.rpm
RUN yum update -y
RUN yum install -y \
    boost169.x86_64 \
    gnutls.x86_64 \
    git \
    cmake3 \
    centos-release-scl scl-utils scl-utils-build.x86_64 \
    cppzmq-devel.x86_64 \
    czmq-devel.x86_64 \
    czmq.x86_64 \
    boost169-devel.x86_64 \
    openssl-devel.x86_64 \
    libarchive-devel.x86_64 \
    openldap-devel.x86_64 \
    popt-devel.x86_64 \
    libtevent-devel.x86_64 \
    pam-devel.x86_64 \
    python3-devel.x86_64 \
    maven \
    saxon \
    sshpass \
    gitlab-runner \
    sudo \
    tw-carrier \
    rpm-build
RUN yum install -y devtoolset-8
RUN pip3 install gcovr
RUN mkdir -p /opt/xunit-to-html
RUN curl -L -o /opt/xunit-to-html/xunit_to_html.xsl https://github.com/Zir0-93/xunit-to-html/releases/download/v1.0.0/xunit_to_html.xsl
RUN mkdir -p /opt/tests_results
RUN echo "gitlab-runner ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
RUN mkdir -p /opt/build
RUN chmod a+rwx /opt/build
CMD gitlab-runner register --non-interactive -r XXXXXXXXXXXXXXXXXXXXXXXXXXXXX \
    -u https://GITLAB.EXAMPLE.COM/ --custom_build_dir-enabled --executor=shell \
    --tag-list=cdcm_install,cdcm_service,cdcm_smoke,cdcm_tests,cdcm -name joelTest && \
    exec /usr/sbin/init
