/*--------------------------------------------------------------------------\
|                       Common.h - Main include file
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
|1.00  05/09/98  Ratajik Released.
\_________________________________________________________________________*/
#ifndef DIRECTION
#define DIRECTION extern
#endif

#include "mem.h"
#include "..\barch\barch.h"

/*   Menu items definitions
*/
#define ID_MAINWND      256
#define ID_SLIDER       257
#define ID_OPTIONS      257
#define ID_EXIT         259
#define ID_BOOM         260
#define ID_LAND         261
#define ID_DEATH        262
#define ID_COLLISION    263
#define ID_START        264
#define ID_STOP         265
#define ID_ADJUST       266

#define PB_OK           107
#define PB_APPLY        108
#define PB_CANCEL       109

#define EF_PRIMARY      101
#define EF_AUTO         105
#define EF_PLAYER       106
#define DLG_ADJ         999

#define DIVE_DISPLAY_MESSAGE 9000
#define DIVE_MEMORY          9001
#define DIVE_DISPLAY_THREADS 9002

#define TW_SCORE        9999
/* New data types */
typedef  PBYTE    *PPBYTE;

/*   Bitmap data structure
*/
typedef struct _BMPDATA
{
    ULONG  ulImage;                  /* Dive buffer number                  */
    PBYTE  pbBuffer;                 /* Pointer to image buffer             */
    ULONG  ulWidth;                  /* Bitmap Width in pels                */
    ULONG  ulHeight;                 /* Bitmap Height in pels               */
    ULONG  ulLineSizeBytes;

    BOOL   bAlwaysPlay;
    SHORT  sIndex;

    struct _BMPDATA *pNextNode;
} BMPDATA, *PBMPDATA;



typedef struct _WINDATA
{
    POINTL   pointl;                 /* Lower left corner of window      */
    SWP      swp;
    RECTL    rcls[50];
    ULONG    ulNumRcls;
    BOOL     fUp;                    /* Key up indicator                 */
    BOOL     fDown;
    BOOL     fLeft;                  /* Key left indicator               */
    BOOL     fRight;                 /* Key right indicator              */

    BOOL     fLastUp;                /* Key up indicator                 */
    BOOL     fLastDown;
    BOOL     fLastLeft;              /* Key left indicator               */
    BOOL     fLastRight;             /* Key right indicator              */

    USHORT   usStopUp;               /* Key up indicator                 */
    USHORT   usStopDown;
    USHORT   usStopLeft;             /* Key left indicator               */
    USHORT   usStopRight;            /* Key right indicator              */

    BOOL     fStart;                 /* Used to start animation          */
    BOOL     fCollision;             /* Used to indicate collision       */
    BOOL     fSoundOn;
    BOOL     fSoundEnabled;          /* On when soundcard installed      */
    SWP      swpZoomWindow;          /* Viewing area of the scene        */
    USHORT   usStop;
} WINDATA, *PWINDATA;


/*
* MMPM/2 Playlist strucuture which contains the commands,
* and the operands.
*/
typedef struct pls
{
    ULONG ulCommand;
    ULONG ulOperandOne;
    ULONG ulOperandTwo;
    ULONG ulOperandThree;
} PLAY_LIST_STRUCTURE_T;

typedef struct _BITMAP_LIST       /* list of loaded bitmaps   */
{
    PSZ      pszBitmapName;
    PBMPDATA pbmpData;

    struct _BITMAP_LIST *pNextNode;
} BITMAP_LIST, *PBITMAP_LIST;



DIRECTION PLAY_LIST_STRUCTURE_T ThrustSound[8];    /* Play List memory Structure  */
DIRECTION PLAY_LIST_STRUCTURE_T BoomSound[2];

DIRECTION USHORT    usWaveDeviceId;
DIRECTION BMPDATA   bmpScene;
DIRECTION BMPDATA   bmpBackLow;
DIRECTION BMPDATA   bmpBackHigh;

DIRECTION BMPDATA   bmpLayer1;
DIRECTION BMPDATA   bmpLayer2;
DIRECTION BMPDATA   bmpLayer3;
DIRECTION BMPDATA   bmpLayer4;
DIRECTION BMPDATA   bmpLayer5;
DIRECTION BMPDATA   bmpLayer6;

DIRECTION BMPDATA   bmpSprite[12];
DIRECTION BMPDATA   bmpLander[12];
DIRECTION BMPDATA   bmpFlame[8];
DIRECTION BMPDATA   bmpBoom[6];
DIRECTION WINDATA   winData;                      /* Bitmap and widow def structure    */
DIRECTION HDIVE     hDive;                        /* Dive instance handle              */
DIRECTION PBYTE     pPalette;                     /* Pointer to bitmap palette area    */
DIRECTION ULONG     ulToEnd    ;                  /* Controls the display thread       */
DIRECTION USHORT    g_usGraphicTime;
DIRECTION USHORT    g_usPlayerTime ;
DIRECTION USHORT    g_usAutoTime   ;
DIRECTION HWND      hWndFrame;
DIRECTION HWND      hWndClient;
DIRECTION HAB       g_hAB;
DIRECTION HMQ       g_hMQ;
DIRECTION HWND      g_hWndStatus;
DIRECTION ULONG     g_ulBackSpeed;
DIRECTION CHAR      szTmp[255];
DIRECTION USHORT    usThreads;
DIRECTION SHORT     g_sScore   ;
DIRECTION USHORT    g_usActiveSprites;
DIRECTION float     g_fCurYLoc;
DIRECTION float     g_fCurXLoc;
DIRECTION USHORT    g_usPlayer;
DIRECTION HFILE     hJoystick;
DIRECTION ULONG     g_ulMemory;
DIRECTION USHORT    g_usTotalSprites;
DIRECTION float     g_fFrames;
DIRECTION BOOL      g_bBlank;
DIRECTION BOOL      g_bDiveBlitSprite;
DIRECTION BOOL      g_bUseJoy;

ULONG   OpenAudioPlaylistDevice ( PSZ Playlist );
ULONG   InitPlaylist ();
ULONG   LoadSound ( PSZ pszFileName, ULONG * pulFileSize );
BOOL    InitSprite(HWND hWndDlg);
BOOL    LoadBitmaps(VOID);
MRESULT HandlePlayerMove(HWND hWndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);

BOOL    PlayerWin(HWND hWndDlg);
BOOL    PlayerDead(HWND hWndDlg);
BOOL    InitSprite(HWND hWndDlg);
VOID    APIENTRY SpriteMove ( ULONG parm1 );
BOOL    SetupDiveBuffer( VOID );
BOOL    GetBitmap( CHAR *pszFile, PBMPDATA pbmpData );
PBITMAP_LIST AddBitmapNode(CHAR *pszFile, PBMPDATA pbmpData);
VOID DisplayMessage(CHAR *szMessage, CHAR *szParm);
BOOL CopyFirstSprite(BMPDATA *pbmpSprite, ULONG ulY, ULONG ulX );
ULONG LoadBitmapFile ( unsigned char *pszFile, PBMPDATA pbmpData );
PBARCH_INFO OpenBarch(PSZ pszName);

