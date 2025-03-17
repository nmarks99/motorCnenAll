#include <dbAccess.h>
#include <dbDefs.h>
#include <dbStaticLib.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <string>
#include <vector>

// Get list of all motor records in the IOC via static database access
std::vector<std::string> get_record_names() {
    std::vector<std::string> rec_list;

    // Initialize DBENTRY
    DBENTRY *pdbentry;
    pdbentry = dbAllocEntry(pdbbase);
    dbInitEntry(pdbbase, pdbentry);

    // Check if there are any motor records at all
    long status = dbFindRecordType(pdbentry, "motor");
    if (status) {
        errlogPrintf("No motor records found\n");
        return rec_list;
    }

    // Get names of motor records in dbd
    status = dbFirstRecord(pdbentry);
    while (!status) {
        rec_list.push_back(dbGetRecordName(pdbentry));
        status = dbNextRecord(pdbentry);
    }
    dbFinishEntry(pdbentry);

    return rec_list;
}
