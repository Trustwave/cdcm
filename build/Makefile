MKDIR 		:= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
TOP 		:= $(subst /build/,,$(MKDIR))
export TOP
export MKDIR
include $(MKDIR)/conf.mk

export PRJ
export REPO_DIR

define REPO_VER
[$(PRJ)-$(MAJOR).$(MINOR)]
name=$(PRJ) $(MAJOR).$(MINOR)
baseurl=$(BASE_URI)/$(PRJ)/$(MAJOR).$(MINOR)

endef

export REPO_VER

APPS		:= $(PRJ) build
MOUNT_DIR 	:= /opt/


REQ_RPMS	:= rsync yum-utils

TAR_CMD		:= tar --numeric-owner -zcf

SAMBA		:= samba-4.10.6
LIBPE_PEV	:= libpe
TAOCPP_JSON	:= taocpp-json

default goal: help

.PHONY: help clean.fs.all clean.app.all clean.iso repo iso pre check deps

define HELP
make <target1> <target2> ...

main targets:
	app.build docker.build app.$(PRJ) docker.$(PRJ)

available targets:
	help
	deps
	cdcm
	app.[build|$(prj)]
	docker.[build|$(prj)]
	clean.app.[build|$(prj)]   (free space only so as useed $(DATE))
	clean.rpms.[build|$(prj)] (allow rebuld rpm set)
	repo.[build|$(prj)] (internal usage)
endef

export HELP

help:
	@echo "$$HELP"

check:
	@echo "* run $@ `date +%F.%H:%M:%S`"
	for rpm in $(REQ_RPMS);do rpm -q $$rpm;done
	@echo "* fin $@ `date +%F.%H:%M:%S`"

# Check that % known and supported
validate.%:
	@echo "* run $@ `date +%F.%H:%M:%S`"
	$(eval P := $(filter $*, $(APPS)))
	@if [ ! -z $(P) ];then \
		echo "* $@"; \
	else \
		echo "Bad target $@"; \
		exit 1; \
	fi
	@echo "* fin $@ `date +%F.%H:%M:%S`"

deps: pre $(TOP)/deps/.done
	@echo "* run $@ `date +%F.%H:%M:%S`"
	@echo "* fin $@ `date +%F.%H:%M:%S`"

$(TOP)/deps/.done: $(TOP)/deps/$(TAOCPP_JSON)/.done $(TOP)/deps/$(LIBPE_PEV)/.done $(TOP)/deps/$(SAMBA)/.done
	@echo "* run $@ `date +%F.%H:%M:%S`"
	@echo "* fin $@ `date +%F.%H:%M:%S`"

$(TOP)/deps/$(TAOCPP_JSON)/.done: $(TOP)/tars/$(TAOCPP_JSON).tar
	@echo "* run $@ `date +%F.%H:%M:%S`"
	if [ -d $(TOP)/deps/ ];then rm -rf $(TOP)/deps/$(TAOCPP_JSON);fi
	tar -xvf $(TOP)/tars/$(TAOCPP_JSON).tar -C $(TOP)/deps/
	-mkdir -p $(TOP)/deps/$(TAOCPP_JSON)/build
	(cd $(TOP)/deps/$(TAOCPP_JSON)/build; cmake3 -G "Unix Makefiles" -DTAOCPP_JSON_BUILD_TESTS=OFF -DTAOCPP_JSON_BUILD_EXAMPLES=OFF ..)
	(cd $(TOP)/deps/$(TAOCPP_JSON)/build; make)
	touch $(TOP)/deps/$(TAOCPP_JSON)/.done
	@echo "* fin $@ `date +%F.%H:%M:%S`"

$(TOP)/deps/$(LIBPE_PEV)/.done: $(TOP)/tars/$(LIBPE_PEV).tar
	@echo "* run $@ `date +%F.%H:%M:%S`"
	if [ -d $(TOP)/deps/ ];then rm -rf $(TOP)/deps/$(LIBPE_PEV);fi
	tar -xvf $(TOP)/tars/$(LIBPE_PEV).tar -C $(TOP)/deps/
	(cd $(TOP)/deps/$(LIBPE_PEV)/; make)
	(cd $(TOP)/deps/$(LIBPE_PEV)/; make install)
	touch $(TOP)/deps/$(LIBPE_PEV)/.done
	@echo "* fin $@ `date +%F.%H:%M:%S`"


