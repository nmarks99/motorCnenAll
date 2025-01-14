# ../../bin/${EPICS_HOST_ARCH}/acsExample st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocacsExampleLinux.dbd")
iocacsExampleLinux_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

< motorSim0.iocsh
< motorSim1.iocsh

dbLoadRecords("$(MOTOR_CNEN_ALL)/db/motorCnenAll.db", "P=$(PREFIX)")

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date


# # all motors in this IOC
# motorCnenAllInit("motorSim11")

# only certain motors
# motorCnenAllInitList("$(PREFIX)", "m1, m6, m3")

motorCnenAllInit("motorSim1")
motorCnenAllInitList("$(PREFIX)", "m1, m3")
