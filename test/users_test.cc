#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdx/debug.hh>
#include <sys/users.hh>
#include <sys/groups.hh>
#include <sys/process.hh>

void
test_users() {
	sys::userstream users;
	std::copy(
		sys::user_iterator(users),
		sys::user_iterator(),
		std::ostream_iterator<sys::user>(std::cout, "\n")
	);
}

void
test_groups() {
	sys::groupstream groups;
	std::copy(
		sys::group_iterator(groups),
		sys::group_iterator(),
		std::ostream_iterator<sys::group>(std::cout, "\n")
	);
}

void
test_groups_with_two_members() {
	sys::groupstream groups;
	std::copy_if(
		sys::group_iterator(groups),
		sys::group_iterator(),
		std::ostream_iterator<sys::group>(std::cout, "\n"),
		[] (const sys::group rhs) {
			return rhs.size() >= 2;
		}
	);
}

void
test_groups_of_the_current_user() {
	sys::userstream users;
	sys::user_iterator end;
	sys::user_iterator result = std::find_if(
		sys::user_iterator(users),
		end,
		[] (const sys::user rhs) {
			return rhs.id() == sys::this_process::user();
		}
	);
	if (result == end) {
		throw std::runtime_error("bad user");
	}
	std::string username = result->name();
	sys::groupstream groups;
	std::copy_if(
		sys::group_iterator(groups),
		sys::group_iterator(),
		std::ostream_iterator<sys::group>(std::cout, "\n"),
		[&username] (const sys::group rhs) {
			auto result = std::find_if(
				rhs.begin(),
				rhs.end(),
				[&username] (const char* member) {
					return username == member;
				}
			);
			return result != rhs.end();
		}
	);
}

int main() {
	std::cout << "all users" << std::endl;
	test_users();
	std::cout << "all groups" << std::endl;
	test_groups();
	std::cout << "all groups with at least two members" << std::endl;
	test_groups_with_two_members();
	std::cout << "all groups in which the current user is a member" << std::endl;
	test_groups_of_the_current_user();
	return 0;
}
