Name:		app-installers
Summary:	Application installers
Version:	1.0
Release:	0
Group:		App Framework/Application Installer
License:	MIT
Source0:	%{name}-%{version}.tar.gz
BuildRequires:  libcap-devel
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-device)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(pkgmgr-installer)
BuildRequires:  pkgconfig(app2sd)
BuildRequires:  pkgconfig(libwbxml2)
BuildRequires:  pkgconfig(libprivilege-control)
BuildRequires:  pkgconfig(dpl-efl)
BuildRequires:  pkgconfig(cert-svc)
BuildRequires:  pkgconfig(cert-svc-vcore)
BuildRequires:  pkgconfig(appcore-common)
BuildRequires:  pkgconfig(vconf)

Requires(post): privilege-checker

Requires:       wgt-installer

%description
This is a meta package that installs the common application
installers of Tizen.

#######################################################
%package -n wgt-installer
Summary: Installer of WGT files

%description -n wgt-installer
Installer of standard widget files WGT

#######################################################
%prep
%setup -q

%build 
%reconfigure
%__make %{?jobs:-j%jobs}

%install
%make_install

mkdir -p %{buildroot}/etc/package-manager/backend
ln -s /usr/bin/wgt-installer %{buildroot}/etc/package-manager/backend/wgt


#######################################################
%post -n wgt-installer -p /sbin/ldconfig

%files -n wgt-installer
%manifest wgt-installer.manifest
%license COPYING
/usr/bin/wgt-installer
/etc/package-manager/backend/wgt

