AUTOMAKE_OPTIONS = foreign


allsources = \
	srsTriggerHandler.cc \
	srs_control.cc \
	daq_device_srs.cc \
	srs_plugin.cc 


AM_LDFLAGS = -L$(libdir) -L$(ONLINE_MAIN)/lib

AM_CXXFLAGS = -I$(includedir) -I$(ONLINE_MAIN)/include $(RPC_INCLUDE)

lib_LTLIBRARIES =  libsrs_utils.la librcdaqplugin_srs.la 

librcdaqplugin_srs_la_SOURCES = \
	daq_device_srs.cc \
	srsTriggerHandler.cc \
	srs_plugin.cc

librcdaqplugin_srs_la_LIBADD = libsrs_utils.la -lrcdaqutils $(RPC_LDD)

libsrs_utils_la_SOURCES = srs_utils.cc 

bin_PROGRAMS =  srs_control

srs_control_SOURCES = srs_control.cc 
srs_control_LDADD = libsrs_utils.la -lrcdaqutils $(RPC_LDD)


clean-local:
	rm -f $(BUILT_SOURCES)
