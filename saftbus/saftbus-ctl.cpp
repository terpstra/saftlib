#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "saftbus.h"
#include "core.h"
#include "Interface.h"


void write_histogram(Glib::ustring filename, const std::map<int,int> &hist)
{
	std::cout << "writing histogram " << filename << std::endl;
	std::ofstream out(filename.c_str());
	for (auto it: hist) {
		out << it.first << " " << it.second << std::endl;
	}

}

void show_help(const char *argv0) {
	std::cerr << "usage: " << argv0 << "[options]" << std::endl;
	std::cerr << "   options are:" << std::endl;
	std::cerr << "   -l     list all active devices in saftlib (object paths)" << std::endl;
	std::cerr << "   -p     show all open signal pipes to proxy objects" << std::endl;
	std::cerr << "   -s     write timing statistics to file" << std::endl;
	std::cerr << "   -m     print mutable state of connection" << std::endl;
}


void print_mutable_state(Glib::RefPtr<saftbus::ProxyConnection> connection) 
{
	saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_GET_STATE);


	std::map<Glib::ustring, std::map<Glib::ustring, int> > saftbus_indices; 
	saftbus::read(connection->get_fd(), saftbus_indices);
	std::vector<int> indices;
	std::vector<int> assigned_indices;
	saftbus::read(connection->get_fd(), indices);
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::left << std::setw(40) << "interface name" 
	          << std::left << std::setw(50) << "object path" 
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
				std::cout << std::left << std::setw(40) << " " 
				          << std::left << std::setw(50) << object_path.first
				          << std::right << std::setw(10) << object_path.second;
			}
			if (find(indices.begin(), indices.end(), object_path.second) != indices.end()) {
				std::cout << "   yes";
				assigned_indices.push_back(object_path.second);
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}


	if (assigned_indices.size() != indices.size()) {
		std::cout << "_____________________________________________________________________________________________________________" << std::endl;
		std::cout << "found vtable indices without saftbus object assigned" << std::endl;
		std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	}

	int saftbus_object_id_counter; // log saftbus object creation
	saftbus::read(connection->get_fd(), saftbus_object_id_counter);
	int saftbus_signal_handle_counter; // log signal subscriptions
	saftbus::read(connection->get_fd(), saftbus_signal_handle_counter);

	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "object id counter:     " << saftbus_object_id_counter << std::endl;
	std::cout << "signal handle counter: " << saftbus_signal_handle_counter << std::endl;
	std::cout << "_____________________________________________________________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::vector<int> sockets_active;
	saftbus::read(connection->get_fd(), sockets_active);
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
	//int _client_id;

	// 	     // handle    // signal
	//std::map<guint, sigc::signal<void, const Glib::RefPtr<Connection>&, const Glib::ustring&, const Glib::ustring&, const Glib::ustring&, const Glib::ustring&, const Glib::VariantContainerBase&> > _handle_to_signal_map;
	std::map<guint, int> handle_to_signal_map;
	saftbus::read(connection->get_fd(), handle_to_signal_map);
	for (auto handle_signal: handle_to_signal_map) {
		std::cout << handle_signal.first << " " << handle_signal.second << std::endl;
	}


	std::map<Glib::ustring, std::set<guint> > id_handles_map;
	saftbus::read(connection->get_fd(), id_handles_map);

	//std::set<guint> erased_handles;
	std::vector<guint> erased_handles;
	saftbus::read(connection->get_fd(), erased_handles);



	// store the pipes that go directly to one or many Proxy objects
			// interface_name        // object path
	std::map<Glib::ustring, std::map < Glib::ustring , std::set< saftbus::ProxyPipe > > > proxy_pipes;
	saftbus::read(connection->get_fd(), proxy_pipes);

	int _saftbus_id_counter;
	saftbus::read(connection->get_fd(), _saftbus_id_counter);

}

int main(int argc, char *argv[])
{

	Glib::init();

	bool list_objects                 = false;
	bool list_pipes                   = false;
	bool get_timing_stats             = false;
	bool list_mutable_state           = false;
	std::string timing_stats_filename = "saftbus_timing.dat";

	for (int i = 1; i < argc; ++i) {
		std::string argvi = argv[i];
		if (argvi == "-l") {
			list_objects = true;
		} else if (argvi == "-p") {
			list_pipes = true;
		} else if (argvi == "-m") {
			list_mutable_state = true;
		} else if (argvi == "-s") {
			get_timing_stats = true;
			// if (++i < argc) {
			// 	timing_stats_filename = argv[i];
			// 	std::cerr << timing_stats_filename << std::endl;
			// }
		} else {
			std::cerr << "unknown argument: " << argvi << std::endl;
			return 1;
		}
	}

	// connect to saft-daemon
	auto connection = Glib::RefPtr<saftbus::ProxyConnection>(new saftbus::ProxyConnection);

	// say hello to saftbus
	saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_HELLO);

	// tell saftbus to send us some information
	saftbus::write(connection->get_fd(), saftbus::SAFTBUS_CTL_STATUS);


	std::map<Glib::ustring, std::map<Glib::ustring, int> > saftbus_indices;
	saftbus::read(connection->get_fd(), saftbus_indices);

	std::vector<int> indices;
	saftbus::read(connection->get_fd(), indices);

	std::map<int, int> signal_flight_times;
	saftbus::read(connection->get_fd(), signal_flight_times);

	std::map<Glib::ustring, std::map<int, int> > function_run_times;
	saftbus::read(connection->get_fd(), function_run_times);


	std::set<Glib::ustring> object_paths;

	for (auto itr: saftbus_indices) {
		for (auto it: itr.second) {
			object_paths.insert(it.first);
		}
	}


	if (list_objects) {
		std::cerr << "listing all objects: " << std::endl;
		for( auto it: object_paths) {
			std::cerr << "    " << it << std::endl;
		}
	}

	if (list_pipes) {
		std::cerr << "listing all active indices: " << std::endl;
		for (auto it: indices) {
			std::cerr << "    " << it << std::endl;
		}
	}

	if (get_timing_stats) {
		write_histogram(timing_stats_filename.c_str(), signal_flight_times);
		for (auto function: function_run_times) {
			write_histogram(function.first, function.second);
		}
	}

	if (list_mutable_state) {
		print_mutable_state(connection);
	}


	connection.reset();
	std::cerr << "saftbus-ctl done" << std::endl;
	return 0;
}