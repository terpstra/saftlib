AUTOMAKE_OPTIONS=foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -Wall -g  $(SIGCPP_CFLAGS) $(EB_CFLAGS) -I $(top_srcdir)/saftbus -I $(top_srcdir)/src -I $(top_builddir)/src -I $(top_srcdir)/src/interfaces -I $(top_builddir)/src/interfaces -DDATADIR='"$(datadir)/saftlib"'

pkgdir = @libdir@/pkgconfig
pkg_DATA = saftlib3.pc
EXTRA_DIST = saftlib3.pc.in
#the file leap-seconds.list ist needed by the Time class to convert between UTC and WR-time
leapseconddir = $(datadir)/saftlib3
leapsecond_DATA = leap-seconds.list
EXTRA_DIST += leap-seconds.list 
EXTRA_DIST += src/software-tr.sdb
EXTRA_DIST += saftbus/configurable_chunck_allocator_rt.cpp
EXTRA_DIST += saftbus/configurable_chunck_allocator_rt.hpp 
EXTRA_DIST += saftbus-gen/saftbus-gen.cpp

#dist_bin_SCRIPTS = src/saftd


.PHONY: version.h # causes build.cpp to be rebuilt every run
BUILT_SOURCES = version.h          \
	src/Owned_Service.cpp           \
	src/SAFTd_Service.cpp            \
	src/Condition_Service.cpp         \
	src/ActionSink_Service.cpp         \
	src/SoftwareCondition_Service.cpp   \
	src/SoftwareActionSink_Service.cpp   \
	src/SCUbusCondition_Service.cpp       \
	src/SCUbusActionSink_Service.cpp       \
	src/EmbeddedCPUCondition_Service.cpp    \
	src/EmbeddedCPUActionSink_Service.cpp    \
	src/WbmCondition_Service.cpp              \
	src/WbmActionSink_Service.cpp              \
	src/Output_Service.cpp                      \
	src/OutputCondition_Service.cpp              \
	src/Input_Service.cpp                         \
	src/ECA_Event.cpp                              \
	src/TimingReceiver_Service.cpp                  \
	src/CommonFunctions_Service.hpp                  \
	src/FunctionGenerator_Service.hpp                 \
 	src/MasterFunctionGenerator_Service.hpp           \
 	src/FunctionGeneratorFirmware_Service.hpp         \
 	src/FunctionGeneratorImpl_Service.hpp             \
	src/BurstGenerator_Service.cpp                    \
	src/BurstGenerator_Service.hpp             

lib_LTLIBRARIES = \
	libsaftbus.la         \
	libsaft-service.la  \
	libsaft-proxy.la     \
	libfg-firmware-service.la  \
	libfg-firmware-proxy.la	    \
	libbg-firmware-service.la    \
	libbg-firmware-proxy.la   




saftbus_includedir = $(includedir)/saftlib3/saftbus

libsaftbus_la_LDFLAGS = -version-info @SAFTD_API@:@SAFTD_REVISION@:@SAFTD_MINOR@ 
libsaftbus_la_SOURCES =  \
	saftbus/loop.cpp      \
	saftbus/saftbus.cpp    \
	saftbus/error.cpp       \
	saftbus/client.cpp       \
	saftbus/service.cpp       \
	saftbus/plugins.cpp        \
	saftbus/server.cpp             

saftbus_include_HEADERS =     \
	saftbus/loop.hpp           \
	saftbus/error.hpp           \
	saftbus/saftbus.hpp          \
	saftbus/client.hpp            \
	saftbus/service.hpp            \
	saftbus/plugins.hpp             \
	saftbus/global_allocator.hpp     \
	saftbus/chunck_allocator_rt.hpp   \
	saftbus/server.hpp                 



