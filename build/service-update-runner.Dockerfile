FROM centos/systemd
RUN curl -L https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.rpm.sh | bash
RUN yum install -y epel-release sudo gitlab-runner
RUN yum update -y
# To avoid leaking Trustwave internal information, you have to manually supply correct values
#   for --registration-token and --url. (This project is publicly available on GitHub.)
CMD gitlab-runner register --registration-token XXXXXXXXXXXXXXXXXXXXXXXXXXXXX \
    --url https://GITLAB.EXAMPLE.COM/ --non-interactive --custom_build_dir-enabled \
    --executor=shell --tag-list=cdcm_service -name service-update-runner && \
    rm /etc/rc.d/init.d/gitlab-runner && \
    gitlab-runner run & \
    exec /usr/sbin/init
