%global _enable_debug_package 0
%global debug_package %{nil}

Summary:            MQTT C Client
Name:               paho-c
Version:            1.2.0
Release:            0%{?dist}
License:            Eclipse Distribution License 1.0 and Eclipse Public License 1.0
Group:              Development/Tools
Source:             paho-c-%{version}.tar.gz
URL:                https://eclipse.org/paho/clients/c/
BuildRequires:      cmake
BuildRequires:      gcc
BuildRequires:      graphviz
BuildRequires:      doxygen
BuildRequires:      openssl-devel
Requires:           openssl


%description
The Paho MQTT C Client is a fully fledged MQTT client written in ANSI standard C.


%package devel
Summary:            MQTT C Client development kit
Group:              Development/Libraries
Requires:           paho-c

%description devel
Development files and samples for the the Paho MQTT C Client.


%package devel-docs
Summary:            MQTT C Client development kit documentation
Group:              Development/Libraries

%description devel-docs
Development documentation files for the the Paho MQTT C Client.

%prep
%autosetup -n paho-c-%{version}

%build
mkdir build.paho && cd build.paho
cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE -DCMAKE_INSTALL_PREFIX=%{buildroot}/usr ..
make

%install
cd build.paho
make install

%files
%doc edl-v10 epl-v10
%{_libdir}/*

%files devel
%{_bindir}/*
%{_includedir}/*

%files devel-docs
%{_datadir}/*

%changelog
* Sat Dec 31 2016 Otavio R. Piske <opiske@redhat.com> - 20161231
- Initial packaging
