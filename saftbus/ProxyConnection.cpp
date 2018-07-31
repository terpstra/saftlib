#include "ProxyConnection.h"
#include "Proxy.h"
#include "saftbus.h"
#include "core.h"

#include <iostream>
#include <sstream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <errno.h>
#include <algorithm>
#include <ctime>

namespace saftbus
{

ProxyConnection::ProxyConnection(const Glib::ustring &base_name)
{
	int _debug_level = 1;
	// try to open first available socket
	_create_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (_create_socket > 0) {
		if (_debug_level) std::cerr << "socket created" << std::endl;
	} // TODO: else { ... }
	_address.sun_family = AF_LOCAL;

	for (int i = 0; i < 100; ++i)
	{
		std::ostringstream name_out;
		name_out << base_name << std::setw(2) << std::setfill('0') << i;
		strcpy(_address.sun_path, name_out.str().c_str());
		if (_debug_level) std::cerr << "try to connect to " << name_out.str() << std::endl;
		int connect_result = connect( _create_socket, (struct sockaddr *)&_address , sizeof(_address));
		if (connect_result == 0) {
			if (_debug_level) std::cerr << "connection established" << std::endl;
			//char msg = 't';
			//int result = send(_create_socket, &msg, 1, MSG_DONTWAIT); 
			//if (_debug_level) std::cerr << "result = " << result << std::endl;
			break;
		} else {
			if (_debug_level) std::cerr << "connect_result = " << connect_result << std::endl;
			if (connect_result == -1) {
				if (_debug_level) std::cerr << "  errno = " << errno << " -> " << strerror(errno) << std::endl;
				switch(errno) {
					case ECONNREFUSED:
						continue;
					break;
					case ENOENT:
						throw std::runtime_error("all sockets busy");
					break;
					default:
						throw std::runtime_error("unknown error");
				}
			} 
		}
	}
    Glib::signal_io().connect(sigc::mem_fun(*this, &ProxyConnection::dispatch), _create_socket, Glib::IO_IN | Glib::IO_HUP, Glib::PRIORITY_HIGH);
    std::ostringstream id_out;
    id_out << this;
    _saftbus_id = id_out.str();
}



Glib::VariantContainerBase& ProxyConnection::call_sync (const Glib::ustring& object_path, const Glib::ustring& interface_name, const Glib::ustring& name, const Glib::VariantContainerBase& parameters, const Glib::ustring& bus_name, int timeout_msec)
{
	if (_debug_level) std::cerr << "ProxyConnection::call_sync(" << object_path << "," << interface_name << "," << name << ") called" << std::endl;
	// first append message meta informations like: type of message, recipient, sender, interface name
	std::vector<Glib::VariantBase> message;
	message.push_back(Glib::Variant<Glib::ustring>::create(object_path));
	message.push_back(Glib::Variant<Glib::ustring>::create(_saftbus_id));
	message.push_back(Glib::Variant<Glib::ustring>::create(interface_name));
	message.push_back(Glib::Variant<Glib::ustring>::create(name)); // name can be method_name or property_name
	message.push_back(parameters);
	// then convert inot a variant vector type
	Glib::Variant<std::vector<Glib::VariantBase> > var_message = Glib::Variant<std::vector<Glib::VariantBase> >::create(message);
	// serialize
	guint32 size = var_message.get_size();
	const char *data_ptr =  static_cast<const char*>(var_message.get_data());
	// send over socket
	saftbus::write(get_fd(), saftbus::METHOD_CALL);
	saftbus::write(get_fd(), size);
	saftbus::write_all(get_fd(), data_ptr, size);

	// receive from socket
	saftbus::MessageTypeS2C type = saftbus::METHOD_REPLY;
	while(!expect_from_server(type));
	//saftbus::read(get_fd(), type);
	if (_debug_level) std::cerr << "got response " << type << std::endl;
	saftbus::read(get_fd(), size);
	_call_sync_result_buffer.resize(size);
	saftbus::read_all(get_fd(), &_call_sync_result_buffer[0], size);

	// deserialize
	//Glib::Variant<std::vector<Glib::VariantBase> > payload;
	deserialize(_call_sync_result, &_call_sync_result_buffer[0], _call_sync_result_buffer.size());

	if (_debug_level) std::cerr << "ProxyConnection::call_sync respsonse = " << _call_sync_result.print() << std::endl;
	//Glib::VariantBase result    = Glib::VariantBase::cast_dynamic<Glib::VariantBase >(payload.get_child(0));
	// std::cerr << " payload.get_type_string() = " << _call_sync_result.get_type_string() << "     .value = " << _call_sync_result.print() << std::endl;
	// for (unsigned n = 0; n < _call_sync_result.get_n_children(); ++n)
	// {
	// 	Glib::VariantBase child = _call_sync_result.get_child(n);
	// 	std::cerr << "     parameter[" << n << "].type = " << child.get_type_string() << "    .value = " << child.print() << std::endl;
	// }
	// std::cerr << "just before returning " << _call_sync_result.print() << std::endl;
	return _call_sync_result;					
}

bool ProxyConnection::expect_from_server(MessageTypeS2C expected_type)
{
	// This is called in case a specific answer is expected from the server.
	// It can happen that the server sends a signal instead
	// because signals can asynchronously be triggered by the hardware.
	// If a signal is received instead of the expected response, a proper
	// signal handling is done. If the expected response finally arrives,
	// the function returns.
	MessageTypeS2C type;
	while (true)
	{
		int result = saftbus::read(get_fd(), type);
		if (result == -1) {
			return false;
		}
		if (type == expected_type) {
				if (_debug_level) std::cerr << "ProxyConnection::expect_from_server() got expected type" << std::endl;
				return true;
		} else if (type == saftbus::SIGNAL) {
			if (_debug_level) std::cerr << "got a SIGNAL **************************************" << std::endl;
			dispatchSignal();
			continue;
		} else {
			if (_debug_level) std::cerr << "unexpected type " << type << " while expecting " << expected_type << std::endl;
			return false;
		}
	}
	return false;
}

void ProxyConnection::register_proxy(Glib::ustring interface_name, Glib::ustring object_path, Proxy *proxy)
{
	if (_debug_level) std::cerr << "ProxyConnection::register_proxy(" << interface_name << ", " <<  object_path << ", " << proxy << ") called" << std::endl;

	// for(auto itr = _proxies.begin(); itr != _proxies.end(); ++itr)
	// {
	// 	for(auto it = itr->second.begin(); it != itr->second.end(); ++it)
	// 	{
	// 		if (_debug_level) std::cerr << "_proxies[" << itr->first << "][" << it->first << "] = " << it->second << std::endl;
	// 	}
	// }
	auto interfaces = _proxies.find(interface_name);
	if (interfaces != _proxies.end()) {
		if (interfaces->second.find(object_path) != interfaces->second.end()) {
			// we already have a proxy for this
			if (_debug_level) std::cerr << interface_name << " -> " << object_path << " has already a proxy object associated with it." << std::endl;
			return ;
		}
	}
	_proxies[interface_name][object_path] = proxy; // map the object_path to the proxy object
}

bool ProxyConnection::dispatch(Glib::IOCondition condition) 
{
	if (_debug_level) std::cerr << "ProxyConnection::dispatch() called" << std::endl;

	saftbus::MessageTypeS2C type = saftbus::SIGNAL;
	while(!expect_from_server(type));

	dispatchSignal();

	return true;
}

void ProxyConnection::dispatchSignal()
{
	guint32 size;
	saftbus::read(get_fd(), size);
	if (_debug_level) std::cerr << "expecting message with size " << size << std::endl;
	std::vector<char> buffer(size);
	saftbus::read_all(get_fd(), &buffer[0], size);
	Glib::Variant<std::vector<Glib::VariantBase> > payload;
	deserialize(payload, &buffer[0], buffer.size());
	if (_debug_level) std::cerr << "got signal content " << payload.get_type_string() << " " << payload.print() << std::endl;
	Glib::Variant<Glib::ustring> object_path    = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(payload.get_child(0));
	Glib::Variant<Glib::ustring> interface_name = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(payload.get_child(1));
	Glib::Variant<Glib::ustring> signal_name    = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(payload.get_child(2));
	Glib::Variant<gint64> sec    = Glib::VariantBase::cast_dynamic<Glib::Variant<gint64> >(payload.get_child(3));
	Glib::Variant<gint64> nsec    = Glib::VariantBase::cast_dynamic<Glib::Variant<gint64> >(payload.get_child(4));
	Glib::VariantContainerBase parameters       = Glib::VariantBase::cast_dynamic<Glib::VariantContainerBase>   (payload.get_child(5));

    struct timespec stop;
    clock_gettime( CLOCK_REALTIME, &stop);
      double dt = (1.0e6*stop.tv_sec   + 1.0e-3*stop.tv_nsec) 
                - (1.0e6*sec.get() + 1.0e-3*nsec.get());

   	//std::cerr << "got signal after " << dt << " us" << std::endl;
    if (_debug_level) 
    {

		std::cerr << "object_path = " << object_path.get() << std::endl;
		std::cerr << "interface_name = " << interface_name.get() << std::endl;
		std::cerr << "signal_name = " << signal_name.get() << std::endl;
		std::cerr << "calling the on_signal function " << std::endl;
	 
		for (auto itr = _proxies.begin(); itr != _proxies.end(); ++itr)
		{
			for (auto it = itr->second.begin(); it != itr->second.end(); ++it)
			{
				std::cerr << "_proxies[" << itr->first << "][" << it->first << "] = " << it->second << std::endl;
			}
		}
	}

	auto interfaces = _proxies.find(interface_name.get());
	if (interfaces != _proxies.end()) {
		if (interfaces->second.find(object_path.get()) != interfaces->second.end()) {
			_proxies[interface_name.get()][object_path.get()]->on_signal("saftd", signal_name.get(), parameters);
		}
	}


}




}