TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += SPiiPlusEnableAll

DBD += SPiiPlusEnableAllSupport.dbd

SPiiPlusEnableAllSupport_DBD += SPiiPlusEnableAll.dbd

# specify all source files to be compiled and added to the library
SPiiPlusEnableAll_SRCS += SPiiPlusEnableAll.cpp SPiiPlusEnableAllInit.cpp

enableAll_LIBS += motor asyn
SPiiPlusEnableAll_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