# saftlib is substituded with the string given to AC_INIT([...])
libsaft_service_la_LDFLAGS = -Wl,--export-dynamic -version-info @SAFTD_API@:@SAFTD_REVISION@:@SAFTD_MINOR@ 
libsaft_service_la_LIBADD  = $(EB_LIBS)   $(SIGCPP_LIBS) -ldl 
libsaft_service_la_SOURCES =\
	src/build.cpp                 \
	src/Time.cpp                   \
	src/eb-source.cpp               \
	src/eb-forward.cpp               \
	src/Owned.cpp                     \
	src/Owned_Service.cpp              \
	src/SAFTd.cpp                       \
	src/SAFTd_Service.cpp                \
	src/Condition.cpp                     \
	src/Condition_Service.cpp              \
	src/ActionSink.cpp                      \
	src/ActionSink_Service.cpp               \
	src/SoftwareCondition.cpp                 \
	src/SoftwareCondition_Service.cpp          \
	src/SoftwareActionSink.cpp                  \
	src/SoftwareActionSink_Service.cpp           \
	src/SCUbusCondition.cpp                       \
	src/SCUbusCondition_Service.cpp                \
	src/SCUbusActionSink.cpp                        \
	src/SCUbusActionSink_Service.cpp                 \
	src/EmbeddedCPUCondition.cpp                      \
	src/EmbeddedCPUCondition_Service.cpp               \
	src/EmbeddedCPUActionSink.cpp                       \
	src/EmbeddedCPUActionSink_Service.cpp                \
	src/WbmCondition.cpp                                  \
	src/WbmCondition_Service.cpp                           \
	src/WbmActionSink.cpp                                   \
	src/WbmActionSink_Service.cpp                            \
	src/SdbDevice.cpp                                        \
	src/MsiDevice.cpp                                        \
	src/OpenDevice.cpp                                       \
	src/WhiteRabbit.cpp                                      \
	src/Mailbox.cpp                                          \
	src/Watchdog.cpp                                         \
	src/SerdesClockGen.cpp                                   \
	src/Io.cpp                                               \
	src/IoControl.cpp                                        \
	src/Output.cpp                                           \
	src/Output_Service.cpp                                   \
	src/OutputCondition.cpp                                  \
	src/OutputCondition_Service.cpp                          \
	src/EventSource.cpp                                      \
	src/Input.cpp                                            \
	src/Input_Service.cpp                                    \
	src/ECA.cpp                                              \
	src/ECA_TLU.cpp                                          \
	src/ECA_Event.cpp                                        \
	src/BuildIdRom.cpp                                       \
	src/TempSensor.cpp                                       \
	src/Reset.cpp                                            \
	src/LM32Cluster.cpp                                      \
	src/TimingReceiver.cpp                                   \
	src/TimingReceiver_Service.cpp                           \
	src/TimingReceiverAddon.cpp                              \
	src/CommonFunctions.cpp                                  \
	src/create_service.cpp

libsaft_service_includedir = $(includedir)/saftlib3
libsaft_service_include_HEADERS =   \
	version.h                        \
	src/ats_regs.h                    \
	src/bg_regs.h                      \
	src/burstgen_shared_mmap.h           \
	src/eca_ac_wbm_regs.h                 \
	src/eca_queue_regs.h                   \
	src/eca_regs.h                          \
	src/eca_flags.h                          \
	src/eca_tlu_regs.h                        \
	src/fg_regs.h                              \
	src/io_control_regs.h                       \
	src/wr_mil_gw_regs.h                         \
	src/build.hpp                                 \
	src/Time.hpp                                   \
	src/eb-source.hpp                               \
	src/eb-forward.hpp                               \
	src/Owned.hpp                                     \
	src/SAFTd.hpp                                      \
	src/Condition.hpp                                   \
	src/ActionSink.hpp                                   \
	src/SoftwareCondition.hpp                             \
	src/SoftwareActionSink.hpp                            \
	src/SCUbusCondition.hpp                               \
	src/SCUbusActionSink.hpp                              \
	src/EmbeddedCPUCondition.hpp                          \
	src/EmbeddedCPUActionSink.hpp                         \
	src/WbmCondition.hpp                                  \
	src/WbmActionSink.hpp                                 \
	src/SdbDevice.hpp                                     \
	src/MsiDevice.hpp                                     \
	src/OpenDevice.hpp                                    \
	src/WhiteRabbit.hpp                                   \
	src/Mailbox.hpp                                       \
	src/Watchdog.hpp                                      \
	src/SerdesClockGen.hpp                                \
	src/Io.hpp                                            \
	src/IoControl.hpp                                     \
	src/Output.hpp                                        \
	src/OutputCondition.hpp                               \
	src/EventSource.hpp                                   \
	src/Input.hpp                                         \
	src/ECA.hpp                                           \
	src/ECA_TLU.hpp                                       \
	src/ECA_Event.hpp                                     \
	src/BuildIdRom.hpp                                    \
	src/TempSensor.hpp                                    \
	src/Reset.hpp                                         \
	src/LM32Cluster.hpp                                   \
	src/TimingReceiver.hpp                                \
	src/TimingReceiverAddon.hpp                           \
	src/CommonFunctions.hpp                               \
 	src/Owned_Service.hpp                                 \
 	src/SAFTd_Service.hpp                                 \
 	src/Condition_Service.hpp                             \
 	src/ActionSink_Service.hpp                            \
 	src/SoftwareCondition_Service.hpp                     \
 	src/SoftwareActionSink_Service.hpp                    \
 	src/SCUbusCondition_Service.hpp                       \
 	src/SCUbusActionSink_Service.hpp                      \
 	src/EmbeddedCPUCondition_Service.hpp                  \
 	src/EmbeddedCPUActionSink_Service.hpp                 \
 	src/WbmCondition_Service.hpp                          \
 	src/WbmActionSink_Service.hpp                         \
 	src/Output_Service.hpp                                \
 	src/OutputCondition_Service.hpp                       \
 	src/Input_Service.hpp                                 \
 	src/TimingReceiver_Service.hpp                           

