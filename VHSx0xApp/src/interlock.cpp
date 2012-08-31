//
//	interlock.cpp
//
//	Purpose:
//		Implementation functions for a software interlock
//		When the interlock is triggered, the emergency off
//		for each selected channel is triggered.
//		The interlock subroutine record also supports
//		auto-clear of the kill signals after the interlock clears,
//		and auto-on for each channel.

#include <stdio.h>
#include "drvVHSx0xLib.h"

#include <registryFunction.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <dbFldTypes.h>
#include <aSubRecord.h>
#include <dbAddr.h>
#include <dbAccess.h>
#include <string>

using namespace		std;

int		dbgVHSx0xInterlock	= 0;

typedef struct	InterlockData
{
	long		fEnablePrior;
}	InterlockData;

inline long getEpicsPV( const string & strPvName, long & valueRet )
{
	DBADDR			addr;
	long			status		= dbNameToAddr( strPvName.c_str(), &addr );
	if ( status != 0 )
	{
		fprintf( stderr, "Invalid PV name: %s\n", strPvName.c_str() );
		return status;
	}

	status = dbGetField( &addr, DBR_LONG, &valueRet, NULL, NULL, NULL );
	if ( status != 0 )
	{
		fprintf( stderr, "Failed to read PV name: %s\n", strPvName.c_str() );
		return status;
	}
	return status;
}


inline long writeEpicsPV( const string & strPvName, long value )
{
	DBADDR			addr;
	long			status		= dbNameToAddr( strPvName.c_str(), &addr );
	if ( status != 0 )
	{
		fprintf( stderr, "Invalid PV name: %s\n", strPvName.c_str() );
		return status;
	}

	if ( dbgVHSx0xInterlock	>= 2 )
		printf( "writeEpicsPV: %s = %ld\n", strPvName.c_str(), value );

	status = dbPutField( &addr, DBR_LONG, &value, 1L );
	if ( status != 0 )
	{
		fprintf( stderr, "Failed to write to PV name: %s\n", strPvName.c_str() );
		return status;
	}
	return status;
}


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
	{
		//	Empty channel names aren't tied to this interlock
		return;
	}
	string	strChanOn( strChanName );
	strChanOn += ":ChannelControlOnOff";
	writeEpicsPV( strChanOn.c_str(), 1 );
}


inline bool isChanInEmergencyOff(
	const string &	strChanName	)
{
	if ( strChanName.size() == 0 )
	{
		//	Empty channel names aren't tied to this interlock
		//	Return false to avoid waiting for unused channel
		return false;
	}

	string	strChanEventStatus( strChanName );
	strChanEventStatus += ":ChannelEventStatus";

	long	chanEventStatus	= 0;
	long	status			= getEpicsPV( strChanEventStatus.c_str(), chanEventStatus );
	if ( status == 0 && (chanEventStatus & VHSX0X_CHNL_EVTSTS_BIT_EMERGENCY_OFF) )
	{
		return true;
	}

	return false;
}


inline void emergencyChannelOff(
	const string &	strChanName	)
{
	if ( strChanName.size() == 0 )
	{
		//	Empty channel names aren't tied to this interlock
		return;
	}

	//	If channel is off, no need to do emergency off
	string	strChanStatus( strChanName );
	strChanStatus += ":ChannelStatus";

	long	chanStatus	= 0;
	long	status		= getEpicsPV( strChanStatus.c_str(), chanStatus );
	if ( status == 0 && (chanStatus & VHSX0X_CHNL_STATUS_BIT_CHANNEL_ON) == 0 )
	{
		if ( dbgVHSx0xInterlock	>= 2 )
			printf( "emergencyChannelOff: Chan %s is already off.\n", strChanName.c_str() );
		return;
	}

	string	strChanOff( strChanName );
	strChanOff += ":ChannelControlEMCY";
	writeEpicsPV( strChanOff.c_str(), 1 );

	// Wait till channel is in emergency off
	bool	fChanInEmergencyOff	= false;
	for ( unsigned int i = 0; i < 2; ++i ) 
	{
		if ( isChanInEmergencyOff( strChanName ) )
		{
			fChanInEmergencyOff	= true;
			break;
		}

    	(void)epicsThreadSleep(0.1);
	}

	writeEpicsPV( strChanOff.c_str(), 0 );
	if ( !fChanInEmergencyOff )
	{
	//	fprintf( stderr, "ERROR: Unable to put %s in emergency OFF!\n", strChanName.c_str() );
		fprintf( stderr, "ToDo: Fix interlock status check!\n" );
	}
}


inline long aSubGetLong( epicsEnum16 type, void * ptr )
{
	long		value	= -1;
	switch ( type )
	{
	default:			break;
	case DBF_CHAR:		value = *static_cast<epicsInt8 *>(		ptr );	break;
	case DBF_UCHAR:		value = *static_cast<epicsUInt8 *>(		ptr );	break;
	case DBF_SHORT:		value = *static_cast<epicsInt16 *>(		ptr );	break;
	case DBF_USHORT:	value = *static_cast<epicsUInt16 *>(	ptr );	break;
	case DBF_LONG:		value = *static_cast<epicsInt32 *>(		ptr );	break;
	case DBF_ULONG:		value = *static_cast<epicsUInt32 *>(	ptr );	break;
	case DBF_ENUM:		value = *static_cast<epicsEnum16 *>(	ptr );	break;
	case DBF_FLOAT:		value = static_cast<long>( *static_cast<epicsFloat32 *>( ptr ) ); break;
	case DBF_DOUBLE:	value = static_cast<long>( *static_cast<epicsFloat64 *>( ptr ) ); break;
	}
	return value;
}

