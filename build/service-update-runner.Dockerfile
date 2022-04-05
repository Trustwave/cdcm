FROM centos/systemd
RUN curl -L https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.rpm.sh | bash
RUN yum install -y epel-release sudo gitlab-runner
RUN yum update -y
CMD gitlab-runner register --non-interactive -r XXXXXXXXXXXXXXXXXXXXXXXXXXXXX \
    -u https://GITLAB.EXAMPLE.COM/ --custom_build_dir-enabled --executor=shell \
    --tag-list=cdcm_service -name service-update-runner && \
    rm /etc/rc.d/init.d/gitlab-runner && \
    gitlab-runner run & \
    exec /usr/sbin/init
