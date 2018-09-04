# r tab stops set to 4
#
# Required libraries:
#
# boost libs			[www.boost.org, tar xvf tar -xvf boost.tar.bz2 -C /opt/]
#                       [Used 1.66 on original install.   cd into /opt/boost_1_66_0; ./bootstrap.sh; ./b2 install;]
# zlib					[sudo apt-get install zlib1g-dev]
#

# On Windows systems that don't support symbolic links, place the AppD SDK in the .../extlibs directory.
#

# For Linux you should create a symbolic link in the .../extlibs directory called appdynamics-cpp-sdk that
# points to the latest version of the AppDynamics C++ SDK install.  
#
# For 4.3.x release and earlier, you'll need to point the symbolic link to a different install structure.
# Make the link point to the following:
#
# SDK_ROOT		= extlibs/appdynamics-sdk-native/sdk_lib
# SDK_INC_DIR	= $(SDK_ROOT)
# SDK_LIB_DIR	= $(SDK_ROOT)/lib
#
# For 4.4.x onwards, use the following:
#
# SDK_ROOT		= ./extlibs/appdynamics-cpp-sdk
# SDK_INC_DIR	= $(SDK_ROOT)/include
# SDK_LIB_DIR	= $(SDK_ROOT)/lib

SDK_ROOT	= ./extlibs/appdynamics-cpp-sdk
SDK_INC_DIR	= $(SDK_ROOT)/include
SDK_LIB_DIR	= $(SDK_ROOT)/lib

# POSIX compliant abstraction layer.  You will need to install boost, and potentially build
# any libraries that require it.
BOOST_INC_DIR	= /usr/local/include
BOOST_LIB_DIR	= /usr/local/lib

# The derived executable
SDKTEST_TEST_EXE = SDKTest

SDKTEST_TEST_SRC =  ./src/main/cpp/SDKTest.cpp
SDKTEST_TEST_INC =  ./src/main/cpp/SDKTestProgramOptions.h

# GNU
CCP			= g++
CPPFLAGS    = -g -std=c++11
CFLAGS      = -g -std=c++11

# includes
BOOST_INC   = -I$(BOOST_INC_DIR)
SDK_INC     = -I$(SDK_INC_DIR)

# these names are default for custom built boost.
BOOST_LIBS	    =  -lboost_thread
BOOST_LIBS	    += -lboost_system
BOOST_LIBS	    += -lboost_program_options
BOOST_LIBS	    += -lboost_timer
BOOST_LIBS	    += -lboost_chrono
BOOST_LIBS	    += -lboost_date_time

LDFLAGS         = $(BOOST_LIBS) -lz -lcrypt -lappdynamics -lpthread -ldl -lrt
SDK_LOAD_PATH	= LD_LIBRARY_PATH=$(SDK_LIB_DIR)

SDKTEST_CONTROLLER_HOST_NAME        = --ControllerHost ERROR_CONTROLLER_HOST_NOT_SET
SDKTEST_CONTROLLER_PORT_NUMBER      = --ControllerPort 0
SDKTEST_CONTROLLER_USE_SSL          = --UseSSL
SDKTEST_CONTROLLER_ACCESS_KEY       = --AccessKey ERROR_ACCESS_KEY_NOT_SET
SDKTEST_CONTROLLER_ACCOUNT_NAME     = --Account ERROR_ACCOUONT_NAME_NOT_SET

SDKTEST_CONTROLLER_APPLICATION      = --Application SDKTest1
SDKTEST_CONTROLLER_TIER_NAME        = --TierName SDKTier1
SDKTEST_CONTROLLER_NODE_NAME        = --NodeName SDKNode1

#SDKTEST_USE_HTTP_PROXY              = --UseHTTPProxy
#SDKTEST_HTTP_PROXY_HOST             = --HTTPProxyHost ERROR_HTTP_PROXY_HOST_NOT_SET
#SDKTEST_HTTP_PROXY_PORT             = --HTTPProxyPort 0
#SDKTEST_HTTP_PROXY_USER             = --HTTPProxyUser ERROR_HTTP_PROXY_USER_NOT_SET
#SDKTEST_HTTP_PROXY_PSWD             = --HTTPProxyPswd ERROR_HTTP_PROXY_PSWD_NOT_SET
#SDKTEST_HTTP_PROXY_FILE             = --HTTPProxyFile ERROR_HTTP_PROXY_FILE_NOT_SET

