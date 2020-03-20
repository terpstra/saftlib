#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <algorithm>
#include "saftbus.h"
#include "core.h"
#include "Interface.h"

void write_histogram(std::string filename, const std::map<int,int> &hist);
void show_help(const char *argv0);
void print_mutable_state(std::shared_ptr<saftbus::ProxyConnection> connection);
void print_saftbus_object_table(std::shared_ptr<saftbus::ProxyConnection> connection) ;
void saftbus_get_property(const std::string& interface_name,
	                      const std::string& object_path,
	                      const std::string& property_name,
	                      const std::string& property_type_signature);
void saftbus_set_property(const std::string& interface_name,
	                      const std::string& object_path,
	                      const std::string& property_name,
	                      const std::string& property_type_signature,
	                      const std::string& value);
void saftbus_method_call (const std::string& interface_name,
	                      const std::string& object_path,
	                      const std::string& method_name,
	                      const std::string& type_signature,
	                      const std::vector<std::string>& arguments,
	                      const std::string return_type_signature);

void write_histogram(std::string filename, const std::map<int,int> &hist)
{
	std::cout << "writing histogram " << filename << std::endl;
	std::ofstream out(filename.c_str());
	for (auto it: hist) {
		out << it.first << " " << it.second << std::endl;
	}

}

void show_help(const char *argv0) 
{
	std::cout << "usage: " << argv0 << " [options]" << std::endl;
	std::cout << "   options are:" << std::endl;
	std::cout << "   -h,--help                          " << std::endl;
	std::cout << "   -s,--status                        " << std::endl;
	std::cout << "   --get-properties <interface-name> <object-path>" << std::endl;
	std::cout << "   --get-property <interface-name> <object-path> <property-name> <type-signature>" << std::endl;
	std::cout << "   --set-property <interface-name> <object-path> <property-name> <type-signature> <property-value>" << std::endl;
	std::cout << "   --call <interface-name> <object-path> <method-name> <return-type-signature> <argument-type-signature> <argument1> ..." << std::endl;
	std::cout << "          the type signature for no argument/return value is v" << std::endl;
	std::cout << "          the type signature for all other types matches the DBus specification" << std::endl;
	std::cout << "   --introspect <interface-name> <object-path>" << std::endl;
	std::cout << "   --enable-logging                   " << std::endl;
	std::cout << "   --disable-logging                  " << std::endl;
	std::cout << "   --enable-signal-timing-stats       " << std::endl;
	std::cout << "   --disable-signal-timing-stats      " << std::endl;
	std::cout << "   --download-signal-timing-stats     " << std::endl;
}