libsaft_proxy_la_LDFLAGS = -version-info @SAFTD_API@:@SAFTD_REVISION@:@SAFTD_MINOR@ 
libsaft_proxy_la_LIBADD  =  $(SIGCPP_LIBS) 
libsaft_proxy_la_SOURCES =   \
	src/Time.cpp                   \
	src/Owned_Proxy.cpp             \
	src/SAFTd_Proxy.cpp              \
	src/Condition_Proxy.cpp           \
	src/ActionSink_Proxy.cpp           \
	src/SoftwareCondition_Proxy.cpp     \
	src/SoftwareActionSink_Proxy.cpp     \
	src/SCUbusCondition_Proxy.cpp         \
	src/SCUbusActionSink_Proxy.cpp         \
	src/EmbeddedCPUCondition_Proxy.cpp      \
	src/EmbeddedCPUActionSink_Proxy.cpp      \
	src/WbmCondition_Proxy.cpp                \
	src/WbmActionSink_Proxy.cpp                \
	src/OpenDevice_Proxy.cpp                    \
	src/WhiteRabbit_Proxy.cpp                    \
	src/Output_Proxy.cpp                          \
	src/OutputCondition_Proxy.cpp                  \
	src/Input_Proxy.cpp                             \
	src/EventSource_Proxy.cpp                        \
	src/ECA_Proxy.cpp                                 \
	src/ECA_TLU_Proxy.cpp                              \
	src/ECA_Event_Proxy.cpp                             \
	src/BuildIdRom_Proxy.cpp                             \
	src/TempSensor_Proxy.cpp                              \
	src/Reset_Proxy.cpp                                    \
	src/LM32Cluster_Proxy.cpp                               \
	src/TimingReceiver_Proxy.cpp                             \
	src/CommonFunctions.cpp                               


