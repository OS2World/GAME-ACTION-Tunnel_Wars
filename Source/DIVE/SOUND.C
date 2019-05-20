/*--------------------------------------------------------------------------\
|                     Sound.c - Sound Layer routines
|
|               For the latest version of the code, go to:
|                    http://www.ratajik.net/TunnelWars
|
|               For any questions or comments, send mail to:
|                       TunnelWars@ratajik.net
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|                         Dive (aka TunnelWars)
|                    Copyright (c) 1995, 98 Greg Ratajik
|                            All rights reserved.
|
|   Redistribution and use in source and binary forms, with or without
|   modification, are permitted provided that the following conditions
|   are met:
|       1. Redistributions of source code must retain the above copyright
|          notice, this list of conditions and the following disclaimer.
|       2. Redistributions in binary form must reproduce the above copyright
|          notice, this list of conditions and the following disclaimer in
|          the documentation and/or other materials provided with the 
|          distribution.
|       3. All advertising materials mentioning features or use of this 
|          software must display the following acknowledgement:
|              This product includes software developed by Greg Ratajik
|       4. The name of the author may not be used to endorse or promote 
|          products derived from this software without specific prior
|          written permission.
|
|   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
|   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
|   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
|   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
|   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
|   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
|   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
|   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
|   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
|   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Ver   Date      PRG     Change
|----- --------- ------- ----------------------------------------
|1.00  02/03/94  RATAJIK Initial Development
|1.00  05/09/98  Ratajik Released.
\_________________________________________________________________________*/
#define INCL_DOS
#define INCL_GPI
#define INCL_WIN
#define INCL_OS2MM
#define _MEERROR_H_

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <os2me.h>
#include <mmioos2.h>                   /* It is from MMPM toolkit           */
#include <dive.h>
#include <fourcc.h>
#include <memory.h>
#include <string.h>

#include "common.h"
#include "sound.h"

DIRECTION PSZ  pszThrustSound = {"thrust.wav"};
DIRECTION PSZ  pszBoomSound = {"boom.wav"};

VOID  SetSoundValues ();


/************************************************************************
*  Name          : InitPlaylist
*
*  Description   : Loads playlist structure with sound file info
*
*
*
* Note: All of the sound files for this application must be of
*       the same type ( samples/sec, bits ... ).
*
************************************************************************/

ULONG InitPlaylist ()

{
    ULONG ulSize;

    //   ThrustSound[0].ulCommand = DATA_OPERATION;
    //   ThrustSound[0].ulOperandOne = LoadSound ( pszThrustSound, &ulSize );
    //   ThrustSound[0].ulOperandTwo = ulSize;

    ThrustSound[0].ulCommand = DATA_OPERATION;
    ThrustSound[0].ulOperandOne = LoadSound ( "whhh.wav", &ulSize );
    ThrustSound[0].ulOperandTwo = ulSize;

    ThrustSound[1].ulCommand = DATA_OPERATION;
    ThrustSound[1].ulOperandOne = LoadSound ( "whhh.wav", &ulSize );
    ThrustSound[1].ulOperandTwo = ulSize;

    ThrustSound[2].ulCommand = DATA_OPERATION;
    ThrustSound[2].ulOperandOne = LoadSound ( "whhh.wav", &ulSize );
    ThrustSound[2].ulOperandTwo = ulSize;

    ThrustSound[1].ulCommand = BRANCH_OPERATION;
    ThrustSound[1].ulOperandTwo = 0;

    return ( FALSE );

}



/******************************************************************
*
* Name              :LoadSound
*
* Description       :Allocates and loads a buffer with sound data,
*                    returns a ULONG pointer to the buffer and
*                    the size of the buffer.
*
******************************************************************/


ULONG LoadSound ( PSZ pszFileName, ULONG * pulFileSize )

