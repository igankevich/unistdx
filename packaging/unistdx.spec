Name:           unistdx
Version:        0.1
Release:        1%{?dist}
Summary:        A library of C++ abstractions for libc.

License:        GPLv2
URL:            http://igankevich.com/
Source0:        unistdx-%{version}.tar.gz

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
%setup


%build


%install
rm -rf $RPM_BUILD_ROOT
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}/unistdx/sys
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}/unistdx/sys/bits
install -m 644 \
sys/argstream.hh \
sys/cmdline.hh \
sys/dir.hh \
sys/endpoint.hh \
sys/event.hh \
sys/fildesbuf.hh \
sys/fildes.hh \
sys/file.hh \
sys/ifaddr.hh \
sys/ifaddr_list.hh \
sys/log.hh \
sys/network_format.hh \
sys/packetstream.hh \
sys/path.hh \
sys/pipe.hh \
sys/process.hh \
sys/semaphore.hh \
sys/sharedmem.hh \
sys/shmembuf.hh \
sys/signal.hh \
sys/signal_semaphore.hh \
sys/socket.hh \
sys/subnet_iterator.hh \
sys/system.hh \
$RPM_BUILD_ROOT/%{_includedir}/unistdx/sys
install -m 644 \
sys/bits/basic_istream_iterator.hh \
sys/bits/bit_count.hh \
sys/bits/check.hh \
sys/bits/endpoint_parse.hh \
sys/bits/ifaddrs_iterator.hh \
sys/bits/macros.hh \
sys/bits/safe_calls.hh \
$RPM_BUILD_ROOT/%{_includedir}/unistdx/sys/bits
install -m 755 -d $RPM_BUILD_ROOT/%{_libdir}
install -m 755 -d $RPM_BUILD_ROOT/%{_libdir}/pkgconfig
install -m 644 packaging/unistdx.pc $RPM_BUILD_ROOT/%{_libdir}/pkgconfig


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files

%files devel
%{_includedir}/unistdx/sys/argstream.hh
%{_includedir}/unistdx/sys/cmdline.hh
%{_includedir}/unistdx/sys/dir.hh
%{_includedir}/unistdx/sys/endpoint.hh
%{_includedir}/unistdx/sys/event.hh
%{_includedir}/unistdx/sys/fildesbuf.hh
%{_includedir}/unistdx/sys/fildes.hh
%{_includedir}/unistdx/sys/file.hh
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
%{_includedir}/unistdx/sys/bits/basic_istream_iterator.hh
%{_includedir}/unistdx/sys/bits/bit_count.hh
%{_includedir}/unistdx/sys/bits/check.hh
%{_includedir}/unistdx/sys/bits/endpoint_parse.hh
%{_includedir}/unistdx/sys/bits/ifaddrs_iterator.hh
%{_includedir}/unistdx/sys/bits/macros.hh
%{_includedir}/unistdx/sys/bits/safe_calls.hh
%{_libdir}/pkgconfig/unistdx.pc

%changelog
* Sat Sep 4 2016 0.1 Ivan Gankevich <igankevich@ya.ru>
- Packaged everything.
