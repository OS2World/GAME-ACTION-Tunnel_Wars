/*--------------------------------------------------------------------------\
|                 Video.c - Video Layer (interface to Dive)
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
/*______________________________________________________________________
|
| System Defines
|_____________________________________________________________________*/
#define INCL_DOS
#define INCL_GPI
#define INCL_WIN
#define INCL_OS2MM
#define _MEERROR_H_
#define _MT

/*______________________________________________________________________
|
| System Includes
|_____________________________________________________________________*/
#include <os2.h>
#include <process.h>
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
#include "video.h"
#include "game.h"

/*______________________________________________________________________
|
| Global Variables
|_____________________________________________________________________*/
BOOL             bVRNDisabled = TRUE;

BOOL BlastLayer(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, ULONG ulHeight, ULONG ulIndex);
BOOL CopyLayer(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, ULONG ulHeight, ULONG ulIndex);
BOOL Test(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, ULONG ulHeight, ULONG ulIndex);

/*__________________________________________________________________________
|
|    Function: InitSprite
|
| Description: Set init info.  Kick off all of the sprite threads.
|
|      Return: T/F
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL InitSprite(HWND hWndDlg)
{
    MCI_PLAY_PARMS   mciPlayParameters;
    PSPRITE_OBJECT   psoCur = psoStart;
    ULONG            ulBufferSize;

    bVRNDisabled = TRUE;

    // Reset the sprite position
    //

    // Zoom back out to the whole scene
    //
    winData.swpZoomWindow.x = 0;
    winData.swpZoomWindow.y = 0;
    winData.swpZoomWindow.cx = bmpScene.ulWidth;
    winData.swpZoomWindow.cy = bmpScene.ulHeight;

    // Repaint the window
    //
    WinSendMsg ( hWndDlg, WM_VRNENABLED, 0L, 0L );

    WinSendMsg ( hWndDlg, WM_CHAR, MPFROM2SHORT ( VK_DOWN, 0 ), 0L );

    winData.fStart = TRUE;

    while(psoCur)
    {
        if(psoCur->pshHeader->pfnSpriteFunction)
            StartSpriteThread(psoCur);

        psoCur = psoCur->pNextNode;
    }

    return(TRUE);
}

/*__________________________________________________________________________
|
|    Function: SetupBlitter
|
| Description: Setup the area to blit to.
|
|      Return: T/F
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL SetupBlitter(HWND hWndDlg, HDIVE hDive, BOOL bVRNDisabled)
{
    SETUP_BLITTER sbSetupBlitter;    /* structure for DiveSetupBlitter       */
    HRGN      hrgn;                  /* Region handle                        */
    HPS       hps;                   /* Presentation Space handle            */
    RGNRECT   rgnCtl;                /* Processing control structure         */
    RECTL     rcl;
    USHORT    rc;

    if(bVRNDisabled)
    {
        DiveSetupBlitter ( hDive, 0 );
        bVRNDisabled = TRUE;
        return(TRUE);
    }
    else
    {
        bVRNDisabled = FALSE;

        hps = WinGetPS ( hWndDlg );
        if ( !hps )
            return(FALSE);

        hrgn = GpiCreateRegion ( hps, 0L, NULL );
        if ( hrgn )
        {
            //
            // NOTE: If mp1 is zero, then this was just a move message.
            // Illustrate the visible region on a WM_VRNENABLE.
            //
            WinQueryVisibleRegion ( hWndDlg, hrgn );
            rgnCtl.ircStart     = 0;
            rgnCtl.crc          = 50;
            rgnCtl.ulDirection  = 1;

            //
            // Get the all ORed rectangles
            //
            if ( GpiQueryRegionRects ( hps, hrgn, NULL,
                &rgnCtl, winData.rcls) )
            {
                winData.ulNumRcls = rgnCtl.crcReturned;

                //
                // Now find the window position and size, relative to parent.
                //
                WinQueryWindowPos ( hWndClient, &winData.swp );

                //
                //Convert the point to offset from desktop lower left.
                //
                winData.pointl.x = winData.swp.x;
                winData.pointl.y = winData.swp.y;

                WinMapWindowPoints ( hWndFrame, HWND_DESKTOP,
                    &winData.pointl, 1 );
            }
            else
            {
                DiveSetupBlitter ( hDive, 0 );
                GpiDestroyRegion ( hps, hrgn );
                return(FALSE);
            }

            GpiDestroyRegion( hps, hrgn );
        }
        WinReleasePS( hps );

        sbSetupBlitter.ulStructLen = sizeof ( SETUP_BLITTER );
        sbSetupBlitter.fccSrcColorFormat = FOURCC_LUT8;
        sbSetupBlitter.ulSrcWidth = winData.swpZoomWindow.cx;
        sbSetupBlitter.ulSrcHeight = winData.swpZoomWindow.cy;
        sbSetupBlitter.ulSrcPosX = winData.swpZoomWindow.x;
        sbSetupBlitter.ulSrcPosY = winData.swpZoomWindow.y;
        sbSetupBlitter.fInvert = TRUE;
        sbSetupBlitter.ulDitherType = 1;

        sbSetupBlitter.fccDstColorFormat = FOURCC_SCRN;
        sbSetupBlitter.ulDstWidth = winData.swp.cx;
        sbSetupBlitter.ulDstHeight = winData.swp.cy;
        sbSetupBlitter.lDstPosX = 0;
        sbSetupBlitter.lDstPosY = 0;
        sbSetupBlitter.lScreenPosX = winData.pointl.x;
        sbSetupBlitter.lScreenPosY = winData.pointl.y;
        sbSetupBlitter.ulNumDstRects = winData.ulNumRcls;
        sbSetupBlitter.pVisDstRects = winData.rcls;
        rc = DiveSetupBlitter ( hDive, &sbSetupBlitter );

        //
        // Blit background image to window
        //
        rc = DiveBlitImage ( hDive, bmpScene.ulImage, DIVE_BUFFER_SCREEN );
        return(TRUE);
    }
    return(FALSE);
}

