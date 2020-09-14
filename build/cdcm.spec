%define _unpackaged_files_terminate_build 0
%global __provides_exclude ^.*.so.*$
%define __requires_exclude ^.*.so.*$
%{!?pkg_version:%define pkg_version 1.0.0}
%{!?release:%define release 1}
Name: cdcm
Version: %{pkg_version}
Release: %{release}
License: Various
Vendor: Trustwave Inc.
URL: https://www.trustwave.com/
Summary: Credentialed Data Collection Module
BuildRequires: systemd
Requires: systemd
Requires: zeromq
Requires: openpgm
Requires: libsodium
Requires: boost169-filesystem
Requires: boost169-log
Requires: boost169-python3
Requires: gnutls
Requires: python36
Requires: python36-pyasn1
Requires: python36-six
Requires: python36-pycryptodomex
Requires: python36-pyOpenSSL
Requires: python36-ldap3
Requires: python36-flask


%description
Credentialed Data Collection Module

%clean
rm -rf %{buildroot}

%prep
cp -rf  %{_specdir}/../deps/impacket %{_topdir}/BUILD/
%build
cd %{_topdir}/BUILD/impacket
python3.6 setup.py build

%install
rm -rf %{buildroot}

[ -d %{buildroot} ] && rm -rf %{buildroot}
%define cdcm_lib /usr/share/cdcm/lib
%define cdcm_conf /etc/cdcm/
for dir in %{cdcm_lib} %{cdcm_lib}/plugins %{_bindir} %{cdcm_conf} /var/log/cdcm /var/cdcm/downloaded_files /usr/lib /tmp ;do
    [ -d %{buildroot}$dir ] || mkdir -p %{buildroot}$dir 
done
executables="cdcm_broker \
cdcm_supervisor \
cdcm_worker"


%define output_dir /opt/output/%{getenv:CI_COMMIT_REF_SLUG}/

for f in $executables;do
    [ -f %{output_dir}/bin/$f ] && install -m 755 %{output_dir}/bin/$f %{buildroot}%{_bindir}
done

libs="libtw-zmq.so \
libtw-sessions-cache.so \
libtw-smb-client.so \
libtw-singleton-runner.so \
libtw-logger.so \
libtw-common.so \
libtw-lsa-client.so \
libtw-rpc-client.so \
libtw-registry-client2.so \
libtw-registry-utils.so \
libtw-srvsvc-client.so \
libtw-wmi_registry-client.so \
libtw-wmi-wql-client.so \
libtw-wmi-common.so \
libtw-session-to-clients.so \
libtw-sd-utils.so"

# copy libs
for f in $libs;do
    cp %{output_dir}/lib/$f %{buildroot}%{cdcm_lib}
done

# copy plugins
cp -r %{output_dir}/lib/plugins/* %{buildroot}%{cdcm_lib}/plugins/

# copy conf
cp -r %{output_dir}/conf/*  %{buildroot}%{cdcm_conf}


set +e
find %{_specdir}/../deps/samba-4.10.6/bin/ -name '*.so*' | while read line;do
l=$line
if [ -L "$l" ];then
    link_base=`basename $l`
    real_file=`readlink $l`
    real_base=`basename $real_file`
    [ ! -f %{buildroot}%{cdcm_lib}/${real_base} ] && [ -f $real_file ]  && cp $real_file  %{buildroot}%{cdcm_lib}/
    [ ! -L %{buildroot}%{cdcm_lib}/${link_base} ] && [ ! -f %{buildroot}%{cdcm_lib}/${link_base} ] && ln -s %{cdcm_lib}/${real_base} %{buildroot}%{cdcm_lib}/${link_base}
else
    cp $l %{buildroot}%{cdcm_lib}/
fi
done
set -e

%{__mkdir} -p %{buildroot}%{_unitdir}
%{__mkdir} -p %{buildroot}/%{_sbindir}
%{__mkdir} -p %{buildroot}/%{_presetdir}
%{__install} -m644 %{_specdir}/%{name}.service %{buildroot}/%{_unitdir}/%{name}.service
%{__install} -m644 %{_specdir}/50-%{name}.preset %{buildroot}/%{_presetdir}/50-%{name}.preset
ln -sf %{_sbindir}/service %{buildroot}/%{_sbindir}/rc%{name}

cd  %{_topdir}/BUILD/impacket
python3.6 setup.py install --single-version-externally-managed -O1 --root=%{buildroot} --record=INSTALLED_FILES

%post
/sbin/ldconfig
%systemd_post %{name}.service
%systemd_user_post %{name}.service
systemctl daemon-reload >/dev/null 2>&1 || :
systemctl start %{name}

%preun
%systemd_preun %{name}.service
%systemd_user_preun %{name}.service
systemctl stop %{name}
rm -fr /dev/shm/sessions_lock

%postun
%systemd_postun %{name}.service
systemctl daemon-reload >/dev/null 2>&1 || :
if [ $1 -ge 1 ] ; then
        systemctl restart  %{name}.service >/dev/null 2>&1 || :
fi

%files -f %{_topdir}/BUILD/impacket/INSTALLED_FILES
%defattr(-,root,root,-)
%attr(755, root, root) %{_bindir}/cdcm_broker
%attr(755, root, root) %{_bindir}/cdcm_supervisor
%attr(755, root, root) %{_bindir}/cdcm_worker

%{cdcm_lib}/*.so*
%{cdcm_lib}/plugins/*
/var/cdcm
/etc/cdcm
%{_unitdir}/%{name}.service
%{_presetdir}/50-%{name}.preset
%{_sbindir}/rc%{name}

%changelog
* Sun Sep 23 2019 <ychislov@trustwave.com> - 1.0-1
- Initial package