void print_mutable_state(std::shared_ptr<saftbus::ProxyConnection> connection) 
{
	saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_GET_STATE);


	std::map<std::string, std::map<std::string, int> > saftbus_indices; 
	saftbus::read(connection->get_fd(), saftbus_indices);
	std::vector<int> indices;
	std::vector<int> assigned_indices;
	saftbus::read(connection->get_fd(), indices);

	int saftbus_object_id_counter; // log saftbus object creation
	saftbus::read(connection->get_fd(), saftbus_object_id_counter);
	int saftbus_signal_handle_counter; // log signal subscriptions
	saftbus::read(connection->get_fd(), saftbus_signal_handle_counter);

	std::vector<int> sockets_active;
	saftbus::read(connection->get_fd(), sockets_active);

	std::map<int, std::string> socket_owner;
	saftbus::read(connection->get_fd(), socket_owner);

	// 	     // handle    // signal
	//std::map<unsigned, sigc::signal<void, const std::shared_ptr<Connection>&, const std::string&, const std::string&, const std::string&, const std::string&, const Glib::VariantContainerBase&> > _handle_to_signal_map;
	std::map<unsigned, int> handle_to_signal_map;
	saftbus::read(connection->get_fd(), handle_to_signal_map);
	// for (auto handle_signal: handle_to_signal_map) {
	// 	std::cout << handle_signal.first << " " << handle_signal.second << std::endl;
	// }


	std::map<std::string, std::set<unsigned> > id_handles_map;
	saftbus::read(connection->get_fd(), id_handles_map);


	//std::set<unsigned> erased_handles;
	std::vector<unsigned> erased_handles;
	saftbus::read(connection->get_fd(), erased_handles);



	// store the pipes that go directly to one or many Proxy objects
			// interface_name        // object path
	std::map<std::string, std::map < std::string , std::set< saftbus::SignalFD > > > proxy_pipes;
	saftbus::read(connection->get_fd(), proxy_pipes);

	int _saftbus_id_counter;
	saftbus::read(connection->get_fd(), _saftbus_id_counter);

	std::map<std::string, std::string> owners;
	saftbus::read(connection->get_fd(), owners);



	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "object id counter:     " << saftbus_object_id_counter << std::endl;
	std::cout << "signal handle counter: " << saftbus_signal_handle_counter << std::endl;
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "socket: ";
	for(unsigned i = 0; i < sockets_active.size(); ++i ) {
		std::cout << std::setw(3) << i;
	}
	std::cout << std::endl;
	std::cout << "busy:   ";
	for(unsigned i = 0; i < sockets_active.size(); ++i ) {
		if (sockets_active[i]) {
			std::cout << std::setw(3) << "*";
		} else {
			std::cout << std::setw(3) << " ";
		}
	}
	std::cout << std::endl;
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;

	std::cout << std::left << std::setw(40) << "interface name" 
	          << std::left << std::setw(50) << "object path \"owner\"" 
	          << std::right << std::setw(10) << "index" 
	          << "   vtable" << std::endl;
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;
	for (auto saftbus_index: saftbus_indices) {
		std::cout << std::left << std::setw(40) << saftbus_index.first;
		bool first = true;
		for (auto object_path: saftbus_index.second) {
			if (first) {
				std::cout << std::left << std::setw(50) << object_path.first 
				          << std::right << std::setw(10) << object_path.second;
				first = false;
			} else {
				std::string obj_path = object_path.first;
				if (saftbus_index.first == "de.gsi.saftlib.Owned") {
					std::string owner = owners[object_path.first];
					if (owner != "") {
						obj_path.append(" \"");
						obj_path.append(owner);
						obj_path.append("\"");
					}
					//std::cerr << " \"" << owners[object_path.first] << "\"" << std::endl;
				}

				std::cout << std::left << std::setw(40) << " " 
				          << std::left << std::setw(50) << obj_path
				          << std::right << std::setw(10) << object_path.second;
			}
			if (find(indices.begin(), indices.end(), object_path.second) != indices.end()) {
				std::cout << "   yes";
				assigned_indices.push_back(object_path.second);
			}
			// if (saftbus_index.first == "de.gsi.saftlib.Owned") {
			// 	std::cerr << " \"" << owners[object_path.first] << "\"" << std::endl;
			// }

			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;


	if (assigned_indices.size() != indices.size()) {
		std::cout << "_____________________________________________________________________________________________________________" << std::endl;
		std::cout << "found vtable indices without saftbus object assigned" << std::endl;
		std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::setw(7) << "socket" << std::setw(7) << " owner" << std::endl;
	for (auto owner: socket_owner) {
		std::cout << std::setw(7) << owner.first << std::setw(7) << owner.second << std::endl;
	}
	std::cout << std::endl;
	//int _client_id;

}


void print_saftbus_object_table(std::shared_ptr<saftbus::ProxyConnection> connection) 
{
	saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_GET_STATE);


	std::map<std::string, std::map<std::string, int> > saftbus_indices; 
	saftbus::read(connection->get_fd(), saftbus_indices);
	std::vector<int> indices;
	std::vector<int> assigned_indices;
	saftbus::read(connection->get_fd(), indices);

	int saftbus_object_id_counter; // log saftbus object creation
	saftbus::read(connection->get_fd(), saftbus_object_id_counter);
	int saftbus_signal_handle_counter; // log signal subscriptions
	saftbus::read(connection->get_fd(), saftbus_signal_handle_counter);

	std::vector<int> sockets_active;
	saftbus::read(connection->get_fd(), sockets_active);

	std::map<int, std::string> socket_owner;
	saftbus::read(connection->get_fd(), socket_owner);

	// 	     // handle    // signal
	//std::map<unsigned, sigc::signal<void, const std::shared_ptr<Connection>&, const std::string&, const std::string&, const std::string&, const std::string&, const Glib::VariantContainerBase&> > _handle_to_signal_map;
	std::map<unsigned, int> handle_to_signal_map;
	saftbus::read(connection->get_fd(), handle_to_signal_map);
	// for (auto handle_signal: handle_to_signal_map) {
	// 	std::cout << handle_signal.first << " " << handle_signal.second << std::endl;
	// }


	std::map<std::string, std::set<unsigned> > id_handles_map;
	saftbus::read(connection->get_fd(), id_handles_map);


	//std::set<unsigned> erased_handles;
	std::vector<unsigned> erased_handles;
	saftbus::read(connection->get_fd(), erased_handles);



	// store the pipes that go directly to one or many Proxy objects
			// interface_name        // object path
	std::map<std::string, std::map < std::string , std::set< saftbus::SignalFD > > > proxy_pipes;
	saftbus::read(connection->get_fd(), proxy_pipes);

	int _saftbus_id_counter;
	saftbus::read(connection->get_fd(), _saftbus_id_counter);

	std::map<std::string, std::string> owners;
	saftbus::read(connection->get_fd(), owners);



	// std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	// std::cout << std::endl;
	// std::cout << "socket: ";
	// for(unsigned i = 0; i < sockets_active.size(); ++i ) {
	// 	std::cout << std::setw(3) << i;
	// }
	// std::cout << std::endl;
	// std::cout << "busy:   ";
	// for(unsigned i = 0; i < sockets_active.size(); ++i ) {
	// 	if (sockets_active[i]) {
	// 		std::cout << std::setw(3) << "*";
	// 	} else {
	// 		std::cout << std::setw(3) << " ";
	// 	}
	// }
	std::cout << std::endl;
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;

	std::cout << std::left << std::setw(50) << "object path" 
	          << std::left << std::setw(50) << "interface name{,proxy signal pipe fds}[owner]" 
	          << std::endl;
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;

	std::vector<std::pair<std::string, std::pair<std::string, std::string> > > table;

	for (auto saftbus_index: saftbus_indices) {
		for (auto object_path: saftbus_index.second) {
			std::string interface_name = saftbus_index.first;
			std::string obj_path = object_path.first;
			std::string owner;
			for (auto pp: proxy_pipes[interface_name][obj_path]) {
				interface_name.append(",");
				std::ostringstream fdout;
				fdout << pp.fd;
				interface_name.append(fdout.str());
			}
			if (saftbus_index.first == "de.gsi.saftlib.Owned") {
				owner = owners[object_path.first];
				if (owner != "") {
					interface_name.append("[");
					interface_name.append(owner);
					interface_name.append("]");
				}
			}
			table.push_back(std::make_pair(obj_path, std::make_pair(interface_name, owner)));
		}
	}
	std::sort(table.begin(), table.end(), [](std::pair<std::string, std::pair<std::string, std::string> > a,
		                                     std::pair<std::string, std::pair<std::string, std::string> > b) 
	                                        { return a.first < b.first; });
	std::string previous_object_path;
	for (auto line: table) {
		if (previous_object_path != line.first) {
			if (!previous_object_path.empty()) std::cout << std::endl;
			std::cout << std::left << std::setw(50) << line.first;
		} else {
			std::cout << std::left << std::setw(50) << "";
		}
		std::cout << std::left << std::setw(50) << line.second.first;
		std::cout << std::endl;
		previous_object_path = line.first;
	}
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;


	std::cout << std::endl;
	std::cout << std::setw(7) << "socket" << std::setw(7) << " owner" << std::endl;
	for (auto owner: socket_owner) {
		std::cout << std::setw(7) << owner.first << std::setw(7) << owner.second << std::endl;
	}
	std::cout << std::endl;
	//int _client_id;

}

template<typename T>
void print_serial_value(saftbus::Serial &s) {
	T value;
	s.get(value);
	std::cout << value << std::endl;
}
template<typename T>
void print_serial_vector(saftbus::Serial &s) {
	std::vector<T> values;
	s.get(values);
	for (auto value: values) {
		std::cout << value << " ";
	}
	std::cout << std::endl;
}
template<typename K, typename V>
void print_serial_map(saftbus::Serial &s) {
	std::map<K,V> map;
	s.get(map);
	for (auto pair: map) {
		std::cout << pair.first << ":" << pair.second  << " ";
	}
	std::cout << std::endl;
}

template<typename K1, typename K2, typename V>
void print_serial_map_map(saftbus::Serial &s) {
	std::map<K1, std::map<K2,V> > map;
	s.get(map);
	for (auto inner_map: map) {
		std::cout << inner_map.first << " -> ";
		for (auto pair: inner_map.second) {
			std::cout << pair.first << ":" << pair.second  << " ";
		}
		std::cout << std::endl;
	}
}

void saftbus_get_properties(const std::string &interface_name,
	                        const std::string &object_path) 
{
	saftbus::ProxyConnection connection;
	std::string introspection_xml = connection.introspect(object_path, interface_name);
	//std::cerr << introspection_xml << std::endl;
	size_t pos_property_end = 0;
	for(;;) {
		auto pos_property_begin = introspection_xml.find("<property", pos_property_end);
		if (pos_property_begin == std::string::npos) break;
		pos_property_end = introspection_xml.find("/>", pos_property_begin);

		std::istringstream in(introspection_xml.substr(pos_property_begin, pos_property_end-pos_property_begin));
		std::string property, name, type, access;

		in >> property >> name >> type >> access;

		name   = name.substr(6, name.find_last_of('\'')-6);
		type   = type.substr(6, type.find_last_of('\'')-6);
		access = access.substr(8, access.find_last_of('\'')-8);

		if (access == "read" || access == "readwrite") {
			std::cout << type << ":" << name << "=" ; saftbus_get_property(interface_name, object_path, name, type);
		}

	}
}

void saftbus_get_property(const std::string& interface_name,
	                      const std::string& object_path,
	                      const std::string& property_name,
	                      const std::string& property_type_signature) 
{
	saftbus::ProxyConnection connection;
	saftbus::Serial params;
	params.put(interface_name);
	params.put(property_name);

	int saftbus_index = connection.get_saftbus_index(object_path, interface_name);
	saftbus::Serial val = connection.call_sync(saftbus_index, object_path, "org.freedesktop.DBus.Properties", "Get", 
	  params, "sender");

	val.get_init();
	if (property_type_signature == "y") { print_serial_value<unsigned char>(val); return;}
	if (property_type_signature == "b") { print_serial_value<bool>(val); return;}
	if (property_type_signature == "n") { print_serial_value<int16_t>(val); return;}
	if (property_type_signature == "q") { print_serial_value<uint16_t>(val); return;}
	if (property_type_signature == "i") { print_serial_value<int32_t>(val); return;}
	if (property_type_signature == "u") { print_serial_value<uint32_t>(val); return;}
	if (property_type_signature == "x") { print_serial_value<int64_t>(val); return;}
	if (property_type_signature == "t") { print_serial_value<uint64_t>(val); return;}
	if (property_type_signature == "d") { print_serial_value<double>(val); return;}
	if (property_type_signature == "h") { print_serial_value<int>(val); return;}
	if (property_type_signature == "s") { print_serial_value<std::string>(val); return;}

	if (property_type_signature == "ay") { print_serial_vector<unsigned char>(val); return;}
	if (property_type_signature == "ab") { print_serial_vector<bool>(val); return;}
	if (property_type_signature == "an") { print_serial_vector<int16_t>(val); return;}
	if (property_type_signature == "aq") { print_serial_vector<uint16_t>(val); return;}
	if (property_type_signature == "ai") { print_serial_vector<int32_t>(val); return;}
	if (property_type_signature == "au") { print_serial_vector<uint32_t>(val); return;}
	if (property_type_signature == "ax") { print_serial_vector<int64_t>(val); return;}
	if (property_type_signature == "at") { print_serial_vector<uint64_t>(val); return;}
	if (property_type_signature == "ad") { print_serial_vector<double>(val); return;}
	if (property_type_signature == "ah") { print_serial_vector<int>(val); return;}
	if (property_type_signature == "as") { print_serial_vector<std::string>(val); return;}

	if (property_type_signature == "a{ss}") { print_serial_map<std::string,std::string>(val); return;}

	if (property_type_signature == "a{sa{ss}}") { print_serial_map_map<std::string,std::string,std::string>(val); return;}

	std::cout << "unsupported type signature" << std::endl;
}

void saftbus_set_property(const std::string& interface_name,
	                      const std::string& object_path,
	                      const std::string& property_name,
	                      const std::string& property_type_signature,
	                      const std::string& value) 
{
	saftbus::ProxyConnection connection;
	saftbus::Serial params;
	params.put(interface_name);
	params.put(property_name);
	saftbus::Serial property_value;
	std::istringstream value_in(value);
	if (property_type_signature == "y") { unsigned char value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "b") { bool          value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "n") { int16_t       value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "q") { uint16_t      value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "i") { int32_t       value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "u") { uint32_t      value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "x") { int64_t       value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "t") { uint64_t      value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "d") { double        value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "h") { int           value; value_in >> value; property_value.put(value); }
	if (property_type_signature == "s") { std::string   value; value_in >> value; property_value.put(value); }
	params.put(property_value);

	int saftbus_index = connection.get_saftbus_index(object_path, interface_name);
	saftbus::Serial val = connection.call_sync(saftbus_index, object_path, "org.freedesktop.DBus.Properties", "Set", 
	  params, "sender");
} 

void saftbus_method_call (const std::string& interface_name,
	                      const std::string& object_path,
	                      const std::string& method_name,
	                      const std::string& type_signature,
	                      const std::vector<std::string>& arguments,
	                      const std::string return_type_signature) 
{
	saftbus::ProxyConnection connection;
	saftbus::Serial args;
	for (unsigned i = 0; i < type_signature.size(); ++i) {
		std::istringstream value_in(arguments[i]);
		if (type_signature[i] == 'y') { unsigned char value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'b') { bool          value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'n') { int16_t       value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'q') { uint16_t      value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'i') { int32_t       value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'u') { uint32_t      value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'x') { int64_t       value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 't') { uint64_t      value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'd') { double        value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 'h') { int           value; value_in >> value; args.put(value); }
		else if (type_signature[i] == 's') { std::string   value; value_in >> value; args.put(value); }
		else {std::cerr << "unknow type signature for method call " ; return; }
	}
	int saftbus_index = connection.get_saftbus_index(object_path, interface_name);
	saftbus::Serial val = connection.call_sync(saftbus_index, object_path, interface_name, method_name, args, "sender");

	val.get_init();
	if (return_type_signature == "a{sa{ss}}") { print_serial_map_map<std::string,std::string,std::string>(val); }
	else if (return_type_signature == "a{ss}") { print_serial_map<std::string,std::string>(val); }
	else if (return_type_signature == "as") { print_serial_vector<std::string>(val); }
	else if (return_type_signature == "y") { print_serial_value<unsigned char>(val); }
	else if (return_type_signature == "b") { print_serial_value<bool>(val); }
	else if (return_type_signature == "n") { print_serial_value<int16_t>(val); }
	else if (return_type_signature == "q") { print_serial_value<uint16_t>(val); }
	else if (return_type_signature == "i") { print_serial_value<int32_t>(val); }
	else if (return_type_signature == "u") { print_serial_value<uint32_t>(val); }
	else if (return_type_signature == "x") { print_serial_value<int64_t>(val); }
	else if (return_type_signature == "t") { print_serial_value<uint64_t>(val); }
	else if (return_type_signature == "d") { print_serial_value<double>(val); }
	else if (return_type_signature == "h") { print_serial_value<int>(val); }
	else if (return_type_signature == "s") { print_serial_value<std::string>(val); }
	else if (return_type_signature == "") { /*void ... do nothing*/ }
	else {std::cerr << "unsupported return type signature" << std::endl; return; }
} 


int main(int argc, char *argv[])
{
	try {
		//Glib::init();

		bool list_mutable_state           = false;
		bool enable_signal_stats          = false;
		bool disable_signal_stats         = false;
		bool save_signal_time_stats       = false;
		bool enable_logging               = false;
		bool disable_logging              = false;
		bool get_property                 = false;
		bool set_property                 = false;
		bool call_method                  = false;
		bool introspect                   = false;
		bool get_properties               = false;

		std::string interface_name;
		std::string object_path;
		std::string property_name;
		std::string type_signature;
		std::string return_type_signature;
		std::string property_value;
		std::string method_name;
		std::vector<std::string> method_arguments;

		std::string timing_stats_filename = "saftbus_timing.dat";

		if (argc == 1) {
			show_help(argv[0]);
			return 0;
		}

		for (int i = 1; i < argc; ++i) {
			std::string argvi = argv[i];
			if (argvi == "-h" || argvi == "--help") {
				show_help(argv[0]);
				return 0;
			} else if (argvi == "-s" || argvi == "--status") {
				list_mutable_state = true;
			} else if (argvi == "--download-signal-timing-stats") {
				save_signal_time_stats = true;
			} else if (argvi == "--enable-signal-timing-stats") {
				enable_signal_stats = true;
			} else if (argvi == "--disable-signal-timing-stats") {
				disable_signal_stats = true;
			} else if (argvi == "--enable-logging") {
				enable_logging = true;
			} else if (argvi == "--disable-logging") {
				disable_logging = true;
			} else if (argvi == "--get-property") {
				get_property = true;
				if (argc - i < 4) {
					std::cerr << "expect 4 arguments after --get-property" << std::endl;
					std::cerr << "        interface_name" << std::endl;
					std::cerr << "        object_path" << std::endl;
					std::cerr << "        property_name" << std::endl;
					std::cerr << "        property_type_signature" << std::endl;
				} else {
					interface_name          = argv[++i];
					object_path             = argv[++i];
					property_name           = argv[++i];
					type_signature          = argv[++i];
				}
			} else if (argvi == "--set-property") {
				set_property = true;
				if (argc - i < 5) {
					std::cerr << "expect 5 arguments after --set-property" << std::endl;
					std::cerr << "        interface_name" << std::endl;
					std::cerr << "        object_path" << std::endl;
					std::cerr << "        property_name" << std::endl;
					std::cerr << "        property_type_signature" << std::endl;
				} else {
					interface_name          = argv[++i];
					object_path             = argv[++i];
					property_name           = argv[++i];
					type_signature          = argv[++i];
					property_value          = argv[++i];
				}
			} else if (argvi == "--call") {
				call_method = true;
				if (argc - i <= 5) {
					std::cerr << "expect >= 5 arguments after --call" << std::endl;
					std::cerr << "        interface_name" << std::endl;
					std::cerr << "        object_path" << std::endl;
					std::cerr << "        property_name" << std::endl;
					std::cerr << "        return_type_signature" << std::endl;
					std::cerr << "        arg_type_signature" << std::endl;
					std::cerr << "        [arguments ...]" << std::endl;
				} else {
					interface_name          = argv[++i];
					object_path             = argv[++i];
					method_name             = argv[++i];
					return_type_signature   = argv[++i];
					type_signature          = argv[++i];

					if (return_type_signature == "v") {
						return_type_signature = "";
					}
					if (type_signature == "v") {
						type_signature = "";
					}
					while(i < argc-1) {
						method_arguments.push_back(argv[++i]);
					}
				}
			} else if (argvi == "--introspect" || argvi == "--get-properties") {
				if (argvi == "--introspect") introspect = true;
				if (argvi == "--get-properties") get_properties = true;
				if (argc - i <= 2) {
					std::cerr << "expect >= 2 arguments after --introspect" << std::endl;
					std::cerr << "        interface_name" << std::endl;
					std::cerr << "        object_path" << std::endl;
				} else {
					interface_name          = argv[++i];
					object_path             = argv[++i];
				}
			} else {
				std::cerr << "unknown argument: " << argvi << std::endl;
				return 1;
			}
		}
		// connect to saft-daemon
		auto connection = std::shared_ptr<saftbus::ProxyConnection>(new saftbus::ProxyConnection);

		if (list_mutable_state) {
			//print_mutable_state(connection);
			print_saftbus_object_table(connection);
		}

		if (enable_signal_stats && disable_signal_stats) {
			std::cerr << "you can either disable or enable signal stats," << std::endl;
			std::cerr << " not both at the same time" << std::endl;
			return 1;
		}

		if (enable_signal_stats) {
			std::cout << "enabling signal flight time statistics in saftd" << std::endl;
			saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_ENABLE_STATS);
		}
		if (disable_signal_stats) {
			std::cout << "disabling signal flight time statistics in saftd" << std::endl;
			saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_DISABLE_STATS);
		}

		if (save_signal_time_stats) {
			std::cout << "downloading signal timing statistics from saftd" << std::endl;
			std::map<int, int> signal_flight_times;
			saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_GET_STATS);
			saftbus::read(connection->get_fd(), signal_flight_times);
			std::ofstream statfile("signal_flight_times.dat");
			for (auto entry: signal_flight_times) {
				statfile << entry.first << " " << entry.second << std::endl;
			}
			statfile.close();
		}

		if (enable_logging) {
			std::cout << "enable saftbus logging" << std::endl;
			saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_ENABLE_LOGGING);
		}
		if (disable_logging) {
			std::cout << "disable saftbus logging" << std::endl;
			saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_DISABLE_LOGGING);
		}

		if (get_property) {
			saftbus_get_property(interface_name, object_path, property_name, type_signature);
		}
		if (get_properties) {
			saftbus_get_properties(interface_name, object_path);
		}
		if (set_property) {
			saftbus_set_property(interface_name, object_path, property_name, type_signature, property_value);
		}
		if (call_method) {
			try {
				saftbus_method_call(interface_name, object_path, method_name, type_signature, method_arguments, return_type_signature);
			} catch(saftbus::Error &e) {
				std::cerr << "exepction retured from method call: " << e.what() << std::endl;
			}
		}
		if (introspect) {
			try {
				saftbus::ProxyConnection connection;
				std::cout << "introspecting " << object_path << " " << interface_name << std::endl; 
				std::cout << connection.introspect(object_path, interface_name) << std::endl;
			} catch(saftbus::Error &e) {
				std::cerr << "exepction retured from introspection: " << e.what() << std::endl;
			}
		}

		connection.reset();
	} catch (...) {
		std::cout << "Error:" << std::endl;
		std::cout << "   Failed to connect to saftd. " << std::endl;
		std::cout << "   Either all sockets are busy, or saftd isn't running at all." << std::endl;
	}
	return 0;
}