{

    MMAUDIOHEADER         mmHeader;
    HMMIO                 hmmioFileHandle;
    ULONG *               pulFileBuffer;
    ULONG                 ulBytesRead;
    ULONG                 rc;
    MMIOINFO mmioInfo;
    PBARCH_INFO pbiInfo;
    PSZ pszData;


    *pulFileSize = 0;
#if 0
    if( (pbiInfo = OpenBarch(pszFileName)) == NULL)
        return(FALSE);

    if(pszData = GetFileFromArchive(pbiInfo, pszFileName))
    {
        ULONG ulSize;
        ulSize = GetFileSizeFromArchive(pbiInfo, pszFileName);


        memset(&mmioInfo, '\0', sizeof(MMIOINFO));
        mmioInfo.fccIOProc = FOURCC_MEM;
        mmioInfo.pchBuffer = pszData;
        mmioInfo.cchBuffer = ulSize;

        // Open the wave file for reading
        //
        if ( ! ( hmmioFileHandle = mmioOpen( NULL,
            &mmioInfo,
            MMIO_READ) ) )
            return ( (ULONG)NULL );

    }
#endif

    if ( ! ( hmmioFileHandle = mmioOpen( pszFileName,
        (PMMIOINFO) NULL,
        MMIO_READ) ) )
        return ( (ULONG)NULL );

    // Get the Header Information for the file so that we can set the channels,
    // Samples Per Second and Bits Per Sample to play the memory playlist.
    //
    if ( mmioGetHeader(hmmioFileHandle,
        (PVOID) &mmHeader,
        sizeof(MMAUDIOHEADER),
        (PLONG) &ulBytesRead,
        (ULONG) NULL,
        (ULONG) NULL) )
    {
        mmioClose(
            hmmioFileHandle,
            0 );
        return ( (ULONG)NULL );
    }



    // Get the file size
    //
    *pulFileSize = mmHeader.mmXWAVHeader.XWAVHeaderInfo.ulAudioLengthInBytes;

    // Allocate a buffer for the wave file
    //
    if ( DosAllocMem ( (PPVOID)&pulFileBuffer, *pulFileSize,
        (ULONG) PAG_COMMIT | PAG_READ | PAG_WRITE) )
    {
        mmioClose(
            hmmioFileHandle,
            0 );
        *pulFileSize = 0;
        return ( (ULONG)NULL );
    }


    // Fill the allocated buffer with the wave file
    //
    ulBytesRead = mmioRead(hmmioFileHandle,
        (PCHAR) pulFileBuffer,
        *pulFileSize );

    if( ulBytesRead != *pulFileSize )
    {
        *pulFileSize = 0;
        DosFreeMem ( (PVOID)pulFileBuffer );
        return ( (ULONG)NULL );
    }


    mmioClose ( hmmioFileHandle, 0 );

    return ( (ULONG) pulFileBuffer );

}



/******************************************************************
*
* Name              :SetSoundValues
*
* Description       :Sets up the waudio device with predefined
*                    waveform settings.
*
*
******************************************************************/

