/*--------------------------------------------------------------------------\
|                 Dive.c - Main program for Dive.exe/TunnelWars
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
|1.00  05/10/95  RATAJIK Initial Development
|1.00  04/20/98  Ratajik Can't NULL out in H files with new VA.  Do here
|1.00  05/09/98  Ratajik Released.
\_________________________________________________________________________*/
/*______________________________________________________________________
|
| System Defines
|_____________________________________________________________________*/
#define INCL_DOS
#define INCL_GPI
#define INCL_WIN
#define INCL_OS2MM
#define _MEERROR_H_

/*______________________________________________________________________
|
| System Includes
|_____________________________________________________________________*/
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <os2me.h>
#include <mmioos2.h>                   /* It is from MMPM toolkit           */
#include <dive.h>
#include <fourcc.h>
#include <memory.h>
#include <string.h>


/*______________________________________________________________________
|
| Application Includes
|_____________________________________________________________________*/
#define DIRECTION
#include "common.h"
#include "sprite.h"
#include "game.h"
#include "video.h"
#include "sound.h"
#include "dive.rch"

/*______________________________________________________________________
|
| Global Variables
|_____________________________________________________________________*/
PSZ       pszMyWindow = "MyWindow";     /* Window class name                 */
PSZ       pszTitleText = "Diver";       /* Title bar text                    */
FOURCC    fccFormats[100] = {0};        /* Color format code                 */
DIVE_CAPS DiveCaps = {0};


#define WM_GetVideoModeTable     0x04A2 // mp1 = address of PVOID to receive
//       the address of the mode table
//       to be allocated for the process
// mp2 = address of ULONG to receive the
//       number of mode VIDEOMODEINFO
//       entries
// Result = 1 if successful

static float fHigh = 0;
static float fLow  = 9000;


#define WinCheckAndMarkButton(hwndDlg, id, usCheckState) \
{\
    ((ULONG)WinSendDlgItemMsg(hwndDlg, id, BM_SETCHECK, \
    MPFROMSHORT(usCheckState), (MPARAM)NULL));\
    WinPostMsg(hWndDlg, WM_CONTROL, MPFROMSHORT(id), 0L);\
}

/*______________________________________________________________________
|
|  Function Prototypes.
|______________________________________________________________________*/
MRESULT EXPENTRY MyWindowProc       ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY AdjustDlgProc      ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY StatusDlgProc      ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

BOOL          GetState ( HWND hWndDlg, USHORT usControlID, BOOL *bFlag );
ULONG         ReadFile ( unsigned char *pszFile, PBMPDATA pbmpData );
VOID APIENTRY SpriteMove ( ULONG parm1 );
VOID          SetupZoomWindow ();
BOOL          CheckCaps( VOID );
VOID          UpdateScene ();
void          InitializeGameSrvr ( HWND );

