#include <algorithm>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>

#include <cadef.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <registryFunction.h>
#include <aSubRecord.h>

static constexpr double CA_IO_TIMEOUT = 5.0; // seconds
static bool context_created = false;

// Defined externally due to duplicate #define's in dbAccess.h and cadef.h
extern std::vector<std::string> get_record_names();

static std::vector<std::string> record_names;
// static std::vector<chid> chid_list;

static std::unordered_map<std::string, std::vector<chid>> chid_map;

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

    // Create channel access context
    if (!context_created) {
        SEVCHK(ca_context_create(ca_enable_preemptive_callback), "motorCnenAllInit: ca_context_create() error");
        context_created = true;
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
            chid_map["all"].push_back(get_chid(rec + ".CNEN"));
        }
    } else {
        // if the requested asyn port has already been checked, return
        if (chid_map.count(asyn_port) > 0) {
            errlogPrintf("motorCnenAllInit: already intialized with asyn port %s\n", asyn_port);
            return;
        }

        // only store chid for motor records with OUT fields containing "asyn port" string
        // FIX: make it so ACS1 and ACS10 do not both match "ACS!"
        for (const auto &rec : record_names) {
            chid out_chid = get_chid(rec + ".OUT");
            if (out_chid) {
                char out_buff[40];
                ca_get(DBR_STRING, out_chid, out_buff);
                ca_pend_io(CA_IO_TIMEOUT);
                if (std::string(out_buff).find(asyn_port) != std::string::npos) {
                    chid_map[asyn_port].push_back(get_chid(rec + ".CNEN"));
                }
            }
        }
    }

}


// Must be called from iocsh after iocInit
// prefix and motor_list are optional and if either are empty, all
// motor records in the IOC will be connected to
void motorCnenAllInitList(const char *instance, const char *prefix, const char *motor_list) {

    // Create channel access context
    if (!context_created) {
        SEVCHK(ca_context_create(ca_enable_preemptive_callback), "motorCnenAllInit: ca_context_create() error");
        context_created = true;
    }

    // Get the motor record names
    if (instance == NULL || motor_list == NULL || prefix == NULL) {
        errlogPrintf("motorCnenAllInitList: Invalid arguments. Must provide, instance, IOC prefix, motor list\n");
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
        chid_map[instance].push_back(get_chid(rec + ".CNEN"));
    }

}

// Enables all requested motors (CNEN=1)
// Registered as function with epicsRegisterFunction and called by aSub record
static int motorCnenEnableAll(aSubRecord *psub) {

    std::string port_name((char *)psub->a);
        
    if (chid_map.count(port_name) > 0) {
        auto chids = chid_map.at(port_name);
        short val = 1;
        for (size_t i = 0; i < chids.size(); i++) {
            ca_put(DBF_SHORT, chids.at(i), &val);
        }
    }

    return 0;
}

// Disabled all requested motors (CNEN=0)
// Registered as function with epicsRegisterFunction and called by aSub record
static int motorCnenDisableAll(aSubRecord *psub) {

    std::string port_name((char *)psub->a);
        
    if (chid_map.count(port_name) > 0) {
        auto chids = chid_map.at(port_name);
        short val = 0;
        for (size_t i = 0; i < chids.size(); i++) {
            ca_put(DBF_SHORT, chids.at(i), &val);
        }
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
static const iocshArg motorCnenAllInitListArg0 = {"Instance", iocshArgString};
static const iocshArg motorCnenAllInitListArg1 = {"IOC prefix", iocshArgString};
static const iocshArg motorCnenAllInitListArg2 = {"Motors list", iocshArgString};
static const iocshArg *const motorCnenAllInitListArgs[] = {
    &motorCnenAllInitListArg0,
    &motorCnenAllInitListArg1,
    &motorCnenAllInitListArg2
};
static const iocshFuncDef motorCnenAllInitListDef = {"motorCnenAllInitList", 3, motorCnenAllInitListArgs};
static void motorCnenAllInitListFunc(const iocshArgBuf *args) {
    motorCnenAllInitList(args[0].sval, args[1].sval, args[2].sval);
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
