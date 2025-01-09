#include <dbAccess.h>
#include <dbDefs.h>
#include <dbStaticLib.h>
#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <string>
#include <vector>

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
        // TODO: return empty vector?
    }

    // Get names of motor records in dbd
    // int num_entries = dbGetNRecords(pdbentry);
    // errlogPrintf("Found %d motor records\n", num_entries);
    status = dbFirstRecord(pdbentry);
    while (!status) {
        rec_list.push_back(dbGetRecordName(pdbentry));
        status = dbNextRecord(pdbentry);
    }
    dbFinishEntry(pdbentry);

    return rec_list;
}
