Name:           vermouth
Version:        0.5.0
Release:        1%{?dist}
Summary:        Wine/Proton game launcher for Linux
License:        MIT
URL:            https://github.com/dekomote/vermouth

Source0: https://github.com/dekomote/vermouth/archive/refs/tags/v%{version}.tar.gz

BuildRequires:  cmake >= 3.20
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtquickcontrols2-devel

Requires:       qt6-qtbase
Requires:       qt6-qtdeclarative
Requires:       qt6-qtquickcontrols2
Requires:       qt6-qtsvg
Recommends:     icoutils

%description
A no-frills game launcher for Linux. Point it at Windows executables and
run them with Proton or Wine. Picks up Proton versions from Steam
automatically, supports launch options, icon extraction, and desktop
shortcut creation.

%prep
%autosetup

%build
%cmake
%cmake_build

%install
%cmake_install
install -Dm644 assets/vermouth.desktop %{buildroot}%{_datadir}/applications/vermouth.desktop
install -Dm644 assets/vermouth.svg %{buildroot}%{_datadir}/icons/hicolor/scalable/apps/vermouth.svg

%files
%license LICENSE
%{_bindir}/vermouth
%{_datadir}/applications/vermouth.desktop
%{_datadir}/icons/hicolor/scalable/apps/vermouth.svg

%changelog
* Thu Mar 26 2026 Dejan <dr.mote@gmail.com> - 0.5.0-1
- Initial package
