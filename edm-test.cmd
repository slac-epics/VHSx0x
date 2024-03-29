#! /bin/bash

# Setup edm environment
source /reg/g/pcds/setup/epicsenv-3.14.12.sh

# -m "PARENT=EXP/highVoltage.edl"		\
# -m "IOC=AMO:R14:IOC:21"				\
# -m "DEV=AMO:R14:IOC:21:VHS0"			\
# -m "HOME=AMOHome.edl"					\

edm -x -eolc							\
 -m "TITLE=High Voltage"				\
 -m "PARENT=gmd/gmdHome.edl"			\
 -m "IOC=SXR:GMD:IOC:VME"				\
 -m "DEV=SXR:GMD:SHV:VHS0"				\
 -m "HOME=SXRHome.edl"					\
 vhsScreens/vhsN.edl					\
 vhsScreens/vhs2Chan.edl				\
 &
