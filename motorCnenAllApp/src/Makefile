TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += motorCnenAll

DBD += motorCnenAllSupport.dbd

motorCnenAllSupport_DBD += motorCnenAll.dbd

# specify all source files to be compiled and added to the library
motorCnenAll_SRCS += motorCnenAll.cpp getMotorRecords.cpp

enableAll_LIBS += motor asyn
motorCnenAll_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
