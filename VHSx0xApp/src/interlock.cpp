//
//	interlock.cpp
//
//	Purpose:
//		Implementation functions for a software interlock
//		When the interlock is triggered, the emergency channel
//		off for each selected channel is triggered.
//		The interlock subroutine record also supports
//		auto-clear of the kill signals after the interlock clears,
//		and auto-on for each channel.

#include <stdio.h>

#include <registryFunction.h>
#include <epicsExport.h>
#include <subRecord.h>
#include <dbTest.h>
#include <string>

using namespace		std;


inline const char * getStringFromInput(
	const struct link	&	inputLink,
	const char			*	pNameInput	)
{
	if ( inputLink.type != CONSTANT )
	{
		fprintf( stderr, "Invalid type for %s, must be a string!\n", pNameInput );
		return "InvalidInput";
	}
	if ( inputLink.value.constantStr == NULL )
	{
		fprintf( stderr, "Invalid ptr for %s, must be a string!\n", pNameInput );
		return "InvalidInput";
	}
	return inputLink.value.constantStr;
}


inline void turnOnChannel(
	const string &	strChanName	)
{
	if ( strChanName.size() == 0 )
		return;
	string	strChanOn( strChanName );
	strChanOn += ":ChannelControlOnOff";
	dbpf( strChanOn.c_str(), "1" );
}


inline void turnOffChannel(
	const string &	strChanName	)
{
	if ( strChanName.size() == 0 )
		return;
	string	strChanOff( strChanName );
	strChanOff += ":ChannelControlEMCY";
	dbpf( strChanOff.c_str(), "1" );
	dbpf( strChanOff.c_str(), "0" );
}


extern "C" static long
InterlockInit( subRecord *psub)
{
	return 0;
}


//
//	Subroutine Inputs
//	INPA:	Name of Interlock PV, high = OK, low = Inhibit
//	INPB:	Name of VHS Device
//	INPC:	Name of VHS Channel 0, "" if not inhibited by this interlock
//	INPD:	Name of VHS Channel 1, "" if not inhibited by this interlock
//	INPE:	Name of VHS Channel 2, "" if not inhibited by this interlock
//	INPF:	Name of VHS Channel 3, "" if not inhibited by this interlock
//	INPG:	AutoEnable, Set to "1" to automatically clear kill signals
//	INPH:	AutoOn, Set to "1" to automatically turn HV ON when interlock clears
 
extern "C" static long
InterlockProcess( subRecord *psub)
{
    if( psub == NULL)
		return -1;

	//	If no change to our interlock, just return
	if ( pSub->a == pSub->la )
		return 0;

	//	Get arguments
    bool		fHvEnable	= psub->a;
	string		strDev		= getStringFromInput( psub->inpb, "INPB" );
	string		strChan0	= getStringFromInput( psub->inpc, "INPC" );
	string		strChan1	= getStringFromInput( psub->inpd, "INPD" );
	string		strChan2	= getStringFromInput( psub->inpe, "INPE" );
	string		strChan3	= getStringFromInput( psub->inpf, "INPF" );
    bool		fAutoClear	= psub->g;
    bool		fAutoOn		= psub->h;

	if ( fHvEnable )
	{
		if ( fAutoClear )
		{
			if ( strDev.size() == 0 )
			{
				fprintf( stderr, "Invalid input for INPB, must be a device name!\n" );
				return -1;
			}

			string	strClearKill	= strDev + ":ModuleControlCLEAR";
			dbpf( strClearKill.c_str(), "1" );
			dbpf( strClearKill.c_str(), "0" );
		}
		if ( fAutoOn )
		{
			turnOnChannel( strChan0	);
			turnOnChannel( strChan1	);
			turnOnChannel( strChan2	);
			turnOnChannel( strChan3	);
		}
	}
	else
	{
		turnOffChannel( strChan0	);
		turnOffChannel( strChan1	);
		turnOffChannel( strChan2	);
		turnOffChannel( strChan3	);
	}

    psub->val = fHvEnable;
    return 0;
}

extern "C" epicsRegisterFunction( InterlockInit );
extern "C" epicsRegisterFunction( InterlockProcess );
