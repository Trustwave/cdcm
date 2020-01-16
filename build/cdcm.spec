%global scl_name_prefix tw-
%global scl_name_base cdcm
%global scl_name_version 1

%global _scl_prefix /opt/tw
%global scl %{scl_name_prefix}%{scl_name_base}%{scl_name_version}
%{?scl:%scl_package cdcm}

%define _unpackaged_files_terminate_build 0

Name:       %{scl_prefix}cdcm
Version:    1.0-3
Release:    1%{?dist}
License:    Trustwave   
Summary:    Package that installs %name
Requires:   %{scl_prefix}samba-libs

 

%description
This is the cdcm package for %scl Software Collection.

%prep

%clean
rm -rf %{buildroot}

%install
[ -d %{buildroot} ] && rm -rf %{buildroot}
%define cdcm_conf /var/cdcm/conf
for dir in %{_libdir} %{_bindir} %{cdcm_conf} /var/cdcm/log /usr/lib ;do  
    [ -d %{buildroot}$dir ] || mkdir -p %{buildroot}$dir 
done
executables="cdcm_broker \
cdcm.sh \
cdcm_worker"

for f in $executables;do
    [ -f /opt/output/bin/$f ] && install -m 755 /opt/output/bin/$f %{buildroot}%{_bindir}
done

libs="libtw-zmq.so \
libtw-sessions-cache.so \
libtw-smb-client.so \
libtw-singleton-runner.so \
libtw-logger.so \
libtw-common.so"

# copy libs
for f in $libs;do
    cp /opt/output/libs/$f %{buildroot}%{_libdir}
done

# copy conf
cp -r /var/cdcm/conf/*  %{buildroot}%{cdcm_conf}

%files
%defattr(-,root,root,-)
%attr(755, root, root) %{_bindir}/cdcm_broker
%attr(755, root, root) %{_bindir}/cdcm_worker
%attr(755, root, root) %{_bindir}/cdcm.sh
%{_libdir}/*.so
/var/cdcm

%changelog
* Sun Sep 23 2019 <ychislov@trustwave.com> - 1.0-1
- Initial package
