BASE_URI 	:= http://192.168.120.20:8000
URI_CENTOS	:= $(BASE_URI)/centos/7

define REPO_OS
[centos-7.6]
name=CentOS 7.6
baseurl=$(URI_CENTOS)/7.6/os/
gpgcheck=0

[centos-7.6-upd]
name=CentOS 7.6 Updates
baseurl=$(URI_CENTOS)/7.6/updates/
gpgcheck=0

[centos-7.6-epel]
name=CentOS 7.6 EPEL
baseurl=$(URI_CENTOS)/7.6/epel
gpgcheck=0

[centos-7.6-extras]
name=CentOS 7.6 Extras
baseurl=$(URI_CENTOS)/7.6/extras/
gpgcheck=0

endef

define REPO_DOCKER
[centos-7-docker]
name=CentOS 7 docker
baseurl=$(URI_CENTOS)/docker/
gpgcheck=0

endef

define REPO_SCL
[scl-dev-ts-7]
name=SCL DEV-TS-7
baseurl=$(URI_CENTOS)/7.6/sclo/devtoolset-7
gpgcheck=0

[scl-dev-ts-8]
name=SCL DEV-TS-8
baseurl=$(URI_CENTOS)/7.6/sclo/devtoolset-8
gpgcheck=0

[scl-llvm-ts-7]
name=SCL LLVM-TS-7
baseurl=$(URI_CENTOS)/7.6/sclo/llvm-toolset-7
gpgcheck=0

endef

define REPO_LOCAL
[swg-app-local]
name=SWG app local
baseurl=file:///opt/repo
gpgcheck=0
endef


export REPO_OS
export REPO_DOCKER
export REPO_SCL
export REPO_LOCAL
