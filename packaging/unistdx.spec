Name:           unistdx
Version:        0.2
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
%{_includedir}/unistdx/sys/argstream.hh
%{_includedir}/unistdx/sys/cmdline.hh
%{_includedir}/unistdx/sys/dir.hh
%{_includedir}/unistdx/sys/endpoint.hh
%{_includedir}/unistdx/sys/event.hh
%{_includedir}/unistdx/sys/fildes.hh
%{_includedir}/unistdx/sys/fildesbuf.hh
%{_includedir}/unistdx/sys/file.hh
%{_includedir}/unistdx/sys/file_mutex.hh
%{_includedir}/unistdx/sys/groups.hh
%{_includedir}/unistdx/sys/ifaddr.hh
%{_includedir}/unistdx/sys/ifaddr_list.hh
%{_includedir}/unistdx/sys/log.hh
%{_includedir}/unistdx/sys/network_format.hh
%{_includedir}/unistdx/sys/packetstream.hh
%{_includedir}/unistdx/sys/path.hh
%{_includedir}/unistdx/sys/pipe.hh
%{_includedir}/unistdx/sys/process.hh
%{_includedir}/unistdx/sys/semaphore.hh
%{_includedir}/unistdx/sys/sharedmem.hh
%{_includedir}/unistdx/sys/shmembuf.hh
%{_includedir}/unistdx/sys/signal.hh
%{_includedir}/unistdx/sys/signal_semaphore.hh
%{_includedir}/unistdx/sys/socket.hh
%{_includedir}/unistdx/sys/subnet_iterator.hh
%{_includedir}/unistdx/sys/system.hh
%{_includedir}/unistdx/sys/users.hh
%{_includedir}/unistdx/sys/bits/basic_istream_iterator.hh
%{_includedir}/unistdx/sys/bits/basic_ostream_iterator.hh
%{_includedir}/unistdx/sys/bits/bit_count.hh
%{_includedir}/unistdx/sys/bits/check.hh
%{_includedir}/unistdx/sys/bits/endpoint_parse.hh
%{_includedir}/unistdx/sys/bits/ifaddrs_iterator.hh
%{_includedir}/unistdx/sys/bits/macros.hh
%{_includedir}/unistdx/sys/bits/safe_calls.hh
%{_libdir}/pkgconfig/unistdx.pc
%{_libdir}/libunistdx.a

%changelog
* Sat Sep 4 2016 0.1 Ivan Gankevich <igankevich@ya.ru>
- Packaged everything.