/*__________________________________________________________________________
|
|    Function: This thread is responsible for layer the sprites to on top
|              of the scence, and blitting it via DIVE.
|
|              It is the lowest level this app get
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
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
VOID APIENTRY SpriteMove ( ULONG parm1 )
{
    static PSPRITE_OBJECT psoCur;
    static ULONG          ulCurrentHiIndex = 0;
    static ULONG          ulCurrentLowIndex = 0;

    static LONG           lIndexLayer1 = 0;
    static LONG           lIndexLayer2 = 0;
    static LONG           lIndexLayer3 = 0;
    static LONG           lIndexLayer4 = 0;
    static LONG           lIndexLayer5 = 0;
    static LONG           lIndexLayer6 = 0;

    static ULONG          ulCurrentRow = 0;
    PBYTE          pbSource;
    PBYTE          pbDest;
    ULONG          ulBufferSize;
    ULONG          ulScanLineBytes;
    ULONG          ulScanLines;
    //ONG          i, j, k, rc;
    USHORT         usSpeed = 1;
    ULONG          ulFramesToTime=8;
    ULONG    ulTime0, ulTime1;              /* output buffer for DosQierySysInfo */
    ULONG     ulNumFrames=0;                /* Frame counter                     */

    //DosSetPriority( PRTYS_PROCESS, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0 );

    //
    // This the thread responsible for
    // processing sprites in AUTO mode.
    //
    //StartAutoThread(); // Disabled in this version.

    //
    // Point to the start of the sprite
    // list
    //
    psoCur = psoStart;

    //
    // Get access to the scene buffer.
    //
    if ( DiveBeginImageBufferAccess ( hDive,
        bmpScene.ulImage,
        (PPBYTE)&bmpScene.pbBuffer,
        &ulScanLineBytes,
        &ulScanLines ) )

        ;
    //WinPostMsg ( hWndFrame, WM_QUIT, 0L, 0L );
    //
    // Loop until the user exits.
    //
    while ( !ulToEnd )
    {
        //if ( ( !bVRNDisabled ) && winData.fStart )

        //
        // Only process if the game has
        // started
        //
        if ( winData.fStart )
        {
            if ( !ulNumFrames++ )
                DosQuerySysInfo ( QSV_MS_COUNT, QSV_MS_COUNT, &ulTime0, 4L );

            //
            // Loop through all of the sprites.
            //
            psoCur = psoStart;
            while(psoCur)
            {

                //
                // Copy the sprite to the scene buffer
                // Do hit detection.
                ProcessSprite(psoCur);

                psoCur = psoCur->pNextNode;
            }

            //
            // Blit the scene buffer to the screen
            //
            DiveBlitImage ( hDive, bmpScene.ulImage, DIVE_BUFFER_SCREEN );

            if(g_bBlank)
            {
                memset(bmpScene.pbBuffer, 0, bmpScene.ulWidth*bmpScene.ulHeight);
            }
            else
            {

                //
                // Make sure we don't exceed the back
                // ground buffer.
                //
                lIndexLayer1 += 27 + usSpeed;
#if 0
                lIndexLayer2 += 23 + usSpeed;
                lIndexLayer3 += 20 + usSpeed;
                lIndexLayer4 += 12 + usSpeed;
                lIndexLayer5 +=  8 + usSpeed;
                lIndexLayer6 +=  2 + usSpeed;
#endif

                if(lIndexLayer1 >=  bmpLayer1.ulWidth)
                    lIndexLayer1 = 0;

#if 0
                if(lIndexLayer2 >=  bmpLayer2.ulWidth)
                    lIndexLayer2 = 0;

                if(lIndexLayer3 >=  bmpLayer3.ulWidth)
                    lIndexLayer3 = 0;

                if(lIndexLayer4 >=  bmpLayer4.ulWidth)
                    lIndexLayer4 = 0;

                if(lIndexLayer5 >=  bmpLayer5.ulWidth)
                    lIndexLayer5 = 0;

                if(lIndexLayer6 >=  bmpLayer6.ulWidth)
                    lIndexLayer6 = 0;
#endif
                //
                // Clear and update the scene buffer.
                //

                // What we really need to do here is Copy just the overlap
                // between the layers.  The rest can just be Blasted!

                // I.E:
                //Bit  |------------------------------------------------ Blast
                //1    |------------------------------------------------ Blast
                //     |------------------------------------------------ Blast
                //    ||************************************************ Copy (transparent)
                //    ||************************************************ Copy (transparent)
                //Bit | ------------------------------------------------ Blast
                //2   | ------------------------------------------------ Blast
                //    | ------------------------------------------------ Blast
                //    | ------------------------------------------------ Blast


                //if(0)
                //   Test(&bmpLayer1, &bmpScene, 0  , lIndexLayer1);

                BlastLayer(&bmpLayer1, &bmpScene, 0  , lIndexLayer1);

                // Enable if using Parallax.
#if 0
                BlastLayer(&bmpLayer2, &bmpScene, 149, lIndexLayer2);
                BlastLayer(&bmpLayer3, &bmpScene, 248, lIndexLayer3);
                BlastLayer(&bmpLayer4, &bmpScene, 322, lIndexLayer4);
                BlastLayer(&bmpLayer5, &bmpScene, 386, lIndexLayer5);
                BlastLayer(&bmpLayer6, &bmpScene, 435, lIndexLayer6);
#endif
            }

        }

        if ( ulNumFrames >= ulFramesToTime )
        {
            DosQuerySysInfo ( QSV_MS_COUNT, QSV_MS_COUNT, &ulTime1, 4L );
            ulTime1 -= ulTime0;

            g_fFrames = (float)ulFramesToTime * 1000.0 / (float)ulTime1 ;

#if 0
            {CHAR szTmp[255];
            sprintf (szTmp, "%5.4f", g_fFrames);
            printf("(*****FPS = %s\n", szTmp);
            }
#endif
            ulNumFrames = 0;

            //
            // Adjust number of frames to time based on last set.
            //
            if ( ulTime1 < 250 )
                ulFramesToTime <<= 1;
            if ( ulTime1 > 3000 )
                ulFramesToTime >>= 1;
        }

        //
        // Don't want to thrash the CPU...
        //
        DosSleep ( g_usGraphicTime );
     }

     DiveEndImageBufferAccess ( hDive, bmpScene.ulImage );
}