$(TOP)/deps/$(SAMBA)/.done: $(TOP)/tars/$(SAMBA).tar.partaa
	@echo "* run $@ `date +%F.%H:%M:%S`"
	if [ -d $(TOP)/deps/ ];then rm -rf $(TOP)/deps/$(SAMBA);fi
	cat $(TOP)/tars/$(SAMBA).tar.parta* > $(TOP)/tars/$(SAMBA).tar
	tar -xvf $(TOP)/tars/$(SAMBA).tar -C $(TOP)/deps/
	(cd $(TOP)/deps/$(SAMBA); ./buildtools/bin/waf configure 	--without-ad-dc        \
    															--with-system-mitkrb   \
                                                                --without-json         \
                                                                --enable-developer     \
                                                                --without-acl-support  \
                                                                --disable-rpath        \
                                                                --disable-python)

	(cd $(TOP)/deps/$(SAMBA); make -j4)
	touch $(TOP)/deps/$(SAMBA)/.done
	@echo "* fin $@ `date +%F.%H:%M:%S`"

pre:
	@echo "* run $@ `date +%F.%H:%M:%S`"
	@echo "* check required rpms"
	for rpm in $(REQ_RPMS);do rpm -q $$rpm;done
	if [ ! -d /proc/self ];then mount -t proc proc /proc;fi
	if [ ! -e /dev/random ];then mknod -m 666 /dev/random c 1 8;fi
	if [ ! -e /dev/urandom ];then mknod -m 666 /dev/urandom c 1 9;fi
	if [ ! -d $(REPO_DIR)/repodata ];then mkdir -p $(REPO_DIR)/repodata;fi
	if [ ! -f $(REPO_DIR)/repodata/repomd.xml ];then touch $(REPO_DIR)/repodata/repomd.xml;fi
	for dir in $(BUILD_ROOT) $(OUT) $(TMP);do \
		if [ ! -d $$dir ];then mkdir -p $$dir;fi; \
	done
	for dir in tars deps;do \
		if [ ! -d $(TOP)/$$dir ];then mkdir -p $(TOP)/$$dir;fi; \
	done
	@echo "* fin $@ `date +%F.%H:%M:%S`"


clean.app.all: clean.app.build clean.app.$(PRJ)
	@echo "* run $@ `date +%F.%H:%M:%S`"
	@echo "* fin $@ `date +%F.%H:%M:%S`"


clean.tmpl:
	@echo "* run $@ `date +%F.%H:%M:%S`"
	-rm -f $(BUILD_ROOT)/tmpl.app.cur
	for dir in $(BUILD_ROOT)/tmpl.app.*;do \
		if [ -d $$dir/rootfs$(REPO_DIR)/repodata ];then umount $$dir/rootfs$(REPO_DIR); fi ;\
		if [ -d $$dir/rootfs/proc/sys ];then umount $$dir/rootfs/proc; fi ;\
		if [ -d $$dir$(REPO_DIR)/repodata ];then umount $$dir$(REPO_DIR); fi ;\
		if [ -d $$dir/proc/sys ];then umount $$dir/proc; fi ;\
		if [ -d $$dir ];then rm -rf $$dir;fi ;\
	done
	@echo "* fin $@ `date +%F.%H:%M:%S`"


clean.app.%: validate.%
	@echo "* run $@ `date +%F.%H:%M:%S`"
	for dir in $(BUILD_ROOT)/app.$*.*;do \
		if [ -d $$dir/rootfs$(REPO_DIR)/repodata ];then umount $$dir/rootfs$(REPO_DIR); fi ;\
		if [ -d $$dir/rootfs/proc/sys ];then umount $$dir/rootfs/proc; fi ;\
		if [ -d $$dir$(REPO_DIR)/repodata ];then umount $$dir$(REPO_DIR); fi ;\
		if [ -d $$dir/proc/sys ];then umount $$dir/proc; fi ;\
		if [ -d $$dir ];then umount $$dir;rm -rf $$dir;fi ;\
	done
	@echo "* fin $@ `date +%F.%H:%M:%S`"


