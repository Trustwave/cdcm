%define _unpackaged_files_terminate_build 0

Name:       tw-cdcm
Version:    1.0.0
Release:    1%{?dist}
License:    Various
Summary:    Credentialed Data Collection Module
BuildRequires: systemd
Requires: systemd
%description
Credentialed Data Collection Module

%clean
rm -rf %{buildroot}


%install
rm -rf %{buildroot}

[ -d %{buildroot} ] && rm -rf %{buildroot}
%define cdcm_lib /usr/share/cdcm/lib
%define cdcm_conf /etc/cdcm/
for dir in %{cdcm_lib} %{cdcm_lib}/plugins %{_bindir} %{cdcm_conf} /var/cdcm/log /var/cdcm/downloaded_files /usr/lib /tmp ;do
    [ -d %{buildroot}$dir ] || mkdir -p %{buildroot}$dir 
done
executables="cdcm_broker \
cdcm.sh \
cdcm_worker"

%define output_dir /opt/output/%{getenv:CI_COMMIT_BRANCH}/

for f in $executables;do
    [ -f %{output_dir}/bin/$f ] && install -m 755 %{output_dir}/bin/$f %{buildroot}%{_bindir}
done

libs="libtw-zmq.so \
libtw-sessions-cache.so \
libtw-smb-client.so \
libtw-singleton-runner.so \
libtw-logger.so \
libtw-common.so"

# copy libs
for f in $libs;do
    cp %{output_dir}/libs/$f %{buildroot}%{cdcm_lib}
done

# copy plugins
cp -r %{output_dir}/libs/plugins/* %{buildroot}%{cdcm_lib}/plugins/

# copy conf
cp -r /etc/cdcm/*  %{buildroot}%{cdcm_conf}


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
%{__install} -m644 %{_specdir}/%{name}.service %{buildroot}/%{_unitdir}/%{name}.service
ln -sf %{_sbindir}/service %{buildroot}/%{_sbindir}/rc%{name}

if [ -f /var/lib/systemd/migrated/%{name} ]; then
%service_add_pre %{name}.service
fi
%post
/sbin/ldconfig
/bin/systemctl --system daemon-reload &> /dev/null || :
/bin/systemctl --system enable  %{name} &> /dev/null || :

%postun
rm -rf /var/log/cdcm
/sbin/ldconfig
/bin/systemctl --system daemon-reload &> /dev/null || :


%files
%defattr(-,root,root,-)
%attr(755, root, root) %{_bindir}/cdcm_broker
%attr(755, root, root) %{_bindir}/cdcm_worker
%attr(755, root, root) %{_bindir}/cdcm.sh
%{cdcm_lib}/*.so*
%{cdcm_lib}/plugins/*
/var/cdcm
/etc/cdcm
%{_unitdir}/%{name}.service
%{_sbindir}/rc%{name}

%changelog
* Sun Sep 23 2019 <ychislov@trustwave.com> - 1.0-1
- Initial package