/*__________________________________________________________________________
|
|    Function: main
|
| Description: main entry point
|
|      Return: n/a
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
main ( int argc, char *argv[] )
{
    TID       tidSpriteThread;
    QMSG      qmsg;                 /* Message from message queue           */
    ULONG     flCreate;             /* Window creation control flags        */
    ULONG     i, rc;                /* Index for the files to open          */
    CHAR      szReturnString[256];


    //
    // Init globals
    //
    ulToEnd = 0;
    g_sScore = 0;
    g_usActiveSprites = 0;
    g_ulMemory = 0;
    g_usTotalSprites = 0;
    g_fFrames = 0;

    winData.fStart  = FALSE;
    winData.fRight  = FALSE;
    winData.fUp     = FALSE;
    winData.fLeft   = FALSE;


    //
    // Set timer defaults
    //
    g_usGraphicTime = 3;
    g_usPlayerTime  = 25;
    g_usAutoTime    = 15;
    g_ulBackSpeed   = 1;
    usThreads       = 1;

    g_bBlank          = FALSE;
    g_bDiveBlitSprite = TRUE;
    g_bUseJoy         = FALSE;

    g_hAB = WinInitialize ( 0 );
    g_hMQ = WinCreateMsgQueue ( g_hAB, 0 );

    srand( (unsigned)time( NULL ) );

    //
    // Get the screen capabilities, and if the system support only 16 colors,
    // exit
    //
    DiveCaps.pFormatData = fccFormats;
    DiveCaps.ulFormatLength = 120;
    DiveCaps.ulStructLen = sizeof(DIVE_CAPS);

    if(!CheckCaps())
    {
        WinDestroyMsgQueue ( g_hMQ );
        WinTerminate ( g_hAB );
        return ( 1 );
    }
    //
    // Get an instance of DIVE APIs.
    //
    if ( DiveOpen ( &hDive, FALSE, 0 ) )
    {

        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
            (PSZ)"Dive is unable to run! (This version of OS/2 may be to old to run this program...)",
            (PSZ)"Diver - DiveOpen error", 0, MB_OK | MB_INFORMATION );
        WinDestroyMsgQueue ( g_hMQ );
        WinTerminate ( g_hAB );
        return ( 1 );
    }

    rc = LoadSprites();

    if (!rc)
    {
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
            (PSZ)"Error loading sprites",
            (PSZ)"Diver - LoadSprites Error", 0, MB_OK | MB_MOVEABLE );
        WinDestroyMsgQueue ( g_hMQ );
        WinTerminate ( g_hAB );
        return ( 1 );
    }

    // Allocate a DIVE buffer for the scene
    //
    bmpScene.ulImage = 0;
    if ( DiveAllocImageBuffer ( hDive,
        &bmpScene.ulImage,
        FOURCC_LUT8,
        bmpScene.ulWidth,
        bmpScene.ulHeight,
        bmpScene.ulWidth,
        bmpScene.pbBuffer ) )
    {
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
            (PSZ)"Unable to Allocate Image Buffer.",
            (PSZ)"Diver - AllocateImageBuffer Error", 0, MB_OK | MB_MOVEABLE );
        WinDestroyMsgQueue ( g_hMQ );
        WinTerminate ( g_hAB );
        return ( 1 );
    }

    winData.swpZoomWindow.x = 0;
    winData.swpZoomWindow.y = 0;
    winData.swpZoomWindow.cx = bmpScene.ulWidth;
    winData.swpZoomWindow.cy = bmpScene.ulHeight;

    StartGameThread();

    hJoystick = InitJoy();

    if(hJoystick)
        CalibrateJoystick(hJoystick);

    //
    // Register a window class, and create a standard window.
    //
    WinRegisterClass ( g_hAB, pszMyWindow, MyWindowProc, 0, sizeof(ULONG) );

    flCreate = FCF_TASKLIST | FCF_SYSMENU  | FCF_TITLEBAR | FCF_ICON |
        FCF_SIZEBORDER | FCF_MINMAX | FCF_MENU | FCF_SHELLPOSITION;

    hWndFrame = WinCreateStdWindow ( HWND_DESKTOP,
        WS_VISIBLE, &flCreate,
        pszMyWindow,
        pszTitleText,
        WS_SYNCPAINT | WS_VISIBLE,
        0, ID_MAINWND,
        &hWndClient );

    //
    // Check if we have a waveaudio device
    //
    mciSendString ( "sysinfo all name 1 wait", szReturnString, 256, 0, 0 );
    if( !strstr ( szReturnString, "Waveaud" ) )
        winData.fSoundEnabled = FALSE;
    else
    {
        winData.fSoundEnabled = TRUE;

        if ( InitPlaylist () )
        {
            WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                (PSZ)"Unable to init playlist.",
                (PSZ)"Diver Sound Error.", 0, MB_OK | MB_MOVEABLE );

            winData.fSoundEnabled = FALSE;
        }

        if( OpenAudioPlaylistDevice ( (PSZ)ThrustSound ) )
        {
            WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                (PSZ)"Unable open audio device.",
                (PSZ)"Diver Sound Error.", 0, MB_OK | MB_MOVEABLE );

            winData.fSoundEnabled = FALSE;
        }

    }

    //
    // Turn on visible region notification.
    //
    WinSetVisibleRegionNotify ( hWndClient, TRUE );

    //
    // Send an invlaidation message to the client.
    //
    WinPostMsg ( hWndFrame, WM_VRNENABLED, 0L, 0L );

    //
    // Create animation thread
    //
    if ( DosCreateThread ( &tidSpriteThread,
        (PFNTHREAD) SpriteMove,
        0L, 0L, 8192L))
    {
        WinSetVisibleRegionNotify ( hWndClient, FALSE );
        DosFreeMem ( (PVOID)bmpSprite[0].pbBuffer );
        DiveClose ( hDive );

        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
            (PSZ)"Unable to start sprite thread.",
            (PSZ)"Diver - thread error", 0, MB_OK | MB_INFORMATION );

        WinDestroyMsgQueue ( g_hMQ );
        WinTerminate ( g_hAB );
        WinDestroyWindow ( hWndFrame );
        WinDestroyMsgQueue ( g_hMQ );
        WinTerminate ( g_hAB );
        return ( 1 );
    }

    InitializeGameSrvr(hWndFrame);

    //
    // While there are still messages, dispatch them.
    //
    while ( WinGetMsg ( g_hAB, &qmsg, 0, 0, 0 ) )
        WinDispatchMsg ( g_hAB, &qmsg );


    //
    // End the sprite thread
    //
    ulToEnd = 1;
    DosWaitThread ( &tidSpriteThread, DCWW_WAIT );

    //
    // Turn off visible region notificationm tidy up, and terminate.
    //
    WinSetVisibleRegionNotify ( hWndClient, FALSE );

    //
    // Free the buffers and close DIVE
    //
    DosFreeMem ( (PVOID)bmpSprite[0].pbBuffer );
    DiveClose ( hDive );
    DosFreeMem ( pPalette );

    //
    // Process for termination
    //
    WinDestroyWindow ( hWndFrame );
    WinDestroyMsgQueue ( g_hMQ );
    WinTerminate ( g_hAB );

    if(hJoystick)
        DosClose(hJoystick);

    return ( 0 );
 }

 /*__________________________________________________________________________
 |
 |    Function: MyWindowProc
 |
 | Description:
 |
 |      Return:
 |
 | -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 |                            C H A N G E    L O G
 | -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 |
 |Date      PRG     Change
 |--------- ------- --------------------------------------------------------
 |01/19/95  RATAJIK Initial Development.
 |__________________________________________________________________________*/
 MRESULT EXPENTRY MyWindowProc ( HWND hWndDlg, ULONG msg, MPARAM mp1, MPARAM mp2 )
 {
     HWND      hwndSlider;            // Slider control window handle
     ULONG     i, rc;
     ULONG     ulSource = 0;
     ULONG     ulDest = 0;
     PBYTE     pbDest, pbSource;
     SLDCDATA  sldcData;              // Slider control structure

     static MCI_PLAY_PARMS   mciPlayParameters;

     switch( msg )
     {

     case WM_CREATE:
         mciPlayParameters.hwndCallback =  hWndDlg;
         WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP,
             (PFNWP)StatusDlgProc,
             (HMODULE)0,
             DLG_GAME_INFO,
             (PVOID)0);

         WinStartTimer( g_hAB, hWndDlg, 1, 200 );

         break;

     case WM_MOUSEMOVE:
         //
         // Turn this back on for mouse control
         //
#if 0
         {
             static SHORT CurX;
             static SHORT CurY;
             static SHORT LastX = 0;
             static SHORT LastY = 0;
             USHORT usDirKey;

             CurX = SHORT1FROMMP(mp1);
             CurY = SHORT2FROMMP(mp1);

             if(CurX > LastX)
             {
                 DisplayMessage("Move right\n", NULL);
                 usDirKey = VK_RIGHT;//right
             }
             else
             {
                 DisplayMessage("Move Left\n", NULL);
                 usDirKey = VK_LEFT; //Left
             }

             if(CurY > LastY)
             {
                 DisplayMessage("Move Up\n", NULL);
                 usDirKey = VK_UP; //Up
             }
             else
             {
                 DisplayMessage("Move Down\n", NULL);
                 usDirKey = VK_DOWN; //Down
             }
             LastX = CurX;
             LastY = CurY;

             mp2 = MPFROM2SHORT(0, usDirKey);
             //mp1 = MPFROM2SHORT(KC_PREVDOWN, 0);

             HandlePlayerMove(hWndDlg, msg, mp1, mp2);
             return WinDefWindowProc ( hWndDlg, msg, mp1, mp2 );
         }
#endif
         break;

     case WM_TIMER:
         WinSetDlgItemShort(g_hWndStatus, TX_SCORE, g_sScore, TRUE);
         WinSetDlgItemShort(g_hWndStatus, TXT_THREADS, usThreads, TRUE);
         WinSetDlgItemShort(g_hWndStatus, TXT_TOT_SPRITES, g_usTotalSprites, TRUE);
         WinSetDlgItemShort(g_hWndStatus, TXT_TOT_ACTIVE , g_usActiveSprites, TRUE);
         sprintf (szTmp, "%5.4f", g_fFrames);
         ////@printf("(*****FPS = %s\n", szTmp);
         WinSetDlgItemText(g_hWndStatus, TXT_FRAMES , szTmp);

         sprintf (szTmp, "%lu", g_ulMemory);
         WinSetDlgItemText(g_hWndStatus, TXT_MEMORY , szTmp);

         if(g_fFrames > fHigh)
         {
             fHigh = g_fFrames;
             sprintf (szTmp, "%5.4f", fHigh);

             WinSetDlgItemText(g_hWndStatus, TXT_FRAME_HIGH , szTmp);
         }

         if(g_fFrames < fLow && (g_fFrames != 0))
         {
             fLow = g_fFrames;
             sprintf (szTmp, "%5.4f", fLow);

             WinSetDlgItemText(g_hWndStatus, TXT_FRAME_LOW, szTmp);
         }

         {
             CHAR szX[200];
             CHAR szY[200];
             sprintf (szX, "%f", g_fCurXLoc);
             sprintf (szY, "%f", g_fCurYLoc);

             WinSetDlgItemText(g_hWndStatus, TXT_CUR_X  , szX);
             WinSetDlgItemText(g_hWndStatus, TXT_CUR_Y  , szY);
         }

         {
             char szTmp[256];

             sprintf(szTmp, "TunnelWars - Score: %i", g_sScore);

             WinSetWindowText(hWndFrame, szTmp);
         }
         break;


     case WM_COMMAND:
         switch ( SHORT1FROMMP ( mp1 ) )
         {

         case IDM_POWER_ONE:
             DosBeep(500,10);
             break;

         case ID_STOP:
             winData.fStart = FALSE;

             {
                 static MCI_PLAY_PARMS   mciPlayParameters;
                 ULONG rc;
                 MCI_SEEK_PARMS mseekp;

                 rc = mciSendCommand(g_usPlayer, MCI_SEEK, MCI_TO_START, (PVOID)&mseekp,0);
                 rc = mciSendCommand(g_usPlayer,
                     MCI_PLAY,
                     0,
                     (PVOID) &mciPlayParameters,
                     0);
                 //@printf("playrc=%lu\n", rc);
             }
             mciSendCommand(usWaveDeviceId,
                 MCI_PLAY,
                 MCI_NOTIFY,
                 (PVOID) &mciPlayParameters,
                 0);

             break;

         case ID_JOY:
             WinCheckAndMarkButton(hWndFrame, ID_JOY, g_bUseJoy);
             WinCheckAndMarkButton(hWndFrame, ID_KEY, g_bUseJoy);

             if(g_bUseJoy)
                 break;

             break;

         case ID_KEY:
             g_bUseJoy = FALSE;
             break;

         case ID_START:
             InitSprite(hWndDlg);

             mciSendCommand(usWaveDeviceId,
                 MCI_PLAY,
                 MCI_NOTIFY,
                 (PVOID) &mciPlayParameters,
                 0);

             break;

         case ID_ADJUST:
             WinDlgBox(HWND_DESKTOP, hWndDlg,
                 (PFNWP)AdjustDlgProc,
                 (HMODULE)0,
                 DLG_ADJ,
                 (PVOID)0);

             break;

         case ID_EXIT:
             WinPostMsg ( hWndDlg, WM_QUIT, 0L, 0L );
             break;


         default:
             //
             // Let PM handle this message.
             //
             return WinDefWindowProc ( hWndDlg, msg, mp1, mp2 );
         }
         break;

         case WM_VRNDISABLED:
             SetupBlitter(hWndDlg, hDive, TRUE);
             break;

         case WM_VRNENABLED:
             SetupBlitter(hWndDlg, hDive, FALSE);

             break;

         case WM_CHAR:
             HandlePlayerMove(hWndDlg, msg, mp1, mp2);
             break;

         case WM_REALIZEPALETTE:
             //
             // This tells DIVE that the physical palette may have changed.
             //

             DiveSetDestinationPalette ( hDive, 0, 0, 0 );

             break;

         case WM_CLOSE:

             WinPostMsg ( hWndDlg, WM_QUIT, 0L, 0L );
             break;

         default:
             //
             // Let PM handle this message.
             //
             return WinDefWindowProc ( hWndDlg, msg, mp1, mp2 );
      }

      return ( FALSE );
}

