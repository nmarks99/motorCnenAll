TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += motorEnableAll

DBD += motorEnableAllSupport.dbd

motorEnableAllSupport_DBD += motorEnableAll.dbd

# specify all source files to be compiled and added to the library
motorEnableAll_SRCS += motorEnableAll.cpp motorEnableAllCA.cpp

enableAll_LIBS += motor asyn
motorEnableAll_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
