FROM centos/systemd
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
    gitlab-runner \
    sudo \
    rpm-build
RUN yum install -y devtoolset-8
RUN pip3 install gcovr
RUN mkdir -p /opt/xunit-to-html
RUN curl -L -o /opt/xunit-to-html/xunit_to_html.xsl https://github.com/Zir0-93/xunit-to-html/releases/download/v1.0.0/xunit_to_html.xsl
RUN mkdir -p /opt/test_results
RUN echo "gitlab-runner ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
# To avoid leaking Trustwave internal information, you have to manually supply correct values
#   for --registration-token and --url. (This project is publicly available on GitHub.)
# Note that sometimes a second instance of gitlab-runner starts which causes jobs to fail. To
#   permanently fix this issue, please run 'systemctl disable gitlab-runner' and 'systemctl stop
#   gitlab-runner' from within the container.
CMD gitlab-runner register --registration-token XXXXXXXXXXXXXXXXXXXXXXXXXXXXX \
    --url https://GITLAB.EXAMPLE.COM/ --non-interactive --custom_build_dir-enabled \
    --executor=shell --tag-list=cdcm_install,cdcm_smoke,cdcm_tests,cdcm \
    --name cdcm-unified-runner && \
    rm /etc/rc.d/init.d/gitlab-runner && \
    cd /opt/test_results && python3 -m http.server & \
    gitlab-runner run & \
    exec /usr/sbin/init
