#include <algorithm>
#include <regex>
#include <string>
#include <vector>

#include <cadef.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <registryFunction.h>

static constexpr double CA_IO_TIMEOUT = 5.0; // seconds
static bool initialized = false;
static bool context_created = false;

// Defined externally due to duplicate #define's in dbAccess.h and cadef.h
extern std::vector<std::string> get_record_names();

static std::vector<std::string> record_names;
static std::vector<std::string> port_names;
static std::vector<chid> chid_list;


// Connect to a channel and return the channel ID for the given PV
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
static std::vector<std::string> parse_motor_list(const std::string &prefix,
                                                 const std::string &motor_list) {

    // remove whitespace
    std::string result = motor_list;
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());

    // split by ","
    std::regex re(",");
    std::sregex_token_iterator begin(result.begin(), result.end(), re, -1);
    std::sregex_token_iterator end;

    // prepend IOC prefix to each motor record name
    std::vector<std::string> out;
    for (const auto &s : std::vector<std::string>({begin, end})) {
        out.push_back(prefix + s);
    }

    return out;
}


// Gets CA channel ID's for all motor records in the IOC with the given
// asyn port name present in their OUT field
void motorCnenAllInit(const char *asyn_port) {
    if (initialized) {
        errlogPrintf("motorCnenAllInit: already initialized\n");
        return;
    }

    // Create channel access context
    if (!context_created) {
        SEVCHK(ca_context_create(ca_enable_preemptive_callback), "motorCnenAllInit: ca_context_create() error");
        context_created = true;
    } else {
        errlogPrintf("CA context already created\n");
    }

    // get all motor records in this IOC, prefix is included in record_names
    record_names = get_record_names();
    if (record_names.size() == 0) {
        errlogPrintf("motorCnenAllInit: No motor records found\n");
        return;
    }

    if (asyn_port == NULL) {
        // get channel ID for all motor records (prefix is included in record_names)
        for (const auto &rec : record_names) {
            errlogPrintf("Record: %s\n", rec.c_str());
            chid_list.push_back(get_chid(rec + ".CNEN"));
        }
        initialized = true;
    } else {
        // if the requested asyn port has already been checked, return
        if (std::find(port_names.begin(), port_names.end(), asyn_port) != port_names.end()) {
            errlogPrintf("motorCnenAllInit: already intialized with asyn port %s\n", asyn_port);
            return;
        } else {
            port_names.push_back(asyn_port);
        }

        // only store chid for motor records with OUT fields containing "asyn port" string
        for (const auto &rec : record_names) {
            chid out_chid = get_chid(rec + ".OUT");
            if (out_chid) {
                char out_buff[40];
                ca_get(DBR_STRING, out_chid, out_buff);
                ca_pend_io(CA_IO_TIMEOUT);
                if (std::string(out_buff).find(asyn_port) != std::string::npos) {
                    chid_list.push_back(get_chid(rec + ".CNEN"));
                    errlogPrintf("Record(port match): %s\n", rec.c_str()); // FIX: remove
                } else { // FIX: remove this else when not debugging
                    errlogPrintf("asyn port %s not found in record %s\n", asyn_port, rec.c_str());
                }
            }
        }
    }

}


// Must be called from iocsh after iocInit
// prefix and motor_list are optional and if either are empty, all
// motor records in the IOC will be connected to
void motorCnenAllInitList(const char *prefix, const char *motor_list) {
    if (initialized) {
        errlogPrintf("motorCnenAllInit: already initialized\n");
        return;
    }

    // Create channel access context
    if (!context_created) {
        SEVCHK(ca_context_create(ca_enable_preemptive_callback), "motorCnenAllInit: ca_context_create() error");
        context_created = true;
    } else {
        errlogPrintf("CA context already created\n");
    }

    // Get the motor record names
    if (motor_list == NULL || prefix == NULL) {
        errlogPrintf("motorCnenAllInitList: No prefix or motor list given\n");
        return;
    } else {
        // get only requested motor records, must add prefix
        record_names = parse_motor_list(prefix, motor_list);
        if (record_names.size() == 0) {
            errlogPrintf("motorCnenAllInit: Error parsing requested motors:\n%s\n",motor_list);
            return;
        }
    }

    // Connect to each CNEN PV and store the channel ID
    for (const auto &rec : record_names) {
        errlogPrintf("Record: %s\n", rec.c_str()); // FIX: remove
        chid_list.push_back(get_chid(rec + ".CNEN"));
    }

    initialized = true;
}


// Enables all requested motors (CNEN=1)
// Registered as function with epicsRegisterFunction and called by sub record
long motorCnenEnableAll() {
    short val = 1;
    for (size_t i = 0; i < chid_list.size(); i++) {
        ca_put(DBF_SHORT, chid_list.at(i), &val);
        ca_flush_io(); // FIX: is this needed?
    }
    return 0;
}


// Disables all requested motors (CNEN=0)
// Registered as function with epicsRegisterFunction and called by sub record
long motorCnenDisableAll() {
    short val = 0;
    for (size_t i = 0; i < chid_list.size(); i++) {
        ca_put(DBF_SHORT, chid_list.at(i), &val);
        ca_flush_io();
    }
    return 0;
}


// ------------------
// iocsh registration
// ------------------

// motorCnenAllInit()
static const iocshArg motorCnenAllInitArg0 = {"asyn port", iocshArgString};
static const iocshArg *const motorCnenAllInitArgs[] = {&motorCnenAllInitArg0};
static const iocshFuncDef motorCnenAllInitDef = {"motorCnenAllInit", 1, motorCnenAllInitArgs};
static void motorCnenAllInitFunc(const iocshArgBuf *args) {
    motorCnenAllInit(args[0].sval);
}
static void motorCnenAllInitRegister(void) {
    iocshRegister(&motorCnenAllInitDef, motorCnenAllInitFunc);
}


// motorCnenAllInitList()
static const iocshArg motorCnenAllInitListArg0 = {"IOC prefix", iocshArgString};
static const iocshArg motorCnenAllInitListArg1 = {"Motors list", iocshArgString};
static const iocshArg *const motorCnenAllInitListArgs[] = {
    &motorCnenAllInitListArg0,
    &motorCnenAllInitListArg1
};
static const iocshFuncDef motorCnenAllInitListDef = {"motorCnenAllInitList", 2, motorCnenAllInitListArgs};
static void motorCnenAllInitListFunc(const iocshArgBuf *args) {
    motorCnenAllInitList(args[0].sval, args[1].sval);
}
static void motorCnenAllInitListRegister(void) {
    iocshRegister(&motorCnenAllInitListDef, motorCnenAllInitListFunc);
}


extern "C" {
epicsExportRegistrar(motorCnenAllInitRegister);
epicsExportRegistrar(motorCnenAllInitListRegister);
epicsRegisterFunction(motorCnenEnableAll);
epicsRegisterFunction(motorCnenDisableAll);
}
