# ../../bin/${EPICS_HOST_ARCH}/acsExample st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocacsExampleLinux.dbd")
iocacsExampleLinux_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

< ./motorSim.iocsh

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date

SPiiPlusEnableAllInit("$(PREFIX)", "m1, m2, m7, m4")
