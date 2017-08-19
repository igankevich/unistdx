#include "two_way_pipe"
#include <unistdx/base/make_object>

void
sys::two_way_pipe::validate() {
	if (this->is_owner()) {
		this->parent_in().validate();
		this->parent_out().validate();
	} else {
		this->child_in().validate();
		this->child_out().validate();
	}
}

void
sys::two_way_pipe::open() {
	this->_pipe1.open();
	this->_pipe2.open();
}

void
sys::two_way_pipe::close() {
	this->_pipe1.close();
	this->_pipe2.close();
}

void
sys::two_way_pipe::close_in_child() {
	this->_pipe1.in().close();
	this->_pipe2.out().close();
}

void
sys::two_way_pipe::close_in_parent() {
	this->_pipe1.out().close();
	this->_pipe2.in().close();
}

void
sys::two_way_pipe::remap_in_child(fd_type in, fd_type out) {
	this->child_in().remap(in);
	this->child_out().remap(out);
}

void
sys::two_way_pipe::close_unused() {
	this->is_owner() ? this->close_in_parent() : this->close_in_child();
}

std::ostream&
sys::operator<<(std::ostream& out, const two_way_pipe& rhs) {
	return out << make_object("pipe1", rhs._pipe1, "pipe2", rhs._pipe2);
}