extern "C"
{
static long InterlockInit( aSubRecord	* pSub )
{
	switch ( pSub->fta )
	{
	default:
		fprintf( stderr, "InterlockInit %s: Invalid type for INPA!\n", pSub->name );
		return -1;
	case DBF_CHAR:
	case DBF_UCHAR:
	case DBF_SHORT:
	case DBF_USHORT:
	case DBF_LONG:
	case DBF_ULONG:
	case DBF_ENUM:
	case DBF_DOUBLE:
	case DBF_FLOAT:
		break;
	}

	if ( pSub->ftb != DBF_STRING )
	{
		fprintf( stderr, "InterlockInit %s: INPB must be type STRING!\n", pSub->name );
		return -1;
	}

	if ( pSub->ftc != DBF_STRING )
	{
		fprintf( stderr, "InterlockInit %s: INPC must be type STRING!\n", pSub->name );
		return -1;
	}

	if ( pSub->ftd != DBF_STRING )
	{
		fprintf( stderr, "InterlockInit %s: INPD must be type STRING!\n", pSub->name );
		return -1;
	}

	if ( pSub->fte != DBF_STRING )
	{
		fprintf( stderr, "InterlockInit %s: INPE must be type STRING!\n", pSub->name );
		return -1;
	}

	if ( pSub->dpvt != NULL )
	{
		fprintf( stderr, "InterlockInit %s: Device private (dpvt) non-NULL!\n", pSub->name );
		return -1;
	}

	InterlockData	*	pData	= new InterlockData;
	pData->fEnablePrior = -1;
	pSub->dpvt = static_cast<void *>( pData );

	printf( "\nInterlockInit %s: Ready for use!\n\n", pSub->name );
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
 
static long InterlockProcess( aSubRecord	*	pSub )
{
	if ( dbgVHSx0xInterlock	>= 4 )
		printf( "InterlockProcess %s: entry\n", pSub->name );

    if ( pSub == NULL )
	{
		fprintf( stderr, "InterlockProcess %s: Invalid pSub ptr!\n", pSub->name );
		return -1;
	}

	InterlockData	*	pData	= reinterpret_cast<InterlockData *>( pSub->dpvt );

	//	Get interlock and prior interlock values
    long		fHvEnable		= aSubGetLong( pSub->fta, pSub->a );

	//	If no change to our interlock, just return
	if ( fHvEnable == pData->fEnablePrior )
	{
		if ( dbgVHSx0xInterlock	>= 4 )
			printf( "InterlockProcess %s: No change\n", pSub->name );
		return 0;
	}

	//	Save prior value for next time
    pData->fEnablePrior = fHvEnable;

	//	Get arguments
	string		strDev		= static_cast<char *>( pSub->b );
	string		strChan0	= static_cast<char *>( pSub->c );
	string		strChan1	= static_cast<char *>( pSub->d );
	string		strChan2	= static_cast<char *>( pSub->e );
	string		strChan3	= static_cast<char *>( pSub->f );
    bool		fAutoClear	= static_cast<bool>( aSubGetLong( pSub->ftg, pSub->g ) );
    bool		fAutoOn		= static_cast<bool>( aSubGetLong( pSub->fth, pSub->h ) );

	if ( fHvEnable )
	{
		if ( dbgVHSx0xInterlock	>= 1 )
			printf( "InterlockProcess %s: HV Enable\n", pSub->name );
		if ( fAutoClear )
		{
			if ( strDev.size() == 0 )
			{
				fprintf( stderr, "InterlockProcess %s: Empty string for INPB, must be a device name!\n", pSub->name );
				return -1;
			}

			string	strClearKill	= strDev + ":ModuleControlCLEAR";
			writeEpicsPV( strClearKill.c_str(), 1 );

			// Wait till all channels are ready
			const char	*	pszChanStuck	= NULL;
			for ( unsigned int i = 0; i < 10; i++ )
			{
				(void)epicsThreadSleep(0.1);
				if ( isChanInEmergencyOff( strChan0 ) )
				{
					pszChanStuck	= strChan0.c_str();
					continue;
				}
				if ( isChanInEmergencyOff( strChan1 ) )
				{
					pszChanStuck	= strChan1.c_str();
					continue;
				}
				if ( isChanInEmergencyOff( strChan2 ) )
				{
					pszChanStuck	= strChan2.c_str();
					continue;
				}
				if ( isChanInEmergencyOff( strChan3 ) )
				{
					pszChanStuck	= strChan3.c_str();
					continue;
				}
				pszChanStuck	= NULL;
				break;
			}

			if ( pszChanStuck )
			{
				fprintf( stderr, "ToDo: Fix interlock status check!\n" );
			}

			//	Clear kill request
			writeEpicsPV( strClearKill.c_str(), 0 );
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
		if ( dbgVHSx0xInterlock	>= 1 )
			printf( "InterlockProcess %s: HV Inhibit!\n", pSub->name );

		//	Enable kill signals
		string	strKillEnable( strDev );
		strKillEnable += ":ModuleControlKILena";
		writeEpicsPV( strKillEnable.c_str(), 1 );

		emergencyChannelOff( strChan0	);
		emergencyChannelOff( strChan1	);
		emergencyChannelOff( strChan2	);
		emergencyChannelOff( strChan3	);
	}

    pSub->val = fHvEnable;
    return 0;
}

epicsRegisterFunction( InterlockInit );
epicsRegisterFunction( InterlockProcess );

} // extern "C"
