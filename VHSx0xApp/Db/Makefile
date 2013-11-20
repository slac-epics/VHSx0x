TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Install db templates
DB += VHSx0x_Interlock.template
DB += VHSx0x_2ChInterlock.template
DB += VHSx0x_Module.template
DB += VHSx0x_2ChModule.template
DB += VHSx0x_Channel.template

#----------------------------------------------------
# Install sample db substitutions
# Use these to override settings on a per channel basis
# All macro expansions are available
DB += VHSx0x_Interlock.substitutions
DB += VHSx0x_2ChInterlock.substitutions
DB += VHSx0x_Detail.substitutions
DB += VHSx0x_Simple.substitutions
DB += VHSx0x_2ChModule.substitutions

#----------------------------------------------------
# Install sample expanded db files
# Use these when all channels on each module are the same
# The only unexpanded macros are:
#	IOC
#	CARD
#	V_MAX
#	POLARITY
DB += VHSx0x_Interlock.db
DB += VHSx0x_2ChInterlock.db
DB += VHSx0x_Detail.db
DB += VHSx0x_Simple.db
DB += VHSx0x_2ChModule.db

#----------------------------------------------------
# Install archive templates
ARCHIVE += archive_VHSx0x_Interlock.tpl-arch
ARCHIVE += archive_VHSx0x_2ChInterlock.tpl-arch
ARCHIVE += archive_VHSx0x_Module.tpl-arch
ARCHIVE += archive_VHSx0x_Channel.tpl-arch

#----------------------------------------------------
# Install archive substitutions
ARCHIVE += archive_VHSx0x_Interlock.sub-arch
ARCHIVE += archive_VHSx0x_Detail.sub-arch
ARCHIVE += archive_VHSx0x_Simple.sub-arch
ARCHIVE += archive_VHSx0x_2ChModule.sub-arch

#----------------------------------------------------
# Install sample expanded archive files
# Use these when all channels on each module are the same
# The only unexpanded macros are:
#	MODULE
#	CARD
ARCHIVE += archive_VHSx0x_Interlock.archive
ARCHIVE += archive_VHSx0x_2ChInterlock.archive
ARCHIVE += archive_VHSx0x_Detail.archive
ARCHIVE += archive_VHSx0x_Simple.archive
ARCHIVE += archive_VHSx0x_2ChModule.archive

#----------------------------------------------------
# Install autosave tpl-reqs
REQ += autosave_VHSx0x_Interlock.tpl-req
REQ += autosave_VHSx0x_2ChInterlock.tpl-req
REQ += autosave_VHSx0x_Module.tpl-req
REQ += autosave_VHSx0x_Channel.tpl-req

#----------------------------------------------------
# Install autosave substitutions
REQ += autosave_VHSx0x_Interlock.sub-req
REQ += autosave_VHSx0x_Detail.sub-req
REQ += autosave_VHSx0x_Simple.sub-req
REQ += autosave_VHSx0x_2ChModule.sub-req

#----------------------------------------------------
# Install sample expanded autosave files
# Use these when all channels on each module are the same
# The only unexpanded macros are:
#	MODULE
#	CARD
REQ += autosave_VHSx0x_Interlock.req
REQ += autosave_VHSx0x_2ChInterlock.req
REQ += autosave_VHSx0x_Detail.req
REQ += autosave_VHSx0x_Simple.req
REQ += autosave_VHSx0x_2ChModule.req

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

