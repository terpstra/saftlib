
#include <memory>
#include <chrono>
#include <map>
#include <fstream>

#include <SAFTd.hpp>
#include <TimingReceiver.hpp>
#include <TimingReceiverAddon.hpp>
#include "AnalyzeJitterMSI.hpp"

#include <saftbus/loop.hpp>


std::map<int,int> hist;

long shutdown_threshold = 0;
bool shutdown = false;

std::ofstream trace_marker;

int count = 0;
void on_MSI(uint32_t value) {
	if (shutdown_threshold) {
		trace_marker << "analyze-jitter-msi-standalone MSI callback function executed" << std::endl;
	}

	static auto last = std::chrono::steady_clock::now();
	       auto now  = std::chrono::steady_clock::now();

	// make sure that no MSI got lost
	static bool first = true;
	static int last_value = 0;
	if (first) {
		first = false;
	} else {
		int diff = value-last_value;
		if (diff != 1) {
			std::cerr << "unexpected value diff: " << diff << ". Did we miss an MSI?" << std::endl;
		}
	}
	last_value = value;


	if (count) {
		auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now-last).count();
		// std::cout << diff << std::endl;
		++hist[diff];


		if (shutdown_threshold && diff > shutdown_threshold) {
			shutdown = true;
			std::ofstream tracing_on("/sys/kernel/debug/tracing/tracing_on");
			tracing_on << "0\n";
			std::cerr << "time measurement (" << diff << " us) was larger than threshold (" << shutdown_threshold << " us ). => writing 0 into /sys/kernel/debug/tracing/tracing_on and exit" << std::endl;
		}

	}
	last = now;
	++count;
}

int main(int argc, char *argv[]) {
	if (argc != 5 && argc != 6) {
		std::cerr << "usage: " << argv[0] << " <eb-device> <msi-period-us> <measurements> <histogram-filename> [ <trace-shutoff-threshold[us]> ] " << std::endl;
		std::cerr << "   example:                         " << argv[0] << " dev/wbm0 40000 100 hist.dat" << std::endl;
		std::cerr << "   example with shutdown threshold: " << argv[0] << " dev/wbm0 40000 100 hist.dat 1500" << std::endl;
		return 1;
	} 

	int msi_period = 0;
	int n_measurements = 0;
	std::istringstream in(argv[2]);
	in >> msi_period;
	if (!in) {
		std::cerr << "cannot read msi_period from " << argv[2] << std::endl;
		return 1;
	}
	if (msi_period <= 500) {
		std::cerr << "msi_period must be > 500" << std::endl;
		return 1;
	}

	std::istringstream n_in(argv[3]);
	n_in >> n_measurements;
	if (!n_in) {
		std::cerr << "cannot read number of measurements from " << argv[3] << std::endl;
		return 1;
	}
	if (n_measurements < 1) {
		std::cerr << "number of measurements must be > 0" << std::endl;
		return 1;
	}

	if (argc == 6) { // read shutdown threshold
		std::istringstream s_in(argv[5]);
		s_in >> shutdown_threshold;
		if (!s_in) {
			std::cerr << "cannot read trace-shutoff-threshold from argument " << argv[5] << std::endl;
			return 1;
		}
		std::cerr << "running with trace-shutoff-threshold of " << shutdown_threshold << " us" << std::endl;
	}

	if (shutdown_threshold) { // open trace marker file
		trace_marker.open("/sys/kernel/tracing/trace_marker");
	}
	

	try {

		saftlib::SAFTd saftd; 
		saftlib::TimingReceiver tr(saftd, "tr0", argv[1], 2);

		std::unique_ptr<saftlib::AnalyzeJitterMSI> jitter_msi = std::unique_ptr<saftlib::AnalyzeJitterMSI>(new saftlib::AnalyzeJitterMSI(&saftd, &tr));

		jitter_msi->MSI.connect(sigc::ptr_fun(on_MSI));
		jitter_msi->start(msi_period);

		while(count <= n_measurements && !shutdown) {
			if (shutdown_threshold) {
				trace_marker << "analyze-jitter-msi-standalone iterate MainLoop" << std::endl;
			}

			saftbus::Loop::get_default().iteration(true);
		}

		jitter_msi->stop();

		std::ofstream hist_file(argv[4]);
		int MAX = 0;
		for(auto &p: hist) {
			if (p.first>MAX) MAX=p.first;
		}
		for (int i = 0; i < MAX; ++i) {
			hist_file << i << " " << hist[i] << std::endl;
		}


	} catch (std::runtime_error &e ) {
		std::cerr << "exception: " << e.what() << std::endl;
	}

	return 0;

}