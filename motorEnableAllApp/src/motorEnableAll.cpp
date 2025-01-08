#include <dbAccess.h>
#include <dbDefs.h>
#include <dbStaticLib.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <string>
#include <vector>

// 1. Get list of motor records via static databse access before database initialization
// 2. For each motor in the list, set m.CNEN="Enable"/"Disable"
// - Create channels and chids for each motor only once
// - 

// declared externally to avoid duplicate definitions in dbAccess.h and cadef.h
extern double get_mres(const std::string &pv);

static std::vector<std::string> rec_list;

void motorEnableAllInit() {

    // Initialize DBENTRY
    DBENTRY *pdbentry;
    pdbentry = dbAllocEntry(pdbbase);
    dbInitEntry(pdbbase, pdbentry);

    // Check if there are any motor records at all
    long status = dbFindRecordType(pdbentry, "motor");
    if (status) {
        errlogPrintf("motorEnableAllInit(): No motor records found\n");
        return;
    }

    // Get names of motor records in dbd
    int num_entries = dbGetNRecords(pdbentry);
    errlogPrintf("Found %d motor records\n", num_entries);
    status = dbFirstRecord(pdbentry);
    while (!status) {
        rec_list.push_back(dbGetRecordName(pdbentry));
        status = dbNextRecord(pdbentry);
    }
    dbFinishEntry(pdbentry);

}

//
// iocsh registration
//
static const iocshArg motorEnableAllInitArg0 = {"Enable (CNEN) all motors", iocshArgString};
static const iocshArg *const motorEnableAllInitArgs[1] = {&motorEnableAllInitArg0};
static const iocshFuncDef motorEnableAllInitDef = {"motorEnableAll", 1, motorEnableAllInitArgs};
static void motorEnableAllInitFunc(const iocshArgBuf *args) {
    motorEnableAllInit();
}
static void motorEnableAllInitRegister(void) {
    iocshRegister(&motorEnableAllInitDef, motorEnableAllInitFunc);
}

extern "C" {
epicsExportRegistrar(motorEnableAllInitRegister);
}