/*__________________________________________________________________________
|
|    Function: AdjustDlgProc
|
| Description:
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|01/19/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
MRESULT EXPENTRY AdjustDlgProc( HWND   hWndDlg,
                               ULONG  msg,
                               MPARAM mp1,
                               MPARAM mp2 )
{
    CHAR   szTmp[15];

    switch( msg )
    {
    case WM_INITDLG :
        WinSetDlgItemShort(hWndDlg, EF_PRIMARY, g_usGraphicTime, FALSE);
        WinSetDlgItemShort(hWndDlg, EF_AUTO   , g_usPlayerTime , FALSE);
        WinSetDlgItemShort(hWndDlg, EF_PLAYER , g_usAutoTime   , FALSE);

        ltoa(g_ulBackSpeed, szTmp, 10);
        WinSetDlgItemText(hWndDlg, EF_SCROLL_SPEED , szTmp);

        break;

    case WM_CONTROL:
        switch(SHORT1FROMMP(mp1))
        {

        }
        break;


        case WM_COMMAND :
            switch (SHORT1FROMMP(mp1))
            {
            case PB_OK:
                WinDismissDlg(hWndDlg, TRUE);
                break;

            case PB_APPLY:
                WinQueryDlgItemShort(hWndDlg, EF_PRIMARY, (PSHORT)&g_usGraphicTime, FALSE);
                WinQueryDlgItemShort(hWndDlg, EF_AUTO   , (PSHORT)&g_usPlayerTime , FALSE);
                WinQueryDlgItemShort(hWndDlg, EF_PLAYER , (PSHORT)&g_usAutoTime   , FALSE);

                WinQueryDlgItemText(hWndDlg, EF_SCROLL_SPEED , sizeof(szTmp), szTmp);

                g_ulBackSpeed = atol(szTmp);

                return(MRESULT)(0);
                break;

            case PB_CANCEL:
                WinDismissDlg(hWndDlg, TRUE);
                break;

            }

            break;

            case WM_QUIT:
                WinDismissDlg(hWndDlg, TRUE);
                break;

            default:
                return(WinDefDlgProc(hWndDlg, msg, mp1, mp2));

    }

    return(WinDefDlgProc(hWndDlg, msg, mp1, mp2));
}

BOOL CheckCaps(VOID)
{
    if ( DiveQueryCaps ( &DiveCaps, DIVE_BUFFER_SCREEN ))
    {
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
            (PSZ)"Unable to run on system (must have DIVE/WARP).",
            (PSZ)"Diver Error", 0, MB_OK | MB_INFORMATION );

        return(FALSE);
    }

    if ( DiveCaps.ulDepth < 8 )
    {
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
            (PSZ)"Unable to run on system (must have at least 256 colors.",
            (PSZ)"Diver Error", 0, MB_OK | MB_INFORMATION );

        return(FALSE);
    }

    return(TRUE);
}

/*__________________________________________________________________________
|
|    Function:
|
| Description:
|
|
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|01/19/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
MRESULT EXPENTRY StatusDlgProc( HWND   hWndDlg,
                               ULONG  msg,
                               MPARAM mp1,
                               MPARAM mp2 )
{
    switch( msg )
    {
    case DIVE_DISPLAY_MESSAGE :
        {
            PSZ pszText;
            pszText = (PSZ)mp2;

            WinSetDlgItemText(hWndDlg, 145, pszText);
        }

        break;

    case WM_INITDLG :
        g_hWndStatus = hWndDlg;

        WinCheckButton(hWndDlg, CK_DIVEBLIT, g_bDiveBlitSprite);
        WinCheckButton(hWndDlg, CK_BLANK, g_bBlank);

        break;

    case WM_CONTROL:
        switch(SHORT1FROMMP(mp1))
        {
        case CK_BLANK:
            GetState(hWndDlg, CK_BLANK  , &g_bBlank);

            break;

        case CK_DIVEBLIT:
            GetState(hWndDlg, CK_DIVEBLIT, &g_bDiveBlitSprite);

            break;

        }

        break;

        case TW_SCORE:
            {
                static MCI_PLAY_PARMS   mciPlayParameters;
                ULONG rc;
                MCI_SEEK_PARMS mseekp;
                rc = mciSendCommand(g_usPlayer, MCI_SEEK, MCI_TO_START, (PVOID)&mseekp,0);
                rc = mciSendCommand(g_usPlayer,
                    MCI_PLAY,
                    0,
                    (PVOID) &mciPlayParameters,
                    0);
            }
            break;

        case WM_COMMAND :
            switch (SHORT1FROMMP(mp1))
            {
            case PB_CLEAR:
                fHigh = 0;
                fLow  = 9000;
                break;

            case PB_OK:
                WinDismissDlg(hWndDlg, TRUE);
                break;

            case PB_APPLY:
                WinQueryDlgItemShort(hWndDlg, EF_PRIMARY, (PSHORT)&g_usGraphicTime, FALSE);
                WinQueryDlgItemShort(hWndDlg, EF_AUTO   , (PSHORT)&g_usPlayerTime , FALSE);
                WinQueryDlgItemShort(hWndDlg, EF_PLAYER , (PSHORT)&g_usAutoTime   , FALSE);
                return(MRESULT)(0);
                break;

            case PB_CANCEL:
                WinDismissDlg(hWndDlg, TRUE);
                break;

            }

            break;

            case WM_QUIT:
                WinDismissDlg(hWndDlg, TRUE);
                break;

            default:
                return(WinDefDlgProc(hWndDlg, msg, mp1, mp2));

    }

    return(WinDefDlgProc(hWndDlg, msg, mp1, mp2));
}

// This initializes the game server to intercept ALT-HOME (keypad home)
// messages to pop the game into full-screen (320x200x256) mode.
// WARNING!!!  When in full screen mode, do NOT do PM calls to figure out
//             how to do setup blitters!!!
void InitializeGameSrvr ( HWND hWndDlg )
{
    UCHAR     szErrorBuf[256];
    HMODULE   hmodGameSrvr;
    PFN       pfnInitGameFrameProc;
    ULONG     pvmi;
    ULONG     ul;
    ULONG     ulRc;

    // Try loading the gamesrvr module.
    if ( !DosLoadModule( szErrorBuf, 256, "GAMESRVR", &hmodGameSrvr ) ) {
        if ( !DosQueryProcAddr( hmodGameSrvr, 1, 0, &pfnInitGameFrameProc ) )
            ulRc = ( pfnInitGameFrameProc )( hWndFrame, 0 );
        WinSendMsg( hWndFrame, WM_GetVideoModeTable, (MPARAM)&pvmi, (MPARAM)&ul );
    }
}

/*___________________________________________________________________________
|
|    Function: GetState
|
| Description: This will load the state (TRUE/FALSE) of the passed control
|              (Used to tell if checkboxes/radio-buttons checked.)
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|ID    Date      PRG     Change
|----- --------- ------- ----------------------------------------
|N/A   04-13-93  RATAJIK Initial Developement
|___________________________________________________________________________*/
BOOL  GetState(HWND hWndDlg, USHORT usControlID, BOOL *bFlag)
{
    if(WinQueryButtonCheckstate(hWndDlg, usControlID))
        *bFlag = TRUE;
    else
        *bFlag = FALSE;

    return(*bFlag);
}
