#ifndef SDK_REGRESSTION_TEST_PROGRAM_OPTIONS_H
#define SDK_REGRESSTION_TEST_PROGRAM_OPTIONS_H


using boost::timer::cpu_timer;
using boost::timer::cpu_times;
using boost::timer::nanosecond_type;
using boost::chrono::steady_clock;


namespace po = boost::program_options;

po::variables_map vm;

template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
    return os;
}

const string       noopSDKToken("NoSDK");
const bool         noopSDKInit(false);

const string       acctNameToken("AccountName");
const string       acctNameInit("customer1");

const string       appNameToken("Application");
const string       appNameInit("SDKTest");

const string       cyclesToken("Cycles");
const ULONG        cyclesInit(1000000000);

const string       accessKeyToken("AccessKey");
const string       accessKeyInit("ERROR_ACCESS_KEY_NOT_SET");

const string       nodeNameToken("NodeName");
const string       nodeNameInit("SDKTestNode1");

const string       termPauseToken("PauseBeforeTerm");
const ULONG        termPauseInit(60);

const string       ratePauseToken("PauseBeforeBTBegin");
const ULONG        ratePauseInit(0);

const string       cntrlHostToken("ControllerHost");
const string       cntrlHostInit("ERROR_CONTROLLER_HOST_NOT_SET");

const string       cntrlPortToken("ControllerPort");
const ULONG        cntrlPortInit(0);

const string       useSSLToken("UseSSL");
const bool         useSSLInit(false);

const string       threadCountToken("Threads");
const ULONG        threadCountInit(5);

const string       tierNameToken("TierName");
const string       tierNameInit("SDKTestTier1");

const string       initWaitToken("InitWait");
const ULONG        initWaitInit(50000);

const string       useCertFileToken("UseCertFile");
const bool         useCertFileInit(false);

const string       certFileToken("CertFile");
const string       certFileInit("ERROR_CERTIFICATE_FILENAME_MISSING");

const string       certDirToken("CertDir");
const string       certDirInit("ERROR_CERTIFICATE_DIRECTORY_PATHNAME_MISSING");

const string       useProxyToken("UseHTTPProxy");
const bool         useProxyInit(false);

const string       proxyHostToken("HTTPProxyHost");
const string       proxyHostInit("ERROR_HTTP_PROXY_HOSTNAME_MISSING");

const string       proxyPortToken("HTTPProxyPort");
const ULONG        proxyPortInit(0);

const string       proxyUserToken("HTTPProxyUser");
const string       proxyUserInit("ERROR_HTTP_PROXY_NAME_MISSING");

const string       proxyPswdToken("HTTPProxyPswd");
const string       proxyPswdInit("ERROR_HTTP_PROXY_PASSWORD_MISSING");												// This must be NULL, only ever specified by user override.

const string       proxyFileToken("HTTPProxyFile");
const string       proxyFileInit("ERROR_HTTP_PROXY_FILE_MISSING");												// This must be NULL, only ever specified by user override.

const string       logLevelToken("LogLevel");
const ULONG        logLevelInit(0);

const string       helpToken("help");
const bool         helpInit(false);


#endif // SDK_REGRESSTION_TEST_PROGRAM_OPTIONS_H
// end of file sdk_regression_test_program_options.h
