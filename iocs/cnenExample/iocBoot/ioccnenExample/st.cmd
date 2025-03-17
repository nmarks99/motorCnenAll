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
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX),PORT=motorSim0")
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX),PORT=motorSim1")
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX),PORT=list1")
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX),PORT=all")

###############################################################################
iocInit
###############################################################################

# Initialize motorCnenAll for each desired port.
# Must match the PORT macro passed to the dbLoadRecords calls above.

# Initialize motorSim0
motorCnenAllInit("motorSim0")

# Initialize motorSim1
motorCnenAllInit("motorSim1")

# Initialize list1
motorCnenAllInitList("list1", "$(PREFIX)", "m1,m3, m5, m16")

# No argument will create port called "all"
motorCnenAllInit()
