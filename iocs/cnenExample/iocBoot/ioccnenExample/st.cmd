# ../../bin/${EPICS_HOST_ARCH}/cnenExample st.cmd
< envPaths

dbLoadDatabase("../../dbd/ioccnenExampleLinux.dbd")
ioccnenExampleLinux_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("IOCSH_PS1", "$(IOC)>")
epicsEnvSet("PREFIX", "cnenExample:")

# load 16 motors
# First 1-8 are on controller 0 (motorSim0)
# First 9-16 are on controller 1 (motorSim1)
< motorSim.iocsh

# Load motorCnenEnableAll and motorCnenDisableAll PVs
# dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)")
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX),PORT=motorSim0")
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX),PORT=motorSim1")

###############################################################################
iocInit
###############################################################################

# Example 2: Initialize all motors with asyn port motorSim0
motorCnenAllInit("motorSim0")
motorCnenAllInit("motorSim1")
