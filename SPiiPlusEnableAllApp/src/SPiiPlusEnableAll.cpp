#include <string>
#include <vector>
#include <algorithm>
#include <regex>

#include <cadef.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <registryFunction.h>
// #include <dbDefs.h>
// #include <cantProceed.h>


static constexpr double CA_IO_TIMEOUT = 5.0; // seconds

// Defined externally due to duplicate #define's in dbAccess.h and cadef.h
extern std::vector<std::string> get_record_names();

static std::vector<std::string> record_names;
static std::vector<chid> chid_list;

static chid get_chid(const std::string &record_name) {
    chid channel_id = 0;

    // int status = ca_context_create(ca_enable_preemptive_callback);
    int status = ca_create_channel(record_name.c_str(), NULL, NULL, 10, &channel_id);
    if (status == ECA_NORMAL) {
        status = ca_pend_io(CA_IO_TIMEOUT);
    } else {
        SEVCHK(status, "ca_create_channel");
        errlogPrintf("get_chid(%s) error: %i\n", record_name.c_str(), status);
        channel_id = 0;
    } 
    return channel_id;
}

// User can specify specific motors in the form of a comma separated list of motor names.
// It is left to the user to ensure the requested motors actually exist
static std::vector<std::string> parse_motor_list(const std::string &prefix, const std::string &motor_list) {

    // remove whitespace
    std::string result = motor_list;
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());

    // split by ","
    std::regex re(",");
    std::sregex_token_iterator begin(result.begin(), result.end(), re, -1);
    std::sregex_token_iterator end;

    // prepend IOC prefix to each motor record name
    std::vector<std::string> out;
    for (const auto &s: std::vector<std::string>({begin, end})) {
        out.push_back(prefix+s);
    }

    return out;

}

// Must be called from iocsh after iocInit
// prefix and motor_list are optional and if either are empty, all
// motor records in the IOC will be connected to
void SPiiPlusEnableAllInit(const char *prefix, const char *motor_list) {
    static bool initialized = false;
    if (initialized) {
        errlogPrintf("SPiiPlusEnableAll already initialized\n");
        return;
    }

    // Create channel access context
    SEVCHK(ca_context_create(ca_enable_preemptive_callback), "SPiiPlusEnableAllInit: ca_context_create() error");

    // Get the motor record names
    std::vector<std::string> record_names;
    if (motor_list == NULL || prefix == NULL) {
        // get all motor records in this IOC
        record_names = get_record_names();
        initialized = true;
        if (record_names.size() == 0) {
            errlogPrintf("SPiiPlusEnableAllInit: No motor records found\n");
            return;
        }
    } else {
        // get only requested motor records
        record_names = parse_motor_list(prefix, motor_list);
        if (record_names.size() == 0) {
            errlogPrintf("SPiiPlusEnableAllInit: Error parsing requested motors:\n%s\n", motor_list);
            return;
        }
    }

    // Connect to each CNEN PV and store the channel ID
    for (const auto &rec : record_names) {
        errlogPrintf("Record: %s\n", rec.c_str());
        chid_list.push_back(get_chid(rec+".CNEN"));
    }
}


// Enables all requested motors (CNEN=1)
// Registered as function with epicsRegisterFunction and called by sub record
long SPiiPlusEnableAll() {
    short val = 1;
    for (size_t i = 0; i < chid_list.size(); i++) {
        ca_array_put(DBF_SHORT, 1, chid_list.at(i), &val);
        ca_flush_io();
    }
    return 0;
}

// Disables all requested motors (CNEN=0)
// Registered as function with epicsRegisterFunction and called by sub record
long SPiiPlusDisableAll() {
    short val = 0;
    for (size_t i = 0; i < chid_list.size(); i++) {
        ca_array_put(DBF_SHORT, 1, chid_list.at(i), &val);
        ca_flush_io();
    }
    return 0;
}


// ------------------
// iocsh registration
// ------------------
static const iocshArg SPiiPlusEnableAllInitArg0 = {"IOC prefix", iocshArgString};
static const iocshArg SPiiPlusEnableAllInitArg1 = {"Motors list", iocshArgString};
static const iocshArg *const SPiiPlusEnableAllInitArgs[2] = {
    &SPiiPlusEnableAllInitArg0,
    &SPiiPlusEnableAllInitArg1
};
static const iocshFuncDef SPiiPlusEnableAllInitDef = {"SPiiPlusEnableAllInit", 2, SPiiPlusEnableAllInitArgs};
static void SPiiPlusEnableAllInitFunc(const iocshArgBuf *args) {
    SPiiPlusEnableAllInit(args[0].sval, args[1].sval);
}
static void SPiiPlusEnableAllInitRegister(void) {
    iocshRegister(&SPiiPlusEnableAllInitDef, SPiiPlusEnableAllInitFunc);
}

static const iocshArg SPiiPlusEnableAllArg0 = {"Enable (CNEN) all motors", iocshArgString};
static const iocshArg *const SPiiPlusEnableAllArgs[1] = {&SPiiPlusEnableAllArg0};
static const iocshFuncDef SPiiPlusEnableAllDef = {"SPiiPlusEnableAll", 1, SPiiPlusEnableAllArgs};
static void SPiiPlusEnableAllFunc(const iocshArgBuf *args) {
    SPiiPlusEnableAll();
}
static void SPiiPlusEnableAllRegister(void) {
    iocshRegister(&SPiiPlusEnableAllDef, SPiiPlusEnableAllFunc);
}

extern "C" {
epicsExportRegistrar(SPiiPlusEnableAllInitRegister);
epicsExportRegistrar(SPiiPlusEnableAllRegister);
epicsRegisterFunction(SPiiPlusEnableAll);
epicsRegisterFunction(SPiiPlusDisableAll);
}
