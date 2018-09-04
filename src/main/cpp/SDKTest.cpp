// ************************************************************************************************************
// ******                                                                                                ******
// ****** PROGRAM: SDKTest.cpp                                                                           ******
// ****** AUTHOR:  Robb Kane                                                                             ******
// ****** DATE:    April 6th, 2018                                                                       ******
// ******                                                                                                ******
// ******                                                                                                ******
// ****** PROPERTY OF APPDYNAMICS, INC.  ALL RIGHTS RESERVED.                                            ******
// ******                                                                                                ******
// ****** This program is example code only.  There is no warranties implied or otherwise regarding its  ******
// ****** performance.  AppDynamics is not responsible for its use, nor any consequence of its use.      ******
// ******                                                                                                ******
// ******                                                                                                ******
// ****** Applicable software licenses:                                                                  ******
// ****** 1) Boost Library: http://www.boost.org/LICENSE_1_0.txt                                         ******
// ******                                                                                                ******
// ****** USE NOTES:                                                                                     ******
// ******                                                                                                ******
// ****** 1) It should NOT be necessary to add additional option parsing logic, you should be able to    ******
// ******    just add features to the Boost program_options call. Look carefully at that first.          ******
// ******    If you wish to modify this program's parameters, make sure you do so using this mechanism!  ******  
// ******                                                                                                ******
// ****** 2) Do not use tabs.  Use tabwidth of 4 spaces when editing, please maintain the coding style   ******
// ******    found within.                                                                               ******
// ******                                                                                                ******
// ************************************************************************************************************

#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/pthread/thread_data.hpp>

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
#include <iterator>
#include <typeinfo>
#include <fstream>

#include "appdynamics.h"
#include "appdynamics_advanced.h"

using namespace std;

typedef unsigned long  ULONG;

#include "SDKTestProgramOptions.h"

#define MAX_BT_NAME_LEN                100
#define MAX_THREADS                    8
#define MAX_CYCLES                     1000000000
#define FAIL_RC                        (0==1)

volatile ULONG aborted_threads = 0;
volatile bool  interrupt_recvd = false;

static void local_signal_handler(int signal)
{
    interrupt_recvd = true;
    throw runtime_error("Signal encountered.");
}

static void catch_signals(void)
{
    struct sigaction action;

    action.sa_handler    = local_signal_handler;
    action.sa_flags      = 0;

    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

class SDKTest
{
public:

    static atomic<ULONG>  bt_begin_calls_made;
    static atomic<ULONG>  bt_end_calls_made;
    static atomic<ULONG>  noop_bt_begin_calls_made;
    static atomic<ULONG>  noop_bt_end_calls_made;
    static appd_config*   cfg;
    static ULONG          controller_port;
    static ULONG          threads;
    static ULONG          term_pause;
    static ULONG          rate_pause;
    static ULONG          limit;
    static ULONG          wait;
    static ULONG          proxy_port;
    static ULONG          log_level;
    static bool           noop_SDK;
    static bool           use_SSL;
    static bool           use_cert_file;
    static bool           use_proxy;
    static const char*    cert_file;
    static const char*    cert_dir;
    static const char*    controller_host;
    static const char*    app_name;
    static const char*    tier_name;
    static const char*    node_name;
    static const char*    acct_name;
    static const char*    access_key;
    static const char*    proxy_host;
    static const char*    proxy_user;
    static const char*    proxy_pswd;
    static const char*    proxy_pswd_file;

private:
    
    bool                  abort_;
    ULONG                 thread_id_;
    ULONG                 cycle_;
    char                  bt_name_[MAX_BT_NAME_LEN];
    appd_bt_handle        bt_handle_;

public:

    inline void abortThread()
    {
        if (abort_ == false)
        {
            abort_ = true;
            aborted_threads++; 
        }
    }

    inline bool aborting() { if (abort_ == false && aborted_threads == 0) { return false; } else { return true; } }

    SDKTest(ULONG thread_id)
    {
        this->abort_        = false;
        this->thread_id_    = thread_id;
        this->cycle_        = 0;
        this->bt_handle_    = 0;

        memset(&bt_name_[0], 0, MAX_BT_NAME_LEN);
        sprintf(bt_name_, "%s%02ld", "SDKTest", thread_id_);
    }

    bool pause (ULONG pause_ms)
    {
        if (pause_ms == 0) { return true; }
        
        try
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(pause_ms));
        }
        catch (boost::thread_interrupted&)
        {
            cerr << "thread " << this->thread_id_ << " interrupted, aborting" << endl;
            exit(-1);
        }

        return true;
    }

    
    bool sdk_api_bt_begin()
    {
        if (aborting()) { return false; }

        if (noop_SDK)
        {
            noop_bt_begin_calls_made++;
        }
        else
        {
            bt_handle_ = appd_bt_begin(bt_name_, NULL);
            bt_begin_calls_made++;
        }

        pause(rate_pause);

        return true;

    } // end of sdk_api_bt_begin()

    bool sdk_api_bt_end()
    {
        if (aborting()) { return false; }

        if (noop_SDK)
        {
            noop_bt_end_calls_made++;
        }
        else
        {
            appd_bt_end(bt_handle_);
            bt_end_calls_made++;
        }

        return true;

    }


    void execute()
    {
        while (!aborting() && cycle_ < limit)
        {
            sdk_api_bt_begin();
            sdk_api_bt_end();
            cycle_++;
        }

        if (aborting() == false)
        {
            cerr << "All Cycles Completed Successfully for Thread" << thread_id_ << endl;
        }

        return;

    }

