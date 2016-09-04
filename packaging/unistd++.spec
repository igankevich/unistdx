Name:           unistd++
Version:        0.1
Release:        1%{?dist}
Summary:        A library of C++ abstractions for libc.

License:        GPLv2
URL:            http://igankevich.com/
Source0:        unistd++-%{version}.tar.gz

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
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}/unistd++/sys
install -m 755 -d $RPM_BUILD_ROOT/%{_includedir}/unistd++/sys/bits
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
$RPM_BUILD_ROOT/%{_includedir}/unistd++/sys
install -m 644 \
sys/bits/basic_istream_iterator.hh \
sys/bits/bit_count.hh \
sys/bits/check.hh \
sys/bits/endpoint_parse.hh \
sys/bits/ifaddrs_iterator.hh \
sys/bits/macros.hh \
sys/bits/safe_calls.hh \
$RPM_BUILD_ROOT/%{_includedir}/unistd++/sys/bits
install -m 755 -d $RPM_BUILD_ROOT/%{_libdir}
install -m 755 -d $RPM_BUILD_ROOT/%{_libdir}/pkgconfig
install -m 644 packaging/unistd++.pc $RPM_BUILD_ROOT/%{_libdir}/pkgconfig


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files

%files devel
%{_includedir}/unistd++/sys/argstream.hh
%{_includedir}/unistd++/sys/cmdline.hh
%{_includedir}/unistd++/sys/dir.hh
%{_includedir}/unistd++/sys/endpoint.hh
%{_includedir}/unistd++/sys/event.hh
%{_includedir}/unistd++/sys/fildesbuf.hh
%{_includedir}/unistd++/sys/fildes.hh
%{_includedir}/unistd++/sys/file.hh
%{_includedir}/unistd++/sys/ifaddr.hh
%{_includedir}/unistd++/sys/ifaddr_list.hh
%{_includedir}/unistd++/sys/log.hh
%{_includedir}/unistd++/sys/network_format.hh
%{_includedir}/unistd++/sys/packetstream.hh
%{_includedir}/unistd++/sys/path.hh
%{_includedir}/unistd++/sys/pipe.hh
%{_includedir}/unistd++/sys/process.hh
%{_includedir}/unistd++/sys/semaphore.hh
%{_includedir}/unistd++/sys/sharedmem.hh
%{_includedir}/unistd++/sys/shmembuf.hh
%{_includedir}/unistd++/sys/signal.hh
%{_includedir}/unistd++/sys/signal_semaphore.hh
%{_includedir}/unistd++/sys/socket.hh
%{_includedir}/unistd++/sys/subnet_iterator.hh
%{_includedir}/unistd++/sys/system.hh
%{_includedir}/unistd++/sys/bits/basic_istream_iterator.hh
%{_includedir}/unistd++/sys/bits/bit_count.hh
%{_includedir}/unistd++/sys/bits/check.hh
%{_includedir}/unistd++/sys/bits/endpoint_parse.hh
%{_includedir}/unistd++/sys/bits/ifaddrs_iterator.hh
%{_includedir}/unistd++/sys/bits/macros.hh
%{_includedir}/unistd++/sys/bits/safe_calls.hh
%{_libdir}/pkgconfig/unistd++.pc

%changelog
* Sat Sep 4 2016 0.1 Ivan Gankevich <igankevich@ya.ru>
- Packaged everything.
