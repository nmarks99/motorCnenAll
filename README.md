# motorCnenAll
EPICS software for easily creating PVs to set the CNEN field for all (or a subset of) motors in an IOC.

***Motivation***  
Several EPICS motor drivers, like [motorAcsMotion](https://github.com/epics-motor/motorAcsMotion) require
each axis to be enabled by setting the motor record's CNEN field to 1. Users often find this annoying
especially when the ACS controller is rebooted and it is not clear that all the motors will start disabled.
This software attempts to solve part of this annoyance by making it very easy to create "Enable All" and "Disable All" PVs.

## Usage

An example IOC can be found in the `iocs` directory.

In your IOC startup script, before iocInit, load the motorCnenAll.db database.
```
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)")
```

After iocInit you will need to call one of the initialization functions:
```bash
# Example 1: Tell motorCnenAll about all motors in the IOC
motorCnenAllInit()

# Example 2: Only motors with asyn port ACS1
motorCnenAllInit("ACS1")

# Example 3: Only motors m1, m5, and m13
motorCnenAllInitList("$(PREFIX)", "m1, m5, m13")

# Example 4: Only motors with asyn port ACS1 and motors m1 and m3
motorCnenAllInit("ACS1")
motorCnenAllInitList("$(PREFIX)", "m1, m3")

# Example 5: Only motors with asyn port ACS1 or ACS2
# Note: this is only function that can be called multiple times
motorCnenAllInit("ACS1")
motorCnenAllInit("ACS2")
```
***Note:*** motorCnenAllInit (with no arguments) and motorCnenAllInitList can only be called once.

Now that the IOC is running and motorCnenAll has been initialized,
- To enable all intialized motors (set CNEN=1), process the $(P)motorCnenEnableAll PV
- To disable all intialized motors (set CNEN=0), process the $(P)motorCnenDisableAll PV