SDKTEST_RUNTIME_THREADS             = --Threads 1
SDKTEST_RUNTIME_CYCLES              = --Cycles 10
#SDKTEST_RUNTIME_NO_SDK              = --NoSDK

SDKTEST_RUNTIME_INIT_WAIT           = --InitWait 60000
SDKTEST_RUNTIME_BT_PAUSE            = --PauseBeforeBTBegin 1
SDKTEST_RUNTIME_TERM_PAUSE          = --PauseBeforeTerm 65

#SDKTEST_USER_CERT_FILE              = --UseCertFile
#SDKTEST_CERT_FILE                   = --CertFile ERROR_CERT_FILE_NOT_SET
#SDKTEST_CERT_DIR                    = --CertDir ERROR_CERT_DIR_NOT_SET

SDKTEST_EXE_OPTS =  $(SDKTEST_CONTROLLER_HOST_NAME)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_PORT_NUMBER)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_ACCESS_KEY)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_ACCOUNT_NAME)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_APPLICATION)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_TIER_NAME)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_NODE_NAME)
SDKTEST_EXE_OPTS += $(SDKTEST_CONTROLLER_USE_SSL)
SDKTEST_EXE_OPTS += $(SDKTEST_USE_HTTP_PROXY)
SDKTEST_EXE_OPTS += $(SDKTEST_HTTP_PROXY_HOST)
SDKTEST_EXE_OPTS += $(SDKTEST_HTTP_PROXY_PORT)
SDKTEST_EXE_OPTS += $(SDKTEST_HTTP_PROXY_USER)
SDKTEST_EXE_OPTS += $(SDKTEST_HTTP_PROXY_PSWD)
SDKTEST_EXE_OPTS += $(SDKTEST_HTTP_PROXY_FILE)
SDKTEST_EXE_OPTS += $(SDKTEST_RUNTIME_THREADS)
SDKTEST_EXE_OPTS += $(SDKTEST_RUNTIME_CYCLES)
SDKTEST_EXE_OPTS += $(SDKTEST_RUNTIME_NO_SDK)
SDKTEST_EXE_OPTS += $(SDKTEST_RUNTIME_INIT_WAIT)
SDKTEST_EXE_OPTS += $(SDKTEST_RUNTIME_BT_PAUSE)
SDKTEST_EXE_OPTS += $(SDKTEST_RUNTIME_TERM_PAUSE)
SDKTEST_EXE_OPTS += $(SDKTEST_USER_CERT_FILE)
SDKTEST_EXE_OPTS += $(SDKTEST_CERT_FILE)
SDKTEST_EXE_OPTS += $(SDKTEST_CERT_DIR)


.PHONY: clean run params

all:	$(SDKTEST_TEST_EXE) $(SDK_TEST_SRC) $(SDK_TEST_INC) Makefile

clean:
	@echo "cleaning everything..."
	rm -rf $(SDKTEST_TEST_EXE)

params:
	@echo 
	@echo "Explicitly set options" \" " $(SDKTEST_EXE_OPTS) " \"
	@echo 

$(SDKTEST_TEST_EXE): src/main/cpp/SDKTest.cpp Makefile
	@echo
	rm -f $@
	@echo
	$(CCP) $(CPPFLAGS) $(SDK_INC) $(BOOST_INC) $< -L$(BOOST_LIB_DIR) $(BOOST_LIBS) -L${SDK_LIB_DIR} $(LDFLAGS) -o $@
	@echo
	@echo Running a short SDKTest in \"noop\" mode...
	@echo
	@make noop
	@echo

noop:	$(SDKTEST_TEST_ExE)
	@echo
	$(SDK_LOAD_PATH) ./$(SDKTEST_TEST_EXE) --NoSDK --Cycles 10 --Threads 1
	@echo

test:	$(SDKTEST_TEST_EXE)
	@echo
	$(SDK_LOAD_PATH) ./$(SDKTEST_TEST_EXE) $(SDKTEST_EXE_OPTS)
	@echo