libsaft_proxy_includedir = $(includedir)/saftlib3
libsaft_proxy_include_HEADERS = \
	src/Time.hpp                      \
	src/Owned_Proxy.hpp                \
	src/SAFTd_Proxy.hpp                 \
	src/Condition_Proxy.hpp              \
	src/ActionSink_Proxy.hpp              \
	src/SoftwareCondition_Proxy.hpp        \
	src/SoftwareActionSink_Proxy.hpp        \
	src/SCUbusCondition_Proxy.hpp            \
	src/SCUbusActionSink_Proxy.hpp            \
	src/EmbeddedCPUCondition_Proxy.hpp         \
	src/EmbeddedCPUActionSink_Proxy.hpp         \
	src/WbmCondition_Proxy.hpp                   \
	src/WbmActionSink_Proxy.hpp                   \
	src/OpenDevice_Proxy.hpp                       \
	src/WhiteRabbit_Proxy.hpp                       \
	src/Output_Proxy.hpp                             \
	src/OutputCondition_Proxy.hpp                     \
	src/Input_Proxy.hpp                                \
	src/EventSource_Proxy.hpp                           \
	src/ECA_Proxy.hpp                                    \
	src/ECA_TLU_Proxy.hpp                                 \
	src/ECA_Event_Proxy.hpp                                \
	src/BuildIdRom_Proxy.hpp                                \
	src/TempSensor_Proxy.hpp                                 \
	src/Reset_Proxy.hpp                                       \
	src/LM32Cluster_Proxy.hpp                                  \
	src/TimingReceiver_Proxy.hpp                                \
	src/CommonFunctions.hpp                                     \
	src/interfaces/saftbus.h                                    \
	src/interfaces/saftlib.h                                    \
	src/interfaces/SAFTd.h                                      \
	src/interfaces/iDevice.h                                    \
	src/interfaces/iOwned.h                                     \
	src/interfaces/CommonFunctions.h                            \
	src/interfaces/SCUbusActionSink.h                           \
	src/interfaces/iEmbeddedCPUActionSink.h                     \
	src/interfaces/iSAFTd.h                                     \
	src/interfaces/EmbeddedCPUActionSink.h                      \
	src/interfaces/SCUbusCondition.h                            \
	src/interfaces/iEmbeddedCPUCondition.h                      \
	src/interfaces/iSCUbusActionSink.h                          \
	src/interfaces/EmbeddedCPUCondition.h                       \
	src/interfaces/SoftwareActionSink.h                         \
	src/interfaces/iEventSource.h                               \
	src/interfaces/iSCUbusCondition.h                           \
	src/interfaces/SoftwareCondition.h                          \
	src/interfaces/iSoftwareActionSink.h                        \
	src/interfaces/TimingReceiver.h                             \
	src/interfaces/iSoftwareCondition.h                         \
	src/interfaces/Input.h                                      \
	src/interfaces/WbmActionSink.h                              \
	src/interfaces/iInputEventSource.h                          \
	src/interfaces/iTimingReceiver.h                            \
	src/interfaces/MILbusActionSink.h                           \
	src/interfaces/WbmCondition.h                               \
	src/interfaces/iMILbusActionSink.h                          \
	src/interfaces/iWbmActionSink.h                             \
	src/interfaces/MILbusCondition.h                            \
	src/interfaces/iMILbusCondition.h                           \
	src/interfaces/iWbmCondition.h                              \
	src/interfaces/iActionSink.h                                \
	src/interfaces/Output.h                                     \
	src/interfaces/iOutputActionSink.h                          \
	src/interfaces/OutputCondition.h                            \
	src/interfaces/iCondition.h                                 \
	src/interfaces/iOutputCondition.h


if GIT_TREE
version.h:
	echo '#define GIT_ID "'$$(git describe --dirty --always --tags)'"' > version.h.tmp
	echo '#define SOURCE_DATE "'$$(date +'%b %d %Y %H:%M:%S' --date=@$$(git log -n1 --pretty='format:%ct'))'"' >> version.h.tmp
	mv -f version.h.tmp version.h
debian:
	cp debian/libsaftlib.install.in debian/libsaftlib@SAFTLIB_SONAME@.install
	./config.status --file=debian/control
endif


libfg_firmware_service_la_LDFLAGS = -Wl,--export-dynamic 
libfg_firmware_service_la_CPPADD = $(SIGCPP_CFLAGS)
libfg_firmware_service_la_LIBADD =  $(SIGCPP_LIBS) libsaftbus.la libsaft-service.la  -ldl
libfg_firmware_service_la_SOURCES =        \
	src/FunctionGenerator.cpp             \
	src/FunctionGenerator_Service.cpp      \
	src/MasterFunctionGenerator.cpp         \
	src/MasterFunctionGenerator_Service.cpp  \
	src/FunctionGeneratorImpl.cpp             \
	src/FunctionGeneratorFirmware.cpp          \
	src/FunctionGeneratorFirmware_Service.cpp   \
	src/fg_firmware_create_service.cpp