VOID  SetSoundValues ()
{
    MCI_WAVE_SET_PARMS    lpWaveSet;


    memset ( &lpWaveSet,
        0,
        sizeof( MCI_WAVE_SET_PARMS ) );


    // Set the WaveSet Structure
    //
    lpWaveSet.ulLevel = 100;
    lpWaveSet.ulSamplesPerSec = HZ_11025;
    lpWaveSet.usBitsPerSample = BPS_8;
    lpWaveSet.usChannels = CH_1;
    lpWaveSet.ulAudio = MCI_SET_AUDIO_ALL;
    lpWaveSet.hwndCallback = (HWND) NULL;


    // Set the Channels for the MCD
    //
    mciSendCommand( usWaveDeviceId,
        MCI_SET,
        MCI_WAIT | MCI_WAVE_SET_CHANNELS,
        (PVOID) &lpWaveSet,
        (USHORT)NULL);

    // Set Samples per second
    //
    mciSendCommand(usWaveDeviceId,
        MCI_SET,
        MCI_WAIT | MCI_WAVE_SET_SAMPLESPERSEC,
        (PVOID) &lpWaveSet,
        (USHORT)NULL);

    // Set the Bits per Sample
    //
    mciSendCommand(g_usPlayer,
        MCI_SET,
        MCI_WAIT | MCI_WAVE_SET_BITSPERSAMPLE,
        (PVOID) &lpWaveSet,
        (USHORT)NULL);


    memset ( &lpWaveSet,
        0,
        sizeof( MCI_WAVE_SET_PARMS ) );


    // Set the WaveSet Structure
    //
    lpWaveSet.ulLevel = 100;
    lpWaveSet.ulSamplesPerSec = HZ_11025;
    lpWaveSet.usBitsPerSample = BPS_8;
    lpWaveSet.usChannels = CH_2;
    lpWaveSet.ulAudio = MCI_SET_AUDIO_ALL;
    lpWaveSet.hwndCallback = (HWND) NULL;


    // Set the Channels for the MCD
    //
    mciSendCommand( g_usPlayer,
        MCI_SET,
        MCI_WAIT | MCI_WAVE_SET_CHANNELS,
        (PVOID) &lpWaveSet,
        (USHORT)NULL);

    // Set Samples per second
    //
    mciSendCommand(g_usPlayer,
        MCI_SET,
        MCI_WAIT | MCI_WAVE_SET_SAMPLESPERSEC,
        (PVOID) &lpWaveSet,
        (USHORT)NULL);

    // Set the Bits per Sample
    //
    mciSendCommand(g_usPlayer,
        MCI_SET,
        MCI_WAIT | MCI_WAVE_SET_BITSPERSAMPLE,
        (PVOID) &lpWaveSet,
        (USHORT)NULL);

}



/************************************************************************
* Name        : OpenAudioPlaylistDevice
*
* Description : This procedure will open the device for Playlist.
*
*
* Concepts    : How to Open a Device to play a wave file from Playlist.
*
* MMPM/2 API's:  mciSendCommand         MCI_OPEN
*
* Parameters  : None.
*
* Return      : None.
*
*************************************************************************/

ULONG  OpenAudioPlaylistDevice  ( PSZ Playlist )
{

    MCI_OPEN_PARMS          lpWaveOpen;        /* Used for MCI_OPEN messages */
    ULONG                   ulReturn;          /* Used for ret code for API  */


                                               /*
                                               * Initialize the MCI_OPEN_PARMS data structure with hwndMainDialogBox
                                               * as callback handle for MM_MCIPASSDEVICE, then issue the MCI_OPEN
                                               * command with the mciSendCommand function.  No alias is used.
    */

    lpWaveOpen.hwndCallback   =  hWndFrame;
    lpWaveOpen.pszAlias       = (PSZ) NULL;


    /*
    * Open the correct waveform device with the MCI_OPEN message to MCI.
    * Use 0 instead of 1 because this will then use the default audio device.
    */
    lpWaveOpen.pszDeviceType  =
        (PSZ) (MAKEULONG(MCI_DEVTYPE_WAVEFORM_AUDIO,0));

        /*
        * The address of the buffer which will record and play the wave.
    */
    lpWaveOpen.pszElementName = Playlist;


    /*
    * Open the waveform file in the playlist mode.
    */
    if ( mciSendCommand(0,
        MCI_OPEN,
        MCI_WAIT | MCI_OPEN_PLAYLIST | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
        (PVOID) &lpWaveOpen,
        (USHORT) NULL) ) return ( TRUE );


    /* Save the Device Id in the global variable to be used later */
    usWaveDeviceId = lpWaveOpen.usDeviceID;

    lpWaveOpen.pszElementName = (PSZ)"haha.wav";
    lpWaveOpen.pszDeviceType  = NULL;
    lpWaveOpen.hwndCallback   =  0;

    if ( ulReturn = mciSendCommand(0,
        MCI_OPEN,
        MCI_WAIT | MCI_OPEN_ELEMENT | MCI_OPEN_SHAREABLE,
        (PVOID) &lpWaveOpen,
        (USHORT) NULL) ) return ( TRUE );

    g_usPlayer = lpWaveOpen.usDeviceID;

    SetSoundValues();
    return ( FALSE );
}