$(REPO_DIR)/repodata/.repolst: $(REPO_DIR)/repodata/repomd.xml
	@echo "* run $@ `date +%F.%H:%M:%S`"
	createrepo $(REPO_DIR)
	# clean .repolst
	>$(REPO_DIR)/repodata/.repolst
	repomanage $(REPO_DIR)/ | while read line;do echo "`rpm -qp --qf %{name} $$line` $$line" >> $(REPO_DIR)/repodata/.repolst;done
	@echo "* fin $@ `date +%F.%H:%M:%S`"


repo.%: pre validate.% rpms.% $(REPO_DIR)/repodata/.repolst
	@echo "* run $@ `date +%F.%H:%M:%S`"
	if [ x"$*" = x"$(PRJ)" ];then \
		find $(REPO_DIR) -name '*-debug*.rpm' | xargs tar -zcf $(OUT)/debug-$(VERSION)-$(DATE).tgz; \
		$(TAR_CMD) $(OUT)/repo-$(VERSION)-$(DATE).tgz --exclude=*-debug* -C /opt repo; \
	fi
	@echo "* fin $@ `date +%F.%H:%M:%S`"


rpms.%:
	@echo "* run $@ `date +%F.%H:%M:%S`"
	@echo "* fin $@ `date +%F.%H:%M:%S`"


$(BUILD_ROOT)/tmpl.app.cur:
	@echo "* run $@ `date +%F.%H:%M:%S`"
	$(eval APP_DIR := $(BUILD_ROOT)/tmpl.app.$(DATE))
	$(eval APP_ROOT := $(APP_DIR)/rootfs)
	createrepo $(REPO_DIR)
	mkdir -p $(APP_ROOT)
	mkdir -p $(APP_ROOT)/var/lib/rpm
	mkdir -p $(APP_ROOT)/etc/yum.repos.d
	mkdir -p $(APP_ROOT)/service
	mkdir -p $(APP_ROOT)/$(REPO_DIR)
	-mkdir -p $(APP_ROOT)/proc
	-mkdir -p $(APP_ROOT)/dev
	mknod -m 666 $(APP_ROOT)/dev/random c 1 8
	mknod -m 666 $(APP_ROOT)/dev/urandom c 1 9
	mknod -m 666 $(APP_ROOT)/dev/tty c 5 0
	mknod -m 666 $(APP_ROOT)/dev/ptmx c 5 2
	mknod -m 600 $(APP_ROOT)/dev/console c 5 1
	mknod -m 666 $(APP_ROOT)/dev/zero c 1 5
	mknod -m 666 $(APP_ROOT)/dev/tty0 c 4 0
	mknod -m 666 $(APP_ROOT)/dev/tty1 c 4 1
	mknod -m 666 $(APP_ROOT)/dev/null c 1 3
	mount -o ro -t proc proc $(APP_ROOT)/proc
	mount -o bind,ro $(REPO_DIR) $(APP_ROOT)/$(REPO_DIR)
	@echo "$$REPO_OS"  >  $(APP_ROOT)/etc/yum.repos.d/os.repo
	@echo "$$REPO_VER" >> $(APP_ROOT)/etc/yum.repos.d/os.repo
	@echo "$$REPO_LOCAL" > $(APP_ROOT)/etc/yum.repos.d/local.repo
	@rpm --rebuilddb --root=$(APP_ROOT)
	yum --installroot=$(APP_ROOT) -y install yum
	yum --installroot=$(APP_ROOT) fs filter documentation
	yum --installroot=$(APP_ROOT) fs filter language en
	-rm -f $(APP_ROOT)/etc/yum.repos.d/CentOS*
	@echo "* $@ install common packages `date +%F.%H:%M:%S`"
	yum --installroot=$(APP_ROOT) -y install util-linux-ng passwd vim-minimal diffutils tar binutils
	umount $(APP_ROOT)/$(REPO_DIR)
	umount $(APP_ROOT)/proc
	ln -sf $(APP_DIR) $(BUILD_ROOT)/tmpl.app.cur
	@echo "* fin $@ `date +%F.%H:%M:%S`"


app.pre.%: $(BUILD_ROOT)/tmpl.app.cur
	@echo "* run $@ `date +%F.%H:%M:%S`"
	$(eval APP_DIR := $(BUILD_ROOT)/app.$*.$(DATE))
	$(eval APP_ROOT := $(APP_DIR)/rootfs)
	createrepo $(REPO_DIR)
	mkdir -p $(APP_DIR)
