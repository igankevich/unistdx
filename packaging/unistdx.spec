Name:           unistdx
Version:        0.3
Release:        1%{?dist}
Summary:        A library of C++ abstractions for libc.

License:        GPLv2
URL:            http://igankevich.com/
Source0:        unistdx-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
A library of C++ abstractions for libc.


%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.


%global debug_package %{nil}


%prep
%autosetup


%build
%meson
%meson_build


%install
%meson_install

%check
%meson_test


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libunistdx.so

%files devel
%{_includedir}/unistdx/*
%{_includedir}/unistdx/bits/*
%{_includedir}/unistdx/net/*
%{_includedir}/unistdx/base/*
%{_includedir}/unistdx/fs/*
%{_includedir}/unistdx/io/*
%{_includedir}/unistdx/ipc/*
%{_includedir}/unistdx/it/*
%{_includedir}/unistdx/net/*
%{_includedir}/unistdx/util/*
%{_libdir}/pkgconfig/unistdx.pc
%{_libdir}/libunistdx.a

%changelog
* Sat Sep 4 2016 0.1 Ivan Gankevich <igankevich@ya.ru>
- Packaged everything.