/*__________________________________________________________________________
|
|    Function: CopySprite
|
| Description: Copy the sprite to the scene buffer.  Don't copy black.
|              @RAT98 - Transparet color is what ever the top left-most
|                       pixel is.
|
|      Return: T/F
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|05/01/98  RATAJIK Set trans color to top left-most bit
|__________________________________________________________________________*/
BOOL CopyOpSprite(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, LONG lY, LONG lX)
{
    PBYTE       pbSource;
    PBYTE       pbDest;
    LONG       i, j, k, rc;

    //
    // Set the desination to the buffer,
    // plus X/Y of sprite coordinates
    //
    pbDest = pbmpScene->pbBuffer;
    pbDest += pbmpScene->ulWidth * (LONG)lY + (ULONG)lX;
    pbSource = pbmpSprite->pbBuffer;

    //
    // Process all of the rows
    //
    for( i = 0; i < pbmpSprite->ulHeight; i++ )
    {
        //  if(lY + i > 0 && lY + i < pbmpScene->ulHeight)
        {
            //
            // Process all of the columns
            //
            for( k = 0; k < pbmpSprite->ulWidth; k++ )
            {
                //
                // If there is somethine there,
                // copy it.
                //
                if( lX + k > 0 && lX + k < pbmpScene->ulWidth)
                {
                    //             if( *(pbSource++) )
                    *(pbDest + k) = *pbSource;
                }
                else
                {
                    pbSource++;
                }
            }
        }
#if 0
        else
        {

            for( k = 0; k < pbmpSprite->ulWidth; k++ )
            {
                pbSource++;
            }
        }
#endif

        pbDest += pbmpScene->ulWidth;
    }
}

