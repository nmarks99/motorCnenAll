# ../../bin/${EPICS_HOST_ARCH}/SPiiPlusEnableAll st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocSPiiPlusEnableAllLinux.dbd")
iocSPiiPlusEnableAllLinux_registerRecordDeviceDriver(pdbbase)

< settings.iocsh

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