public:

    static bool sdk_api_initialize()
    {
        bt_begin_calls_made    = 0;
        bt_end_calls_made      = 0;
        noop_bt_begin_calls_made = 0;
        noop_bt_end_calls_made = 0;

        controller_port        = vm [ (cntrlPortToken.substr(0,             cntrlPortToken.find(',')))].as<ULONG>();
        rate_pause             = vm [ (ratePauseToken.substr(0,             ratePauseToken.find(',')))].as<ULONG>();
        wait                   = vm [ (initWaitToken.substr(0,               initWaitToken.find(',')))].as<ULONG>();
        limit                  = vm [ (cyclesToken.substr(0,                   cyclesToken.find(',')))].as<ULONG>();
        term_pause             = vm [ (termPauseToken.substr(0,             termPauseToken.find(',')))].as<ULONG>();
        threads                = vm [ (threadCountToken.substr(0,         threadCountToken.find(',')))].as<ULONG>();
        proxy_port             = vm [ (proxyPortToken.substr(0,             proxyPortToken.find(',')))].as<ULONG>();
        log_level              = vm [ (logLevelToken.substr(0,               logLevelToken.find(',')))].as<ULONG>();
        noop_SDK               = vm [ (noopSDKToken.substr(0,                 noopSDKToken.find(',')))].as<bool>();
        use_SSL                = vm [ (useSSLToken.substr(0,                   useSSLToken.find(',')))].as<bool>();
        use_cert_file          = vm [ (useCertFileToken.substr(0,         useCertFileToken.find(',')))].as<bool>();
        use_proxy              = vm [ (useProxyToken.substr(0,               useProxyToken.find(',')))].as<bool>();
        controller_host        = vm [ (cntrlHostToken.substr(0,             cntrlHostToken.find(',')))].as<string>().c_str();
        app_name               = vm [ (appNameToken.substr(0,                 appNameToken.find(',')))].as<string>().c_str();
        tier_name              = vm [ (tierNameToken.substr(0,               tierNameToken.find(',')))].as<string>().c_str();
        node_name              = vm [ (nodeNameToken.substr(0,               nodeNameToken.find(',')))].as<string>().c_str();
        acct_name              = vm [ (acctNameToken.substr(0,               acctNameToken.find(',')))].as<string>().c_str();
        access_key             = vm [ (accessKeyToken.substr(0,             accessKeyToken.find(',')))].as<string>().c_str();
        proxy_host             = vm [ (proxyHostToken.substr(0,             proxyHostToken.find(',')))].as<string>().c_str();
        cert_file              = vm [ (certFileToken.substr(0,               certFileToken.find(',')))].as<string>().c_str();
        cert_dir               = vm [ (certDirToken.substr(0,                 certDirToken.find(',')))].as<string>().c_str();
        proxy_user             = vm [ (proxyUserToken.substr(0,             proxyUserToken.find(',')))].as<string>().c_str();
        proxy_pswd             = vm [ (proxyPswdToken.substr(0,             proxyPswdToken.find(',')))].as<string>().c_str();
        proxy_pswd_file        = vm [ (proxyFileToken.substr(0,             proxyFileToken.find(',')))].as<string>().c_str();


        if (use_cert_file)
        {
            cerr << endl;
            cerr << "Using Certification File" << endl;
            cerr << "Certificate File \"" << cert_file << "\"" << endl;
            cerr << "Certificate Directory \"" << cert_dir << "\"" << endl;
        }
        else
        {
            cerr << endl;
            cerr << "No Certification File" << endl;
        }

        if (use_proxy)
        {
            cerr << endl;
            cerr << "Using Proxy Settings:" << endl;
            cerr << "Host: \""  << proxy_host << "\"" << endl;
            cerr << "Port: " << proxy_port << endl;
            cerr << "User: \"" << proxy_user << "\"" << endl;
            cerr << "Password: \"" << proxy_pswd << "\""  << endl;
            cerr << "Password File: \"" << proxy_pswd_file << "\""  << endl;
            cerr << "LogLevel: \"" << log_level<< "\""  << endl;
        }
        else
        {
            cerr << endl;
            cerr << "No HTTP Proxy" << endl;
        }


        cerr << endl;
        cerr << "Controller Wait Timeout is " << (wait/1000) << " seconds." << endl;
        cerr << "Controller Host is \"" << controller_host << "\"" << endl;
        cerr << "Controller Port is " << controller_port << endl;
        cerr << "Controller useSSL is " <<  (use_SSL ? "true" : "false") << endl;
        cerr << "Controller Application is \"" << app_name << "\"" << endl;
        cerr << "Controller Tier is \"" << tier_name << "\"" << endl;
        cerr << "Controller Node is \"" << node_name << "\"" << endl;
        cerr << "Controller Account Name is \"" << acct_name << "\"" << endl;
        cerr << "Access Key is \"" << access_key << "\"" << endl;
        cerr << "Logging level is " << log_level << endl;

        if (noop_SDK)
        {
            cerr << "noop'ing call to appd_config_init()." << endl;
        }
        else
        {
            cfg = appd_config_init();

            if (cfg == NULL)
            {
                cerr << "appd_config_init() failed!" << endl;
                exit(1);
            }

            appd_config_set_init_timeout_ms(cfg, wait);
            appd_config_set_app_name(cfg, app_name);
            appd_config_set_tier_name(cfg, tier_name);
            appd_config_set_node_name(cfg, node_name);
            appd_config_set_controller_host(cfg, controller_host);
            appd_config_set_controller_port(cfg, controller_port);
            appd_config_set_controller_account(cfg, acct_name);
            appd_config_set_controller_access_key(cfg, access_key);
            appd_config_set_controller_use_ssl(cfg, use_SSL);
            appd_config_set_logging_min_level(cfg, (enum appd_config_log_level) log_level);

            if (use_cert_file)
            {
                appd_config_set_controller_certificate_file(cfg, cert_file);
                appd_config_set_controller_certificate_dir( cfg, cert_dir);
            }

            if (use_proxy)
            {
                appd_config_set_controller_http_proxy_host(         cfg, proxy_host);
                appd_config_set_controller_http_proxy_port(         cfg, proxy_port & 0xFFFF);
                appd_config_set_controller_http_proxy_username(     cfg, proxy_user);
                appd_config_set_controller_http_proxy_password(     cfg, proxy_pswd);
                appd_config_set_controller_http_proxy_password_file(cfg, proxy_pswd_file);
            }
        }

        if (rate_pause > 1000)
        {
            cerr << "Rate " << rate_pause << " outside range, setting to full speed." << endl;
            rate_pause = 0;
        }
        else
        {
            cerr << "Will pause " << rate_pause << "ms after each appd_bt_begin() call." << endl;
        }

        if (limit < 1 || limit > MAX_CYCLES)
        {
            cerr << "Limit " << limit << " outside range, assigning value of " << MAX_CYCLES << " cycles." << endl;
            limit = MAX_CYCLES;
        }
        else
        {
            cerr << "Limit of " << limit << " cycles will be run." << endl;
        }

        if (noop_SDK)
        {
            cerr << "noop'ing call to appd_sdk_init() call." << endl;
        }
        else
        {
            cerr << "appd_sdk_init() call waiting at most " << wait << " milliseconds for Controller registration." << endl;
            auto rc = appd_sdk_init(cfg);
            if (rc != 0) { return false; }
        }

        return true;
    }



}; // end of SDKTest class.


