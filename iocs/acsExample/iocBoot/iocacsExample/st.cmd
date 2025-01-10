# ../../bin/${EPICS_HOST_ARCH}/acsExample st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocacsExampleLinux.dbd")
iocacsExampleLinux_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

< ./motorSim.iocsh

dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)")

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date


# # all motors in this IOC
# motorCnenAllInit()

# # only certain motors
# motorCnenAllInit("$(PREFIX)", "m1, m3, m2, m4")

# Filter by asyn port name
# motorCnenAllInitPort("motorSim0")
