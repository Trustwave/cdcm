%define _unpackaged_files_terminate_build 0

Name:       tw-cdcm
Version:    1.0.0
Release:    1%{?dist}
License:    Various
Summary:    Credentialed Data Collection Module

%description
Credentialed Data Collection Module

%clean
rm -rf %{buildroot}

%install
rm -rf %{buildroot}

[ -d %{buildroot} ] && rm -rf %{buildroot}
%define cdcm_conf /etc/cdcm/
for dir in /usr/share/cdcm/lib %{_bindir} %{cdcm_conf} /var/cdcm/log /var/cdcm/downloaded_files /usr/lib ;do
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
    cp /opt/output/libs/$f %{buildroot}/usr/share/cdcm/lib
done

# copy conf
cp -r /etc/cdcm/*  %{buildroot}%{cdcm_conf}

set +e
find %{_specdir}/../deps/samba-4.10.6/bin/ -name '*.so*' | while read line;do
l=$line
if [ -L "$l" ];then
    link_base=`basename $l`
    real_file=`readlink $l`
    real_base=`basename $real_file`
    [ ! -f %{buildroot}/usr/share/cdcm/lib/${real_base} ] && [ -f $real_file ]  && cp $real_file  %{buildroot}/usr/share/cdcm/lib/
    [ ! -L %{buildroot}/usr/share/cdcm/lib/${link_base} ] && [ ! -f %{buildroot}/usr/share/cdcm/lib/${link_base} ] && ln -s /usr/share/cdcm/lib/${real_base} %{buildroot}/usr/share/cdcm/lib/${link_base}
else
    cp $l %{buildroot}/usr/share/cdcm/lib/
fi
done
set -e

%files
%defattr(-,root,root,-)
%attr(755, root, root) %{_bindir}/cdcm_broker
%attr(755, root, root) %{_bindir}/cdcm_worker
%attr(755, root, root) %{_bindir}/cdcm.sh
/usr/share/cdcm/lib/*.so*
/var/cdcm
/etc/cdcm

%changelog
* Sun Sep 23 2019 <ychislov@trustwave.com> - 1.0-1
- Initial package