ifeq ($(USETMPFS),1)
	mount -t tmpfs tmpfs $(APP_DIR)
endif
	cp -a  $(BUILD_ROOT)/tmpl.app.cur/rootfs $(APP_ROOT)
	mount -o ro -t proc proc $(APP_ROOT)/proc
	mount -o bind,ro $(REPO_DIR) $(APP_ROOT)/$(REPO_DIR)
	if [ x"$*" = x"build" ];then \
		echo "$$REPO_SCL" >> $(APP_ROOT)/etc/yum.repos.d/scl.repo ;\
	fi
	chroot $(APP_ROOT) yum clean metadata
	chroot $(APP_ROOT) yum -y update
	@echo "* fin $@ `date +%F.%H:%M:%S`"


app.fs.%: app.pre.%
	@echo "* run $@ `date +%F.%H:%M:%S`"
	$(eval APP_ROOT := $(BUILD_ROOT)/app.$*.$(DATE)/rootfs)
	touch $(APP_ROOT)/sbin/init
	yum --installroot=$(APP_ROOT) -y install app-$(PRJ)-$*
	yum --installroot=$(APP_ROOT) -y install runit
	yum --installroot=$(APP_ROOT) clean all
	umount $(APP_ROOT)/$(REPO_DIR)
	umount $(APP_ROOT)/proc
	@for dir in /run /dev/shm /proc /sys /tmp /var/tmp /etc/app.d;do \
		if [ ! -d $(APP_ROOT)/$$dir ];then mkdir -p $(APP_ROOT)/$$dir; fi ;\
	done
	chmod 1777 $(APP_ROOT)/tmp
	chmod 1777 $(APP_ROOT)/var/tmp
	ln -sf /dev/shm $(APP_ROOT)/run/shm
	-@rm $(APP_ROOT)/etc/rc.d/rc[0-7].d/{S,K}*
	ln -sf /proc/mounts $(APP_ROOT)/etc/mtab
	@echo "* $@ remove locales `date +%F.%H:%M:%S`"
	chroot  $(APP_ROOT) localedef --list-archive| grep -v '^en_US' \
	| chroot $(APP_ROOT) xargs localedef --delete-from-archive
	mv $(APP_ROOT)/usr/lib/locale/locale-archive $(APP_ROOT)/usr/lib/locale/locale-archive.tmpl
	chroot  $(APP_ROOT) build-locale-archive
	find $(APP_ROOT)/usr/share/locale/ -maxdepth 1 -type d | grep -v '/en' | while read line;do \
		find $$line -type f -delete;\
	done
	@echo "* remove init.d `date +%F.%H:%M:%S`"
	-rm -f $(APP_ROOT)/etc/rc.d/init.d/*
	@echo "* $@ clean `date +%F.%H:%M:%S`"
	@-for dir in usr/games usr/share/games var/lib/games var/lib/yum var/cache/yum var/agentx;do \
		rm -rf $(APP_ROOT)/$$dir ;\
	done
	@echo "* fin $@ `date +%F.%H:%M:%S`"

app.%: validate.% repo.% app.fs.%
	@echo "* run $@ `date +%F.%H:%M:%S`"
	$(eval APP_DIR := $(BUILD_ROOT)/app.$*.$(DATE))
	$(eval APP_ROOT := $(BUILD_ROOT)/app.$*.$(DATE)/rootfs)
	-rm $(APP_ROOT)/etc/yum.repos.d/local.repo
	if [ x"$*" = x"build" ];then \
		echo 'PS1="[$(MAJOR).$(MINOR) \W]\\\$$ "' >> $(APP_ROOT)/root/.bashrc ;\
		$(TAR_CMD) $(OUT)/apt-$(PRJ)-$*-$(MAJOR).$(MINOR)-$(DATE).tgz -C $(APP_ROOT) . ;\
	else \
		$(TAR_CMD) $(OUT)/apt-$(PRJ)-$*-$(DATE).tgz -C $(APP_DIR) . ;\
	fi
ifeq ($(USETMPFS),1)
	umount $(APP_DIR)
endif
	@echo "* fin $@ `date +%F.%H:%M:%S`"

version:
	@echo "$(VERSION)"

