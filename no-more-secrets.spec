Name:           no-more-secrets
Version:        0.2.0
Release:        1%{?dist}
Summary:        Infamous "decrypting text" effect 
Group:          Amusements/Games
License:        GPLv3
URL:            https://github.com/bartobri/no-more-secrets
Source0: 	https://github.com/bartobri/no-more-secrets/archive/v%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires:  ncurses-devel

%description
"No More Secrets" is the name I've lovingly given to the infamous "decrypting text" effect seen on screen in the 1992 hacker movie Sneakers.

%prep
%setup -q -n %{name}-%{version}

%build
make

%install
rm -rf %{buildroot}
install -d m0755 %{buildroot}%{_bindir}
make install prefix=%{buildroot}/usr

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
/usr/bin/nms
/usr/bin/sneakers

%changelog
* Fri May 20 2016 Thomas Guthmann <tguthmann@iseek.com.au> - 0.2
- Already a new version!

* Wed May 18 2016 Thomas Guthmann <tguthmann@iseek.com.au> - 0.1
- Initial release