atomic<ULONG>  SDKTest::bt_begin_calls_made;
atomic<ULONG>  SDKTest::bt_end_calls_made;
atomic<ULONG>  SDKTest::noop_bt_begin_calls_made;
atomic<ULONG>  SDKTest::noop_bt_end_calls_made;
ULONG          SDKTest::threads;
ULONG          SDKTest::term_pause;
ULONG          SDKTest::rate_pause;
ULONG          SDKTest::limit;
ULONG          SDKTest::wait;
ULONG          SDKTest::controller_port;
ULONG          SDKTest::proxy_port;
ULONG          SDKTest::log_level;
bool           SDKTest::use_cert_file;
bool           SDKTest::use_proxy;
bool           SDKTest::noop_SDK;
bool           SDKTest::use_SSL;
const char*    SDKTest::cert_file;
const char*    SDKTest::cert_dir;
const char*    SDKTest::app_name;
const char*    SDKTest::proxy_host;
const char*    SDKTest::proxy_user;
const char*    SDKTest::proxy_pswd;
const char*    SDKTest::proxy_pswd_file;
const char*    SDKTest::controller_host;
const char*    SDKTest::acct_name;
const char*    SDKTest::access_key;
const char*    SDKTest::tier_name;
const char*    SDKTest::node_name;

