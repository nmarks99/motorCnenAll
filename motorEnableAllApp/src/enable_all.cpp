#include <string>
#include <vector>

#include <cadef.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
// #include <dbDefs.h>
// #include <cantProceed.h>


static constexpr double CA_IO_TIMEOUT = 5.0; // seconds

// Defined externally due to duplicate #define's in dbAccess.h and cadef.h
extern std::vector<std::string> get_record_names();

static std::vector<std::string> record_names;
static std::vector<chid> chid_list;

// static chid get_chid(const std::string &record_name) {
    // chid channel_id = 0;
//
    // // int status = ca_context_create(ca_enable_preemptive_callback);
    // ca_create_channel(record_name.c_str(), NULL, NULL, 10, &channel_id);
    // ca_pend_io(5.0);
    // // if (status == ECA_NORMAL) {
        // // errlogPrintf("normal\n");
        // // status = ca_pend_io(CA_IO_TIMEOUT);
    // // } else if (status != ECA_NORMAL) {
        // // SEVCHK(status, "ca_create_channel");
        // // errlogPrintf("motorEnableAll: getChID(%s) error: %i\n", record_name.c_str(), status);
	// // channel_id = 0;
    // // } else {
        // // errlogPrintf("something else\n");
    // // }
    // return channel_id;
// }

// static long create_pv_monitor(int event_type, chid channel_id) {
    // // status = ca_add_event(DBR_SHORT, channelID, &dmov_handler, &(motorArray[motor_index].index), 0);
// }

// must be done after IOC init
void motorEnableAllInit() {
    static bool initialized = false;
    if (initialized) {
        errlogPrintf("motorEnableAll already initialized\n");
        return;
    }

    // Create channel access context
    SEVCHK(ca_context_create(ca_enable_preemptive_callback), "motorEnableAllInit: ca_context_create() error");

    record_names = get_record_names();
    initialized = true;
    if (record_names.size() == 0) {
        errlogPrintf("motorEnableAllInit: No motor records found\n");
        return;
    }

    for (const auto &rec : record_names) {
        errlogPrintf("Found record: %s\n", rec.c_str());
        chid channel_id = 0;
        ca_create_channel((rec+".CNEN").c_str(), NULL, NULL, 10, &channel_id);
        ca_pend_io(5.0);
        chid_list.push_back(channel_id);
    }
}

void motorEnableAll() {
    short val = 1;
    for (size_t i = 0; i < chid_list.size(); i++) {
        ca_array_put(DBF_SHORT, 1, chid_list.at(i), &val);
        ca_flush_io();
    }
}

// double get_mres(const std::string &pv) {
    // double mres;
    // chid motor_chid;
//
    // ca_context_create(ca_enable_preemptive_callback);
    // std::string pv_name = pv + ".MRES";
    // ca_create_channel(pv_name.c_str(), NULL, NULL, 10, &motor_chid);
    // ca_pend_io(5.0);
    // ca_get(DBR_DOUBLE, motor_chid, (void *)&mres);
    // ca_pend_io(5.0);

    // return mres;
// }

//
// iocsh registration
//
static const iocshArg motorEnableAllInitArg0 = {"Enable (CNEN) all motors", iocshArgString};
static const iocshArg *const motorEnableAllInitArgs[1] = {&motorEnableAllInitArg0};
static const iocshFuncDef motorEnableAllInitDef = {"motorEnableAllInit", 1, motorEnableAllInitArgs};
static void motorEnableAllInitFunc(const iocshArgBuf *args) {
    motorEnableAllInit();
}
static void motorEnableAllInitRegister(void) {
    iocshRegister(&motorEnableAllInitDef, motorEnableAllInitFunc);
}

static const iocshArg motorEnableAllArg0 = {"Enable (CNEN) all motors", iocshArgString};
static const iocshArg *const motorEnableAllArgs[1] = {&motorEnableAllArg0};
static const iocshFuncDef motorEnableAllDef = {"motorEnableAll", 1, motorEnableAllArgs};
static void motorEnableAllFunc(const iocshArgBuf *args) {
    motorEnableAll();
}
static void motorEnableAllRegister(void) {
    iocshRegister(&motorEnableAllDef, motorEnableAllFunc);
}

extern "C" {
epicsExportRegistrar(motorEnableAllInitRegister);
epicsExportRegistrar(motorEnableAllRegister);
}