libfg_firmware_service_includedir = $(includedir)/saftlib3/fg-firmware
libfg_firmware_service_include_HEADERS =   \
	src/FunctionGenerator.hpp             \
	src/FunctionGenerator_Service.hpp      \
	src/MasterFunctionGenerator.hpp         \
	src/MasterFunctionGenerator_Service.hpp  \
	src/FunctionGeneratorImpl.hpp             \
	src/FunctionGeneratorFirmware.hpp          \
	src/FunctionGeneratorFirmware_Service.hpp

libfg_firmware_proxy_la_LIBADD  =  $(SIGCPP_LIBS) libsaftbus.la
libfg_firmware_proxy_la_SOURCES =             \
	src/FunctionGenerator_Proxy.cpp          \
	src/MasterFunctionGenerator_Proxy.cpp     \
	src/FunctionGeneratorFirmware_Proxy.cpp   

libfg_firmware_proxy_includedir = $(includedir)/saftlib3/fg-firmware
libfg_firmware_proxy_include_HEADERS =        \
	src/FunctionGenerator_Proxy.hpp          \
	src/MasterFunctionGenerator_Proxy.hpp     \
	src/FunctionGeneratorFirmware_Proxy.hpp   



bgfirmwaredir = $(datadir)/saftlib/firmware
bgfirmware_DATA = firmware-bin/burstgen.bin
EXTRA_DIST += firmware-bin/burstgen.bin

libbg_firmware_service_la_CPPADD = $(SIGCPP_CFLAGS)
libbg_firmware_service_la_LIBADD =  $(SIGCPP_LIBS) libsaftbus.la libsaft-service.la -ldl
libbg_firmware_service_la_SOURCES = \
	src/BurstGenerator.cpp                   \
	src/BurstGenerator_Service.cpp            \
	src/bg_firmware_create_service.cpp

libbg_firmware_service_includedir = $(includedir)/saftlib3/bg-firmware
libbg_firmware_service_include_HEADERS = \
	src/BurstGenerator.hpp                        \
	src/BurstGenerator_Service.hpp                 

libbg_firmware_proxy_la_LIBADD  =  $(SIGCPP_LIBS) libsaftbus.la
libbg_firmware_proxy_la_SOURCES =   \
	src/BurstGenerator_Proxy.cpp                   

libbg_firmware_proxy_includedir = $(includedir)/saftlib3/bg-firmware
libbg_firmware_proxy_include_HEADERS = \
	src/BurstGenerator_Proxy.hpp                








bin_PROGRAMS = 	\
	soft-tr3 wait-msi3 \
	saftbusd saftbusd-sda saftbusd-noda	saftbus-ctl3 \
	saft-testbench3 saft-software-tr3 \
	saft-ctl3 saft-io-ctl3 saft-pps-gen3 saft-scu-ctl3 saft-ecpu-ctl3 saft-wbm-ctl3 saft-clk-gen3 saft-dm3 saft-eb-fwd3 saft-gmt-check3  saft-uni3 saft-lcd3 saft-standalone-mbox3 saft-roundtrip-latency3 saft-standalone-roundtrip-latency3 \
	saft-burst-ctl3 saft-fg-ctl3 saft-mfg-ctl3


# saftbus programs 
saftbusd_LDFLAGS = -Wl,--export-dynamic 
saftbusd_LDADD   = $(SIGC_LIBS) libsaftbus.la -ldl 
saftbusd_SOURCES = \
	saftbus/saftbusd.cpp saftbus/configurable_chunck_allocator_rt.cpp

saftbusd_sda_LDFLAGS = -Wl,--export-dynamic 
saftbusd_sda_LDADD   = $(SIGC_LIBS) libsaftbus.la -ldl 
saftbusd_sda_SOURCES = \
	saftbus/saftbusd-sda.cpp saftbus/print_allocator_fillstate.cpp

saftbusd_noda_LDFLAGS = -Wl,--export-dynamic 
saftbusd_noda_LDADD   = $(SIGC_LIBS) libsaftbus.la -ldl 
saftbusd_noda_SOURCES = \
	saftbus/saftbusd-noda.cpp saftbus/print_allocator_fillstate.cpp

saftbus_ctl3_LDADD   = $(SIGC_LIBS) libsaftbus.la  -ldl 
saftbus_ctl3_SOURCES = saftbus/saftbus-ctl.cpp