BOOL BlastLayer(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, ULONG ulHeight, ULONG ulIndex)
{
    PBYTE       pbSource;
    PBYTE       pbDest;
    BOOL        bMore = FALSE;
    register LONG i;
    register ULONG ulSpriteHeight;
    register ULONG ulSceneWidth;

    //
    // Set the desination to the buffer,
    // plus X/Y of sprite coordinates
    //
    pbDest = pbmpScene->pbBuffer + (ulHeight * pbmpScene->ulWidth);

    pbSource = pbmpSprite->pbBuffer + ulIndex;

    ulSpriteHeight = pbmpSprite->ulHeight;
    ulSceneWidth = pbmpScene->ulWidth;

    if(ulIndex > pbmpSprite->ulWidth - ulIndex)
    {
        //
        // Process all of the rows
        //
        for( i = 0; i < ulSpriteHeight; i++ )
        {
            memcpy(pbDest, pbSource, pbmpSprite->ulWidth - ulIndex);
            pbDest += pbmpSprite->ulWidth - ulIndex;

            memcpy(pbDest, (pbmpSprite->pbBuffer + (i * pbmpSprite->ulWidth)),
                pbmpScene->ulWidth - (pbmpSprite->ulWidth - ulIndex));

            pbDest   += pbmpScene->ulWidth - (pbmpSprite->ulWidth - ulIndex);
            pbSource += pbmpSprite->ulWidth;
        }
    }
    else
    {
        //
        // Process all of the rows
        //
        for( i = 0; i < ulSpriteHeight; i++ )
        {

            //
            // Process all of the columns
            //
            memcpy(pbDest, pbSource, pbmpScene->ulWidth);
            pbDest   += pbmpScene->ulWidth;
            pbSource += pbmpSprite->ulWidth;
        }
    }
}



BOOL CopyLayer(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, ULONG ulHeight, ULONG ulIndex)
{
    PBYTE       pbSource;
    PBYTE       pbDest;
    register INT       i, j, k;
    LONG rc;

    //
    // Set the desination to the buffer,
    // plus X/Y of sprite coordinates
    //
    pbDest = pbmpScene->pbBuffer + (ulHeight * pbmpScene->ulWidth);
    //pbDest += pbmpScene->ulWidth;
    pbSource = pbmpSprite->pbBuffer + ulIndex;

    //
    // Process all of the rows
    //
    for( i = 0; i < pbmpSprite->ulHeight; i++ )
    {
        //
        // Process all of the columns
        //
        for( k = 0; k < pbmpSprite->ulWidth; k++ )
        {
            //Source++;
            if( *(pbSource++) )
                *(pbDest + k) = *pbSource;
        }

        pbDest += pbmpScene->ulWidth;
        //pbSource += pbmpSprite->ulWidth;
    }
}

BOOL Test(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, ULONG ulHeight, ULONG ulIndex)
{
#define NEXTROW 1
    ULONG CBMP[200];
    PBYTE       pbSource;
    PBYTE       pbDest;
    register INT       i, j, k;
    LONG rc;
    ULONG ulLastPos;
    ULONG ulTotBytes = 100;

    pbDest = pbmpScene->pbBuffer + (ulHeight * pbmpScene->ulWidth);

    for(j = 0; j < ulTotBytes; j++)
    {
        if(CBMP[j] == NEXTROW)
        {
            pbDest += pbmpScene->ulWidth - ulLastPos;
        }
        else
        {
            ulLastPos = CBMP[j];
            pbDest += CBMP[j];
            *pbDest = CBMP[j + 1];

            j++;
        }
    }
}

