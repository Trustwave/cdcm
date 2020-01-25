%define rpmbuild        %(pwd)
%define _specdir        %{rpmbuild}/
%define _builddir       %{rpmbuild}/
%define _sourcedir      %{rpmbuild}/
%define _srcrpmdir      %{rpmbuild}/

%global scl_name_prefix tw-
%global scl_name_base cdcm
%global scl_name_version 1

%global scl %{scl_name_prefix}%{scl_name_base}%{scl_name_version}

# Optional but recommended: define nfsmountable
%global nfsmountable 0

%global _scl_prefix /opt/tw
%scl_package %scl

%global debug_package %{nil}

Summary: Package that installs %scl
Name: %scl_name
Version: 1.0
Release: 1%{?dist}
License: GPLv3
Requires: %{scl_prefix}cdcm
BuildRequires: scl-utils-build

%description
This is the main package for %scl Software Collection.

%package runtime
Summary: Package that handles %scl Software Collection.
Requires: scl-utils

%description runtime
Package shipping essential scripts to work with %scl Software Collection.

%package build
Summary: Package shipping basic build configuration
Requires: scl-utils-build

%description build
Package shipping essential configuration macros to build %scl Software Collection.

# This is only needed when you want to provide an optional scldevel subpackage
#%package scldevel
#Summary: Package shipping development files for %scl
#
#%description scldevel
#Package shipping development files, especially useful for development of
#packages depending on %scl Software Collection.

%prep
%setup -c -T

%install
%scl_install

cat >> %{buildroot}%{_scl_scripts}/enable << EOF
export PATH="%{_bindir}:%{_sbindir}\${PATH:+:\${PATH}}"
export LD_LIBRARY_PATH="%{_libdir}\${LD_LIBRARY_PATH:+:\${LD_LIBRARY_PATH}}"
export MANPATH="%{_mandir}:\${MANPATH:-}"
export PKG_CONFIG_PATH="%{_libdir}/pkgconfig\${PKG_CONFIG_PATH:+:\${PKG_CONFIG_PATH}}"
EOF

# This is only needed when you want to provide an optional scldevel subpackage
#cat >> %{buildroot}%{_root_sysconfdir}/rpm/macros.%{scl_name_base}-scldevel << EOF
#%%scl_%{scl_name_base} %{scl}
#%%scl_prefix_%{scl_name_base} %{scl_prefix}
#EOF

%files

# %%files runtime -f filelist
%files runtime
%scl_files

%files build
%{_root_sysconfdir}/rpm/macros.%{scl}-config

#%files scldevel
#%{_root_sysconfdir}/rpm/macros.%{scl_name_base}-scldevel

%changelog
* Sun Sep 22 2019 <ychislov@trustwave.com> - 1.0-1
- Initial package
