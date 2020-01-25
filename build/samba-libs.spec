%define rpmbuild        %(pwd)
%define _specdir        %{rpmbuild}/
%define _builddir       %{rpmbuild}/
%define _sourcedir      %{rpmbuild}/
%define _srcrpmdir      %{rpmbuild}/
%define _unpackaged_files_terminate_build    0

%global scl_name_prefix tw-
%global scl_name_base cdcm
%global scl_name_version 1
%global _scl_prefix /opt/tw
%global scl %{scl_name_prefix}%{scl_name_base}%{scl_name_version}
%{?scl:%scl_package cdcm}

%global debug_package %{nil}

Name:       %{scl_prefix}samba-libs
Version:    4.10.6
Release:    1%{?dist}
License:    GPLv3 and LGPLv3
Summary:    Package that installs %name
Provides:   samba-common = 	4.10.6
Provides:   samba-common-lbs = 4.10.6
Provides:   samba-client-libs = 4.10.6

%description
This is the samba-libs package for %scl Software Collection.

%prep
# (cd /sandbox/src/build; make deps)

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_libdir}

cond_copy(){
    set +e
    find %{_specdir}/../deps/samba-4.10.6/bin/ -name '*.so*' | while read line;do
    l=$line
	if [ -L "$l" ];then
        	link_base=`basename $l`
	        real_file=`readlink $l`
	        real_base=`basename $real_file`
		[ ! -f %{buildroot}%{_libdir}/${real_base} ] && [ -f $real_file ]  && cp $real_file  %{buildroot}%{_libdir}/
		[ ! -L %{buildroot}%{_libdir}/${link_base} ] && [ ! -f %{buildroot}%{_libdir}/${link_base} ] && ln -s %{_libdir}/${real_base} %{buildroot}%{_libdir}/${link_base}
	else
        	cp $l %{buildroot}%{_libdir}/
	fi
    done
    set -e
}

cond_copy
%files
%{_libdir}/*.so*

%changelog
* Sun Sep 23 2019 <ychislov@trustwave.com> - 1.0-1
- Initial package
