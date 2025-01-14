# ../../bin/${EPICS_HOST_ARCH}/acsExample st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocacsExampleLinux.dbd")
iocacsExampleLinux_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("IOCSH_PS1", "$(IOC)>")
epicsEnvSet("PREFIX", "acsExample:")

# load 16 motors
# First 1-8 are on controller 0 (motorSim0)
# First 9-16 are on controller 1 (motorSim1)
< motorSim.iocsh

# Load motorCnenEnableAll and motorCnenDisableAll PVs
dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)")

###############################################################################
iocInit
###############################################################################

# Below are three examples on how to initialize motorCnenAll
# This must be after iocInit. Note you can use doAfterIocInit if your IOC
# include the synApps std module

# Example 1: Initialize all motors in the IOC for use with motorCnenAll
motorCnenAllInit()

# # Example 2: Initialize all motors with asyn port motorSim0
# motorCnenAllInit("motorSim0")

# # Example 3: Initialize motors m1, m5, and m13
# motorCnenAllInitList("$(PREFIX)", "m1, m5, m13")

# # Example 4: Initialize all motors on controller 1 and motors m1 and m3
# motorCnenAllInit("motorSim1")
# motorCnenAllInitList("$(PREFIX)", "m1, m3")
