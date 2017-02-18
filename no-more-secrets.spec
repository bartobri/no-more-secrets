Name:           no-more-secrets
Version:        0.3.1
Release:        1%{?dist}
Summary:        A recreation of the "decrypting text" effect from the 1992 movie Sneakers
Group:          Amusements/Games
License:        GPL-3.0+
URL:            https://github.com/bartobri/no-more-secrets
Source0: 	https://github.com/bartobri/no-more-secrets/archive/v%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires:  ncurses-devel

%description
A tool set to recreate the famous "decrypting text" effect as seen in the 1992 movie Sneakers.

%prep
%setup -q -n %{name}-%{version}

%build
make nms sneakers-ncurses

%install
install -d m0755 %{buildroot}%{_bindir}
make install prefix=%{buildroot}/usr

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/nms
%{_bindir}/sneakers
%{_mandir}/man6/nms.6%{ext_man}
%{_mandir}/man6/sneakers.6%{ext_man}

%changelog
* Sat Feb 17 2017 Ferdinand Thiessen <rpm@fthiessen.de> - 0.3.1
- Update to 0.3.1
- Added manpages

* Fri May 20 2016 Thomas Guthmann <tguthmann@iseek.com.au> - 0.2
- Already a new version!

* Wed May 18 2016 Thomas Guthmann <tguthmann@iseek.com.au> - 0.1
- Initial release

