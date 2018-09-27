#include "Proxy.h"

#include <iostream>

#include "saftbus.h"
#include "core.h"

namespace saftbus
{

Glib::RefPtr<saftbus::ProxyConnection> Proxy::_connection;

int Proxy::_global_id_counter = 0;
std::mutex Proxy::_id_counter_mutex;

Proxy::Proxy(saftbus::BusType  	   bus_type,
             const Glib::ustring&  name,
             const Glib::ustring&  object_path,
             const Glib::ustring&  interface_name,
             const Glib::RefPtr< InterfaceInfo >& info,
             ProxyFlags            flags)
	: _name(name)
	, _object_path(object_path)
	, _interface_name(interface_name)
{
	// if there is no ProxyConnection for this process yet we need to create one
	if (!static_cast<bool>(_connection)) {
		_connection = Glib::RefPtr<saftbus::ProxyConnection>(new ProxyConnection);
	}

	// generate unique proxy id (unique for all running saftlib programs)
	{
		std::unique_lock<std::mutex> lock(_id_counter_mutex);
		++_global_id_counter;
		// thjs assumes there are no more than 100 saftbus sockets available ever
		// (connection_id is the socket number XX in the socket filename "/tmp/saftbus_XX")
		_global_id = 100*_global_id_counter + _connection->get_connection_id();
	}

	// create a pipe through which we will receive signals from the saftd
	try {
		if (pipe(_pipe_fd) != 0) {
			throw std::runtime_error("Proxy constructor: could not create pipe for signal transmission");
		}

		// send the writing end of a pipe to saftd 
		saftbus::write(_connection->get_fd(), saftbus::SIGNAL_FD);
		saftbus::sendfd(_connection->get_fd(), _pipe_fd[1]);	
		saftbus::write(_connection->get_fd(), _object_path);
		saftbus::write(_connection->get_fd(), _interface_name);
		saftbus::write(_connection->get_fd(), _global_id);
	} catch(...) {
		std::cerr << "Proxy::~Proxy() threw" << std::endl;
	}

	// hook the reading end of the pipe into the default Glib::MainLoop with 
	//     HIGH priority and connect the dispatch method as signal handler
	_signal_connection_handle = Glib::signal_io().connect(sigc::mem_fun(*this, &Proxy::dispatch), 
	                          _pipe_fd[0], Glib::IO_IN | Glib::IO_HUP, 
	                          Glib::PRIORITY_HIGH);
}

Proxy::~Proxy() 
{
	_signal_connection_handle.disconnect();
	// free all resources ...
	close(_pipe_fd[0]);
	close(_pipe_fd[1]);
	try {
		// ... and tell saftd that it can release the writing end signal pipe for this Proxy
		saftbus::write(_connection->get_fd(), saftbus::SIGNAL_REMOVE_FD);
		saftbus::write(_connection->get_fd(), _object_path);
		saftbus::write(_connection->get_fd(), _interface_name);
		saftbus::write(_connection->get_fd(), _global_id);
	} catch(std::exception &e) {
		std::cerr << "Proxy::~Proxy() threw: " << e.what() << std::endl;
	}
}

bool Proxy::dispatch(Glib::IOCondition condition)
{
	// this method is called from the Glib::MainLoop whenever there is signal data in the pipe

	try {

	// read type and size of signal
	saftbus::MessageTypeS2C type;
	guint32                 size;
	saftbus::read(_pipe_fd[0], type);
	saftbus::read(_pipe_fd[0], size);

	// prepare buffer of the right size for the incoming data
	std::vector<char> buffer(size);
	saftbus::read_all(_pipe_fd[0], &buffer[0], size);

	// de-serialize the data using the Glib::Variant infrastructure
	Glib::Variant<std::vector<Glib::VariantBase> > payload;
	deserialize(payload, &buffer[0], buffer.size());
	// read content from the variant type (this works because we know what saftd will send us)
	Glib::Variant<Glib::ustring> object_path    = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> > (payload.get_child(0));
	Glib::Variant<Glib::ustring> interface_name = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> > (payload.get_child(1));
	Glib::Variant<Glib::ustring> signal_name    = Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> > (payload.get_child(2));
	// the following two items are for signal flight time measurement (the time when the signal was sent)
	Glib::Variant<gint64> sec                   = Glib::VariantBase::cast_dynamic<Glib::Variant<gint64> >        (payload.get_child(3));
	Glib::Variant<gint64> nsec                  = Glib::VariantBase::cast_dynamic<Glib::Variant<gint64> >        (payload.get_child(4));
	Glib::VariantContainerBase parameters       = Glib::VariantBase::cast_dynamic<Glib::VariantContainerBase>    (payload.get_child(5));

	// if we don't get the expected _object path, saftd probably messed up the pipe lookup
	if (_object_path != object_path.get()) {
		std::ostringstream msg;
		msg << "Proxy::dispatch() : signal with wrong object_path: expecting " 
		    << _object_path 
		    << ",  got " 
		    << object_path.get();
		throw std::runtime_error(msg.str());
	}

	double signal_flight_time;

	// special treatment for property changes
	if (interface_name.get() == "org.freedesktop.DBus.Properties" && signal_name.get() == "PropertiesChanged")
	{
		// in case of a property change, the interface name of the property 
		// that was changed (here we call it derived_interface_name) is embedded in the data
		Glib::Variant<Glib::ustring> derived_interface_name 
				= Glib::VariantBase::cast_dynamic<Glib::Variant<Glib::ustring> >(parameters.get_child(0));
		// if we don't get the expected _interface_name, saftd probably messed up the pipe lookup		
		if (_interface_name != derived_interface_name.get()) {
			std::ostringstream msg;
			msg << "Proxy::dispatch() : signal with wrong interface name: expected " 
			    << _interface_name 
			    << ",  got " 
			    << derived_interface_name.get();
			throw std::runtime_error(msg.str());
		}

		// get the real data: which property has which value
		Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> > property_map 
				= Glib::VariantBase::cast_dynamic<Glib::Variant<std::map<Glib::ustring, Glib::VariantBase> > >
						(parameters.get_child(1));
		Glib::Variant<std::vector< Glib::ustring > > invalidated_properies 
				= Glib::VariantBase::cast_dynamic<Glib::Variant<std::vector< Glib::ustring > > >
						(parameters.get_child(2));

		// get the signal flight stop time right before we call the signal handler from the Proxy object
	    struct timespec stop;
	    clock_gettime( CLOCK_REALTIME, &stop);
	    signal_flight_time = (1.0e6*stop.tv_sec   + 1.0e-3*stop.tv_nsec) 
	                       - (1.0e6*sec.get()     + 1.0e-3*nsec.get());
		// report the measured signal flight time to saftd
	    try {
			saftbus::write(_connection->get_fd(), saftbus::SIGNAL_FLIGHT_TIME);
			saftbus::write(_connection->get_fd(), signal_flight_time);
	    // deliver the signal: call the property changed handler of the derived class
			on_properties_changed(property_map.get(), invalidated_properies.get());
		} catch(...) {
			std::cerr << "Proxy::dispatch() : on_properties_changed threw " << std::endl;
		}
	}
	else // all other signals
	{
		// if we don't get the expected _interface_name, saftd probably messed up the pipe lookup		
		if (_interface_name != interface_name.get()) {
			std::ostringstream msg;
			msg << "Proxy::dispatch() : signal with wrong interface name: expected " 
			    << _interface_name 
			    << ",  got " 
			    << interface_name.get();
			throw std::runtime_error(msg.str());
		}
		// get the signal flight stop time right before we call the signal handler from the Proxy object
	    struct timespec stop;
	    clock_gettime( CLOCK_REALTIME, &stop);
	    signal_flight_time = (1.0e6*stop.tv_sec   + 1.0e-3*stop.tv_nsec) 
	                       - (1.0e6*sec.get()     + 1.0e-3*nsec.get());
		// report the measured signal flight time to saftd
	    try {
			saftbus::write(_connection->get_fd(), saftbus::SIGNAL_FLIGHT_TIME);
			saftbus::write(_connection->get_fd(), signal_flight_time);
	    // deliver the signal: call the signal handler of the derived class 
			on_signal("de.gsi.saftlib", signal_name.get(), parameters);
		} catch(...) {
			std::cerr << "Proxy::dispatch() : on_signal threw " << std::endl;
		}
	}

	} catch (std::exception &e) {
		std::cerr << "Proxy::dispatch() : exception : " << e.what() << std::endl;
	}


	return true;
}

void Proxy::get_cached_property (Glib::VariantBase& property, const Glib::ustring& property_name) const 
{
	// this is not implemented yet and it is questionable if this is beneficial in case of saftlib
	return; // empty response
}

void Proxy::on_properties_changed (const MapChangedProperties& changed_properties, const std::vector< Glib::ustring >& invalidated_properties)
{
	// this will be overloaded by the derived Proxy class
}
void Proxy::on_signal (const Glib::ustring& sender_name, const Glib::ustring& signal_name, const Glib::VariantContainerBase& parameters)
{
	// this will be overloaded by the derived Proxy class
}
Glib::RefPtr<saftbus::ProxyConnection> Proxy::get_connection() const
{
	return _connection;
}

Glib::ustring Proxy::get_object_path() const
{
	return _object_path;
}
Glib::ustring Proxy::get_name() const
{
	return _name;
}

const Glib::VariantContainerBase& Proxy::call_sync(std::string function_name, const Glib::VariantContainerBase &query)
{
	// call the Connection::call_sync in a special way that cast the result in a special way. 
	//   Without this cast the generated Proxy code cannot handle the resulting variant type.
	_result = Glib::VariantBase::cast_dynamic<Glib::VariantContainerBase>(
			  	Glib::VariantBase::cast_dynamic<Glib::Variant<std::vector<Glib::VariantBase> > >(
						_connection->call_sync(_object_path, 
		                			          _interface_name,
		                			          function_name,
		                			          query)).get_child(0));
	return _result;
}

}