BOOL CopyFirstSprite(BMPDATA *pbmpSprite, ULONG ulY, ULONG ulX )
{
    static        HDIVE hSpriteDive = 0;
    static        ULONG ulBuf;
    SETUP_BLITTER sbSetupBlitter;     /* structure for DiveSetupBlitter       */
    ULONG rc;

    if(!hSpriteDive)
        if ( DiveOpen ( &hSpriteDive, TRUE, 0 ) )
        {

            printf("Error on open\n");

            return(FALSE);
        }
        //@RAT
        //       else
        //           DiveSetTransparentBlitMode ( hSpriteDive,
        //                                        DIVE_TBM_EXCLUDE_SOURCE_VALUE,
        //                                        0,
        //                                        0 );

        if(!pbmpSprite->ulImage)
        {

            if ( DiveAllocImageBuffer ( hSpriteDive,
                &pbmpSprite->ulImage,
                FOURCC_LUT8,
                pbmpSprite->ulWidth,
                pbmpSprite->ulHeight,
                pbmpSprite->ulWidth,
                pbmpSprite->pbBuffer ) )
            {
                printf("Error on Alloc\n");
                return(FALSE);
            }

            if(!ulBuf)
                if ( DiveAllocImageBuffer ( hSpriteDive,
                    &ulBuf,
                    FOURCC_LUT8,
                    bmpScene.ulWidth,
                    bmpScene.ulHeight,
                    bmpScene.ulWidth,
                    bmpScene.pbBuffer ) )



                {
                    printf("Error on Alloc\n");
                    return(FALSE);
                }

        }

        sbSetupBlitter.ulStructLen = sizeof ( SETUP_BLITTER );
        sbSetupBlitter.fccSrcColorFormat = FOURCC_LUT8;
        sbSetupBlitter.ulSrcWidth = pbmpSprite->ulWidth;
        sbSetupBlitter.ulSrcHeight = pbmpSprite->ulHeight;
        sbSetupBlitter.ulSrcPosX = 0;
        sbSetupBlitter.ulSrcPosY = 0;
        sbSetupBlitter.fInvert = FALSE;
        sbSetupBlitter.ulDitherType = 1;

        sbSetupBlitter.fccDstColorFormat = FOURCC_LUT8;
        sbSetupBlitter.ulDstWidth = pbmpSprite->ulWidth;
        sbSetupBlitter.ulDstHeight = pbmpSprite->ulHeight;

        sbSetupBlitter.lDstPosX = ulX;
        sbSetupBlitter.lDstPosY = ulY;
        sbSetupBlitter.lScreenPosX = winData.pointl.x;
        sbSetupBlitter.lScreenPosY = winData.pointl.y;


        //sbSetupBlitter.ulNumDstRects = DIVE_FULLY_VISIBLE;

        sbSetupBlitter.ulNumDstRects = winData.ulNumRcls;
        sbSetupBlitter.pVisDstRects = winData.rcls;
        sbSetupBlitter.pVisDstRects[0].xRight= bmpScene.ulWidth + 1500;
        sbSetupBlitter.pVisDstRects[0].yTop = bmpScene.ulHeight+ 1500;

        rc = DiveSetupBlitter( hSpriteDive, &sbSetupBlitter);

        rc = DiveBlitImage(hSpriteDive, pbmpSprite->ulImage, ulBuf);


        return(TRUE);
}

/*__________________________________________________________________________
|
|    Function: CopySprite
|
| Description: Copy the sprite to the scene buffer.  Don't copy black.
|
|      Return: T/F
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL CopySprite(BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, LONG lY, LONG lX)
{
    PBYTE       pbSource;
    PBYTE       pbDest;
    LONG       i, j, k, rc;
    BYTE       bTrans;

    //printf("\nGET: y=%lu, x= %lu\n", lY, lX);

    if(!pbmpSprite->pbBuffer)
        return(FALSE);

    //
    // Set the desination to the buffer,
    // plus X/Y of sprite coordinates
    //
    pbDest = pbmpScene->pbBuffer;
    pbDest += pbmpScene->ulWidth * (LONG)lY + (ULONG)lX;
    pbSource = pbmpSprite->pbBuffer;

    //@RAT98

    //
    // Instead of black being transparent,
    // use the top left most pixel as
    // trans.  (Thanks Donald!)
    //
    bTrans = *(pbSource  + (pbmpSprite->ulHeight * pbmpSprite->ulWidth) -1);

    //
    // Process all of the rows
    //
    for( i = 0; i < pbmpSprite->ulHeight; i++ )
    {
        if(lY + i > 0 && lY + i < pbmpScene->ulHeight)
        {
            //
            // Process all of the columns
            //
            for( k = 0; k < pbmpSprite->ulWidth; k++ )
            {
                //
                // If there is somethine there,
                // copy it.
                //
                if( lX + k > 0 && lX + k < pbmpScene->ulWidth)
                {
                    //
                    // Only blit non-transparent pixels.
                    //
                    if( *(pbSource++) != bTrans)
                        *(pbDest + k) = *pbSource;
                }
                else
                {
                    pbSource++;
                }
            }
        }
        else
        {

            for( k = 0; k < pbmpSprite->ulWidth; k++ )
            {
                pbSource++;
            }
        }

        pbDest += pbmpScene->ulWidth;
    }
}