appd_config*   SDKTest::cfg;

int main(int argc, char* argv[])
{
    // Set up signal handler.
    catch_signals();

    // Create Boost program_options structure.
    po::options_description cmdline_opts("Allowed command line options");

    // There are the command line options.  NOTE: They won't necessarily match the inifile options!!
    cmdline_opts.add_options()
            (helpToken.c_str(),         po::bool_switch()->default_value(helpInit),          "")
            (noopSDKToken.c_str(),      po::bool_switch()->default_value(noopSDKInit),       "")
            (acctNameToken.c_str(),     po::value<string>()->default_value(acctNameInit),    "")
            (appNameToken.c_str(),      po::value<string>()->default_value(appNameInit),     "")
            (cyclesToken.c_str(),       po::value<ULONG>()->default_value(cyclesInit),       "")
            (cntrlHostToken.c_str(),    po::value<string>()->default_value(cntrlHostInit),   "")
            (accessKeyToken.c_str(),    po::value<string>()->default_value(accessKeyInit),   "")
            (nodeNameToken.c_str(),     po::value<string>()->default_value(nodeNameInit),    "")
            (cntrlPortToken.c_str(),    po::value<ULONG>()->default_value(cntrlPortInit),    "")
            (useSSLToken.c_str(),       po::bool_switch()->default_value(useSSLInit),        "")
            (threadCountToken.c_str(),  po::value<ULONG>()->default_value(threadCountInit),  "")
            (tierNameToken.c_str(),     po::value<string>()->default_value(tierNameInit),    "")
            (initWaitToken.c_str(),     po::value<ULONG>()->default_value(initWaitInit),     "")
            (useCertFileToken.c_str(),  po::bool_switch()->default_value(useCertFileInit),   "")
            (certFileToken.c_str(),     po::value<string>()->default_value(certFileInit),    "")
            (certDirToken.c_str(),      po::value<string>()->default_value(certDirInit),     "")
            (useProxyToken.c_str(),     po::bool_switch()->default_value(useProxyInit),      "")
            (proxyHostToken.c_str(),    po::value<string>()->default_value(proxyHostInit),   "")
            (proxyPortToken.c_str(),    po::value<ULONG>()->default_value(proxyPortInit),    "")
            (proxyUserToken.c_str(),    po::value<string>()->default_value(proxyUserInit),   "")
            (proxyPswdToken.c_str(),    po::value<string>()->default_value(proxyPswdInit),   "")
            (proxyFileToken.c_str(),    po::value<string>()->default_value(proxyFileInit),   "")
            (logLevelToken.c_str(),     po::value<ULONG>()->default_value(logLevelInit),     "")
            (termPauseToken.c_str(),    po::value<ULONG>()->default_value(termPauseInit),    "")
            (ratePauseToken.c_str(),    po::value<ULONG>()->default_value(ratePauseInit),    "")
            ;

    try
    {
        po::positional_options_description p;
        po::store(po::command_line_parser(argc, argv).options(cmdline_opts).positional(p).run(), vm);
        po::notify(vm);

        auto display_help =  vm [ (helpToken.substr(0,                 helpToken.find(',')))].as<bool>();
        if (display_help)
        {
            cout << cmdline_opts << "\n";
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        cerr << " " << e.what() << endl;
        exit(FAIL_RC);
    }

    try
    {
        SDKTest**             workerThreads = new SDKTest* [MAX_THREADS];
        boost::thread_group     workerThreadGroup;
    
        if (SDKTest::sdk_api_initialize() != true)
        {
            cerr  << "SDKTest::sdk_api_initialize() failed."
                  << endl;
            exit(FAIL_RC);
        }
    
        cerr << "Creating " << SDKTest::threads << " worker threads, mainline will wait for them to complete." << endl;

        for (ULONG loop = 1; loop <= SDKTest::threads; loop++)
        {
            if (aborted_threads > 0 || interrupt_recvd == true) { break; }
            workerThreads[loop] = new SDKTest(loop);
            workerThreadGroup.create_thread(boost::bind(&SDKTest::execute, workerThreads[loop]));
        }
    
        if (aborted_threads == 0 && interrupt_recvd != true) 
        {
            workerThreadGroup.join_all();
            cerr << "All " << SDKTest::threads << " worker threads have completed." << endl;
        }
    }
    catch (runtime_error&)
    {
        cerr  << "Program recieved external SIGNAL, exiting now." << endl;
    }

    if (SDKTest::noop_SDK)
    {
        cerr << "Noop'ed call to appd_sdk_term()." << endl;
    }
    else
    {
        cerr << "About to sleep " << SDKTest::term_pause << " seconds before calling appd_sdk_term()." << endl;
        sleep(SDKTest::term_pause);
        appd_sdk_term();
    }

    cerr  << "All SDK API calls have completed." << endl;

    if (SDKTest::noop_SDK)
    {
        cerr << left << setw(17) << "Noop'ed BT Begin Calls Made:  " << right << setw(30) << SDKTest::noop_bt_begin_calls_made << endl;
        cerr << left << setw(17) << "Noop'ed BT End Calls Made:    " << right << setw(30) << SDKTest::noop_bt_end_calls_made << endl;
    }
    else
    {
        cerr << left << setw(17) << "BT Begin Calls Made:   " << right << setw(25) << SDKTest::bt_begin_calls_made << endl;
        cerr << left << setw(17) << "BT End Calls Made:     " << right << setw(25) << SDKTest::bt_end_calls_made << endl;
    }

    cerr << endl
         << "\"" << argv[0] << "\" exiting with " << aborted_threads
         << " threads that aborted due to errors."
         << endl << endl;

    exit(aborted_threads);

}

// end of SDKTest.cpp file.
