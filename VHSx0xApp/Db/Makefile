TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Install db templates
DB += VHSx0x_Module.template
DB += VHSx0x_Channel.template
DB += VHSx0x_Interlock.template

#----------------------------------------------------
# Install sample db substitutions
DB += VHSx0x.substitutions
DB += VHSx0x_Interlock.substitutions

#----------------------------------------------------
# Install archive templates
ARCHIVE += archive_VHSx0x_Module.template
ARCHIVE += archive_VHSx0x_Channel.template
ARCHIVE += archive_VHSx0x_Interlock.template

#----------------------------------------------------
# Install archive substitutions
ARCHIVE += archive_VHSx0x.substitutions
ARCHIVE += archive_VHSx0x_Interlock.substitutions

#----------------------------------------------------
# Install autosave templates
REQ += autosave_VHSx0x_Module.template
REQ += autosave_VHSx0x_Channel.template
REQ += autosave_VHSx0x_Interlock.template

#----------------------------------------------------
# Install autosave substitutions
REQ += autosave_VHSx0x.substitutions
REQ += autosave_VHSx0x_Interlock.substitutions

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

