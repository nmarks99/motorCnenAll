# motorCnenAll
EPICS software for easily creating PVs to set the CNEN field for all (or a subset of) motors in an IOC.

***Motivation***  
Several EPICS motor drivers, like [motorAcsMotion](https://github.com/epics-motor/motorAcsMotion) require
each axis to be enabled by setting the motor record's CNEN field to 1. Users often find this annoying
especially when the ACS controller is rebooted and it is not clear that all the motors will start disabled.
This software attempts to solve part of this annoyance by making it very easy to create "Enable All" and "Disable All" PVs.

## Usage
*An example IOC can be found in the `iocs` directory.*

*Note:* If your IOC does not include the synApps std module, you won't have the doAfterIocInit function
that is used in the below examples. In this case, the `motorCnenAllInit` calls must be placed after
iocInit in your startup script.


### **Example 1:** Create PVs to enable/disable all motors on a given controller  
Assume two ACS Motion controllers are connected with asyn ports named "ACS1" and "ACS2" respectively

In your IOC startup script, after connecting to the controllers, add the following:
```bash
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)", "PORT=ACS1")
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)", "PORT=ACS2")
doAfterIocInit('motorCnenAllInit("ACS1")')
doAfterIocInit('motorCnenAllInit("ACS2")')
```

The above will load the following four records: $(PREFIX)ACS1:EnableAllCNEN, $(PREFIX)ACS1:DisableAllCNEN,
$(PREFIX)ACS2:EnableAllCNEN, and $(PREFIX)ACS2:DisableAllCNEN. When these records process, all motors for the given port
will be enabled/disabled.

### **Example 2:** Create PVs to enable/disable all motors in the given list

```bash
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)", "PORT=list1")
doAfterIocInit('motorCnenAllInitList("list1", "$(PREFIX)", "m1, m3, m5, m7")')
```
The above will load the following two records: $(PREFIX)list1:EnableAllCNEN and $(PREFIX)list1:DisableAllCNEN,
When these records process, motors m1, m3, m5, and m7 will be enabled/disabled.

### **Example 3:** Create PVs to enable/disable all motors in the IOC

```bash
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)", "PORT=all")
doAfterIocInit('motorCnenAllInit()')
```
Note that no argument is passed to motorCnenAllInit. This will create a "port" with a special name: "all" and two records
will be loaded named $(PREFIX)all:EnableAllCNEN and $(PREFIX)all:DisableAllCNEN which will enable/disable all motors
in the IOC.
