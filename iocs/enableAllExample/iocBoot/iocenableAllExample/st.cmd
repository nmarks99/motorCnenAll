# ../../bin/${EPICS_HOST_ARCH}/enableAllExample st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocenableAllExampleLinux.dbd")
iocenableAllExampleLinux_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

< ./motorSim.iocsh

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