# software timing receiver program
saft_software_tr3_sdb_DATA = src/software-tr.sdb
saft_software_tr3_sdbdir   = $(datadir)/saftlib
saft_software_tr3_LDADD    = $(EB_LIBS) -lpthread  -ldl 
saft_software_tr3_SOURCES  = src/saft-software-tr.cpp 

# another software timing receiver program
soft_tr3_LDADD    = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-service.la  -ldl 
soft_tr3_SOURCES  = src/soft-tr.cpp 

# simple program to find out how to receive MSIs from soft-tr
wait_msi3_LDADD    = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-service.la  -ldl 
wait_msi3_SOURCES  = src/wait-msi.cpp 

saft_testbench3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -lpthread -ldl 
saft_testbench3_SOURCES = src/saft-testbench.cpp

saft_ctl3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -lpthread -ldl 
saft_ctl3_SOURCES = src/saft-ctl.cpp

saft_io_ctl3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_io_ctl3_SOURCES = src/saft-io-ctl.cpp

saft_pps_gen3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_pps_gen3_SOURCES = src/saft-pps-gen.cpp

saft_scu_ctl3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_scu_ctl3_SOURCES = src/saft-scu-ctl.cpp

saft_ecpu_ctl3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_ecpu_ctl3_SOURCES = src/saft-ecpu-ctl.cpp

saft_wbm_ctl3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_wbm_ctl3_SOURCES = src/saft-wbm-ctl.cpp

saft_clk_gen3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_clk_gen3_SOURCES = src/saft-clk-gen.cpp

saft_dm3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_dm3_SOURCES = src/saft-dm.cpp

saft_eb_fwd3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_eb_fwd3_SOURCES = src/saft-eb-fwd.cpp

saft_gmt_check3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_gmt_check3_SOURCES = src/saft-gmt-check.cpp

saft_uni3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_uni3_SOURCES = src/saft-uni.cpp

saft_lcd3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_lcd3_SOURCES = src/saft-lcd.cpp

saft_standalone_mbox3_LDADD   = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-service.la  -ldl 
saft_standalone_mbox3_SOURCES = src/saft-standalone-mbox.cpp

saft_roundtrip_latency3_LDADD = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la -ldl 
saft_roundtrip_latency3_SOURCES = src/saft-roundtrip-latency.cpp

saft_standalone_roundtrip_latency3_LDADD = $(EB_LIBS)  $(SIGCPP_LIBS) libsaftbus.la libsaft-service.la  -ldl 
saft_standalone_roundtrip_latency3_SOURCES = src/saft-standalone-roundtrip-latency.cpp


saft_burst_ctl3_LDADD   =  $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la libbg-firmware-proxy.la -ldl 
saft_burst_ctl3_SOURCES = src/saft-burst-ctl.cpp 


saft_fg_ctl3_LDADD   =   $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la libfg-firmware-proxy.la -ldl 
saft_fg_ctl3_SOURCES = src/saft-fg-ctl.cpp 

saft_mfg_ctl3_LDADD   =   $(SIGCPP_LIBS) libsaftbus.la libsaft-proxy.la libfg-firmware-proxy.la -lpthread -ldl 
saft_mfg_ctl3_SOURCES = src/saft-mfg-ctl.cpp 



if NO_SAFTBUSGEN
# if saftbus-gen was not found, build a local verison of it
saftbus-gen-local: saftbus-gen/saftbus-gen.cpp
	g++ --std=c++11 -o $@ $< -DVERSION=\"local\" -O2 -g

# autogenerated files
%_Service.cpp %_Service.hpp %_Proxy.cpp %_Proxy.hpp: %.hpp saftbus-gen-local
	flock  lock_dir ./saftbus-gen-local $< -o src
	rm -rf lock_dir

# cleaning of autogenerated files
CLEANFILES = saftbus-gen-local $(top_builddir)/src/*_Proxy.*pp $(top_builddir)/src/*_Service.*pp 
else		
# autogenerated files
%_Service.cpp %_Service.hpp %_Proxy.cpp %_Proxy.hpp: %.hpp
	flock  lock_dir saftbus-gen $< -o src
	rm -rf lock_dir

# cleaning of autogenerated files
CLEANFILES = $(top_builddir)/src/*_Proxy.*pp $(top_builddir)/src/*_Service.*pp 
endif

