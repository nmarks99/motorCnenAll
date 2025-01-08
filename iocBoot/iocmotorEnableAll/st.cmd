# ../../bin/${EPICS_HOST_ARCH}/motorEnableAll st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocmotorEnableAllLinux.dbd")
iocmotorEnableAllLinux_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
