/*--------------------------------------------------------------------------\
|                       Sprite.c - Sprite Layer
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
|1.00  07/15/95  RATAJIK Added MPS support
|1.00  04/21/98  RATAJIK Fixed TotSprite counter (inc. at wrong place!)
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
#include "common.h"
#include "sprite.h"
#include "video.h"

/*______________________________________________________________________
|
| Global Variables
|_____________________________________________________________________*/

/*______________________________________________________________________
|
|  Function Prototypes.
|______________________________________________________________________*/
PBMPDATA       AddBMPNode         ( PBMPDATA pLastNode);
BOOL           DeleteBitmapList   ( PBMPDATA pbmpBegBMP );
BOOL           ActOnSprite        ( PACTIVE_BITMAP_LIST pabCur, PSPRITE_OBJECT psoCur);
PSPRITE_OBJECT CheckMultiCollision( PACTIVE_BITMAP_LIST pabCur, PSPRITE_OBJECT psoCur, PSPRITE_OBJECT psoTest);

BOOL CheckPoint(float fX1, float fY1, float fLX1, float fLY1, float fX2, float fY2);

/*__________________________________________________________________________
|
|    Function: StartSpriteThread
|
| Description: Kick off a sprites thread.  Create its semaphore
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
BOOL StartSpriteThread(PSPRITE_OBJECT psoCur)
{
    CHAR   szSemName[CCHMAXPATH];
    USHORT rc;
    ULONG  ulPost;

    //
    // Create the threads semaphore
    //
    sprintf (szSemName, "\\SEM32\\%8s.SEM", psoCur->pshHeader->pszSemName);

    if(!psoCur->pshHeader->semThread)
    {
        if(rc = DosCreateEventSem(szSemName, &psoCur->pshHeader->semThread, 0, TRUE))
        {

            printf("Unable to CreateSem (%s)\n", szSemName);
            return(FALSE);
        }

        printf("Create Sem: %s\n", szSemName);///@RAt
        DisplayMessage("Create Sem: %s\n", szSemName);
    }
    else
        printf("Sem Not needed (Already has one)\n"); //@RAt

    //
    // Set the semaphore.
    //
    rc = DosResetEventSem(psoCur->pshHeader->semThread, &ulPost);
    printf("After ResetEventSem in thread start\n");//@RAT


    //
    // Start the thread, passing the
    // sprites LL pointer.
    psoCur->pshHeader->pidSpriteThread = _beginthread(psoCur->pshHeader->pfnSpriteFunction,
        0,
        (unsigned)14000,
        (PVOID)psoCur);

    if(psoCur->pshHeader->pidSpriteThread > 0)
    {
        g_ulMemory += 4000;
        usThreads++;
#if DEV
        if(g_hWndStatus)
        {
            WinPostMsg(g_hWndStatus,
                DIVE_DISPLAY_THREADS,
                0L,
                (MPARAM)usThreads);

        }
#endif

        printf("Started thread for %s\n", psoCur->pshHeader->pszSpriteName);

    }
    else
    {
        printf("ERROR:  Unable to start thread!\n");

        return(FALSE);
    }
    printf("Exiting StartSpriteThread()\n");//@RAT
}


/*__________________________________________________________________________
|
|    Function: CreateSprite
|
| Description: Create the primary sprite node, and set default values
|
|      Return: Pointer to the new node, or NULL if error.
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL CreateSprite(PSPRITE_OBJECT *ppsoList,          // Start of List
                  CHAR           *szSpriteName,      // Name of Sprite
                  USHORT         usSpriteType,       // Type
                  PVOID          pfnSpriteFunction,  // Control Function
                  PSPRITE_OBJECT *psoNewSprite       // Pointer to new sprite
                  )
{
    static PSPRITE_OBJECT pLastSprite;
    static USHORT         usSemIndex = 0;
    static BOOL           bFirst = TRUE;
    static HEV            semCreateSprite = 0;

    PSPRITE_OBJECT pNewSprite;
    PSPRITE_OBJECT pCurSprite;
    CHAR           szTmp[15];
    HPS            hPS;
    ULONG          ulPost;
    USHORT         rc;

    if(bFirst)
    {
        CHAR szCreateSpriteSemName[255];
        bFirst = FALSE;

        //
        // Create the threads semaphore
        //
        strcpy(szCreateSpriteSemName, "\\SEM32\\CRSPRT.SEM");

        if(rc = DosCreateEventSem(szCreateSpriteSemName, &semCreateSprite, 0, TRUE))
        {
            printf("Unable to CreateSem (%s)\n", szCreateSpriteSemName);
            return(FALSE);
        }

        DisplayMessage("Create Sem: %s\n", szCreateSpriteSemName);
    }


    //
    // Only allow one sprite to be created
    // at one time. (Might blow the list
    // otherwise!).
    //
    printf("Waiting for CreateSprite\n");
    rc = DosWaitEventSem(semCreateSprite, SEM_INDEFINITE_WAIT);
    printf("Got CreateSprite\n");
    rc = DosResetEventSem(semCreateSprite, &ulPost);

    *psoNewSprite = NULL;

    //
    // Attempt to re-use inactive sprites
    //
    pCurSprite = *ppsoList;
    while(pCurSprite)
    {
        if(!(strcmp(pCurSprite->pshHeader->pszSpriteName, szSpriteName)) &&
            pCurSprite->pshHeader->usState == STATE_UNKNOWN)
        {
            pCurSprite->pshHeader->usState = STATE_INACTIVE;

            printf("Returning already allocated sprite node: %s\n", pCurSprite->pshHeader->pszSpriteName);

            pCurSprite->usMessage = 0;

            memset(pCurSprite->phiInfo, 0, sizeof(HIT_INFO  ));
            memset(pCurSprite->psiLoc,  0, sizeof(SPACE_INFO));
            memset(pCurSprite->pabInfo, 0, sizeof(ACTIVE_BITMAP_LIST));

            pCurSprite->phiInfo->usHitState   = HIT_STATE_NOT_HIT;
            pCurSprite->pshHeader->usState    = STATE_UNKNOWN;

            *psoNewSprite = pCurSprite;
            g_usActiveSprites++;

            DosPostEventSem(semCreateSprite);
            return(TRUE);
        }

        pCurSprite = pCurSprite->pNextNode;
    }

    //
    // Allocate the new node.
    //
    pNewSprite = (PSPRITE_OBJECT)AllocMem(sizeof(SPRITE_OBJECT));


    //
    // Clear the new memory out.
    //
    memset(pNewSprite, 0, sizeof(SPRITE_OBJECT));


    //
    // Allocate the substructures.
    //
    pNewSprite->phiInfo   = (PHIT_INFO)AllocMem            (sizeof(HIT_INFO          ));
    pNewSprite->pshHeader = (PSPRITE_HEADER)AllocMem       (sizeof(SPRITE_HEADER     ));
    pNewSprite->psiLoc    = (PSPACE_INFO)AllocMem          (sizeof(SPACE_INFO        ));

    pNewSprite->pabInfo   = (PACTIVE_BITMAP_LIST) AllocMem(sizeof(ACTIVE_BITMAP_LIST));

    memset(pNewSprite->phiInfo  , 0, sizeof(HIT_INFO           ));
    memset(pNewSprite->pshHeader, 0, sizeof(SPRITE_HEADER      ));
    memset(pNewSprite->psiLoc   , 0, sizeof(SPACE_INFO         ));
    memset(pNewSprite->pabInfo  , 0, sizeof(ACTIVE_BITMAP_LIST));

    //
    // Allocate enough room for the
    // strings.
    pNewSprite->pshHeader->pszSpriteName = (PSZ)AllocMem(strlen(szSpriteName) + 1);

    sprintf (szTmp, "TW%6u.SEM", usSemIndex);
    usSemIndex++;

    pNewSprite->pshHeader->pszSemName    = (PSZ)AllocMem(strlen(szTmp) + 1);

    //
    // Copy the strings.
    //
    strcpy(pNewSprite->pshHeader->pszSpriteName, szSpriteName);
    strcpy(pNewSprite->pshHeader->pszSemName   , szTmp);

    //
    // Set the default values.
    //
    pNewSprite->phiInfo->usHitState          = HIT_STATE_NOT_HIT;
    pNewSprite->pshHeader->usState           = STATE_UNKNOWN;
    pNewSprite->pshHeader->usSpriteType      = usSpriteType;
    pNewSprite->pshHeader->pfnSpriteFunction = pfnSpriteFunction;

    g_usActiveSprites++;

    //
    // Find the last node, and add the
    // new node ot the end of the list.
    //
    if(*ppsoList == NULL)
    {
        pCurSprite = *ppsoList = pNewSprite;
    }
    else
    {

        //
        // Find the last node in the list
        //
        //
        for(pCurSprite = *ppsoList; pCurSprite->pNextNode != NULL; pCurSprite = pCurSprite->pNextNode);

        //
        // Set pPrevNode in the current node
        // to the last node.
        //
        pNewSprite->pPrevNode = pCurSprite;

        //
        // Set pNextNode in the current node
        // to the next node.
        //
        pCurSprite->pNextNode = pNewSprite;
        pCurSprite = pNewSprite;
    }

    *psoNewSprite = pCurSprite;

    g_usTotalSprites++;

    DosPostEventSem(semCreateSprite);
    printf("Giving CreateSprite\n");

    return(FALSE);
}

/*__________________________________________________________________________
|
|    Function: AddBMPToSprite
|
| Description: Add a bitmap to a specific linked list in a sprite node.
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
PBMPDATA AddBMPToSprite(PBMPDATA *ppBMPList, CHAR *szBMPName)
{
    PBMPDATA pCurBMP;
    PBMPDATA pNewBMP;

    pNewBMP = AllocMem(sizeof(BMPDATA));

    //
    // Load the bitmap into the node.
    //
    if(LoadBitmapFile(szBMPName, pNewBMP))
    {
        DisplayMessage("Unable to load file %s.  Sprite bitmap node not created.\n", szBMPName);
        FreeMem(pNewBMP, sizeof(BMPDATA));
        return(NULL);
    }

    //
    // If we already have a list here,
    // find the end, and add to it.
    //
    if(*ppBMPList)
    {
        pCurBMP = *ppBMPList;
        while(pCurBMP->pNextNode != NULL)
        {
            pCurBMP = pCurBMP->pNextNode;
        }

        pCurBMP->pNextNode = pNewBMP;
        pCurBMP = pCurBMP->pNextNode;
    }
    else
    {
        //
        // New list.  Create the start of it.
        //
        pCurBMP = *ppBMPList = pNewBMP;
    }

    if(pCurBMP)
    {
        //
        // Load the bitmap into the node.
        //
        LoadBitmapFile(szBMPName, pNewBMP);
        pCurBMP->sIndex = 0;

        return(pCurBMP);
    }
    else
        return(NULL);

}

/*__________________________________________________________________________
|
|    Function: AddBMPNode
|
| Description: Allocate a new node.
|
|      Return: Pointer to the bitmaps data
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
PBMPDATA AddBMPNode(PBMPDATA pLastNode)
{
    PBMPDATA pNewNode;

    pNewNode = AllocMem(sizeof(BMPDATA));

    return(pNewNode);
}

/*__________________________________________________________________________
|
|    Function: ProcessSprite
|
| Description: Figure out which sprite to display, and copy it to the
|              scene buffer.
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
BOOL ProcessSprite(PSPRITE_OBJECT psoCur)
{
    PSPRITE_OBJECT psoHit;
    PBMPDATA pbmpStart;
    PBMPDATA pbmpCur = NULL;
    PBMPDATA pbmpIndex;
    USHORT   usIndex;
    PACTIVE_BITMAP_LIST pabCur;

    switch(psoCur->pshHeader->usState)
    {
    case STATE_ACTIVE:
        pabCur = psoCur->pabInfo;
        while(pabCur)
        {
            ActOnSprite(pabCur, psoCur);

            pabCur = pabCur->pNextNode;
        }

        break;

    case STATE_UNKNOWN:
        pbmpCur = NULL;
        break;
    }

    return(TRUE);
}

/*__________________________________________________________________________
|
|    Function: DetectCollision
|
| Description: Wrapper that figures out if two sprites have collided.
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
PSPRITE_OBJECT DetectCollision(PSPRITE_OBJECT psoTest)
{
    PSPRITE_OBJECT psoCur;
    PBMPDATA pbmpTest;
    PBMPDATA pbmpst;
    PBMPDATA pbmpCur;
    PACTIVE_BITMAP_LIST  pabCur;
    BOOL     bHit = FALSE;
    PSPRITE_OBJECT psoHit;

    psoCur = psoStart;
    while(psoCur)
    {
        //
        // Don't detect for ourselfs.. :)
        //
        if(psoCur != psoTest)
        {

            //
            // The sprite is active.  Detect.
            //
            if(psoCur->pshHeader->usState != STATE_UNKNOWN)
            {
                pabCur = psoCur->pabInfo;
                while(pabCur)
                {
                    if((psoHit = CheckMultiCollision(pabCur, psoCur, psoTest)))
                        bHit = TRUE;

                    pabCur = pabCur->pNextNode;
                }
            }
        }

        psoCur = psoCur->pNextNode;
    }

    return(psoHit);
}

/*__________________________________________________________________________
|
|    Function:
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
void _Optlink AutoThread( VOID *pv)
{
    PSPRITE_OBJECT   psoCur;
    USHORT           usMessage = 0;
    BOOL             bSendMessage = FALSE;
    PBMPDATA         pbmpCur;
#if 0
    //@MPS
    while(TRUE)
    {
        psoCur = psoStart;
        while(psoCur)
        {
            if(psoCur->pshHeader->usState != STATE_UNKNOWN)
            {
                pbmpCur = psoCur->pbiInfo->pbmpActiveSprite;

                if(psoCur->usMessage == MESSAGE_LOOP_DONE)
                {
                    usMessage = MESSAGE_LOOP_DONE;
                    bSendMessage = TRUE;
                }
                if(pbmpCur)
                {
                    if(psoCur->pshHeader->usControlType == CONTROL_AUTO)
                    {
                        if(psoCur->psiLoc->fCurX == psoCur->psiLoc->fDestinationX
                            ||
                            (
                            (
                            psoCur->psiLoc->fCurX < psoCur->psiLoc->fDestinationX &&
                            psoCur->psiLoc->fDestinationX - psoCur->psiLoc->fCurX <= psoCur->psiLoc->usXVelocity
                            )
                            ||
                            (
                            psoCur->psiLoc->fCurX > psoCur->psiLoc->fDestinationX &&
                            psoCur->psiLoc->fCurX - psoCur->psiLoc->fDestinationX <= psoCur->psiLoc->usXVelocity
                            )
                            )
                            )
                        {
                            //printf("X Hit.  fCurX = %f, fDestX = %f\n", psoCur->psiLoc->fCurX , psoCur->psiLoc->fDestinationX);
                            psoCur->psiLoc->bHitX = TRUE;
                            if(psoCur->psiLoc->bHitY)
                            {
                                //printf("X Move.\n");
                                psoCur->psiLoc->bHitX = FALSE;
                                psoCur->psiLoc->bHitY = FALSE;

                                bSendMessage = TRUE;
                                DisplayMessage("%s - AutoTHread -AutoMoveDone.\n", psoCur->pshHeader->pszSpriteName);
                                usMessage = MESSAGE_AUTOMOVE_DONE;

                            }
                        }
                        else
                        {
                            if(psoCur->psiLoc->fCurX < psoCur->psiLoc->fDestinationX)
                            {
                                if((psoCur->psiLoc->usXVelocity + psoCur->psiLoc->fCurX + pbmpCur->ulWidth  ) > bmpScene.ulWidth  )
                                {
                                    if(!bSendMessage)
                                    {
                                        bSendMessage = TRUE;
                                        usMessage = MESSAGE_BOUNDRY_HIT;
                                    }
                                }
                                //  else
                                {
                                    float fOldX;
                                    fOldX = psoCur->psiLoc->fCurX;

                                    if(psoCur->pbiInfo->pbmpCurActiveSprite)
                                    {
                                        if(pbmpCur->bAlwaysPlay)
                                        {
                                            psoCur->psiLoc->fCurX += psoCur->psiLoc->usXVelocity + 10;
                                        }
                                        else
                                        {
                                            psoCur->psiLoc->fCurX += psoCur->psiLoc->usXVelocity;
                                        }
                                    }
                                    // if(CheckForCollision(psoCur))
                                    //    {
                                    //    psoCur->psiLoc->fCurX = fOldX;
                                    //    }
                                }
                            }
                            else
                            {
                                if((psoCur->psiLoc->fCurX + pbmpCur->ulWidth - psoCur->psiLoc->usXVelocity ) < 1 )
                                {
                                    if(!bSendMessage)
                                    {
                                        bSendMessage = TRUE;
                                        usMessage = MESSAGE_BOUNDRY_HIT;
                                    }
                                }
                                // else
                                {
                                    float fOldX;
                                    fOldX = psoCur->psiLoc->fCurX;
                                    psoCur->psiLoc->fCurX -= psoCur->psiLoc->usXVelocity;

                                    // if(CheckForCollision(psoCur))
                                    //    {
                                    //    psoCur->psiLoc->fCurX = fOldX;
                                    //    }
                                }
                            }

                        }

                        if(psoCur->psiLoc->fCurY == psoCur->psiLoc->fDestinationY
                            ||
                            (
                            (
                            psoCur->psiLoc->fCurY < psoCur->psiLoc->fDestinationY &&
                            psoCur->psiLoc->fDestinationY - psoCur->psiLoc->fCurY <= psoCur->psiLoc->usYVelocity
                            )
                            ||
                            (
                            psoCur->psiLoc->fCurY > psoCur->psiLoc->fDestinationY &&
                            psoCur->psiLoc->fCurY - psoCur->psiLoc->fDestinationY <= psoCur->psiLoc->usYVelocity
                            )
                            )
                            )
                        {
                            //printf("Y Hit.  fCurY = %f, fDestY = %f\n", psoCur->psiLoc->fCurY , psoCur->psiLoc->fDestinationY);
                            psoCur->psiLoc->bHitY = TRUE;
                            if(psoCur->psiLoc->bHitX)
                            {
                                //printf("Y Move.\n");
                                psoCur->psiLoc->bHitX = FALSE;
                                psoCur->psiLoc->bHitY = FALSE;

                                if(!bSendMessage)
                                {
                                    bSendMessage = TRUE;
                                    usMessage = MESSAGE_AUTOMOVE_DONE;
                                }
                            }
                        }
                        else
                        {

                            if(psoCur->psiLoc->fCurY < psoCur->psiLoc->fDestinationY)
                            {
                                if ((psoCur->psiLoc->usYVelocity + psoCur->psiLoc->fCurY + pbmpCur->ulHeight ) > bmpScene.ulHeight )
                                {
                                    //printf("Y Boundry Hit 1.  fCurY = %f, fDestY = %f\n", psoCur->psiLoc->fCurY , psoCur->psiLoc->fDestinationY);
                                    if(!bSendMessage)
                                    {
                                        bSendMessage = TRUE;
                                        usMessage = MESSAGE_BOUNDRY_HIT;
                                    }
                                }
                                //   else
                                {
                                    float fOldY;
                                    fOldY = psoCur->psiLoc->fCurY;
                                    psoCur->psiLoc->fCurY += psoCur->psiLoc->usYVelocity;
                                    //   if(CheckForCollision(psoCur))
                                    //      {
                                    //      psoCur->psiLoc->fCurY = fOldY;
                                    //      }
                                }
                            }
                            else
                            {
                                if((pbmpCur->ulWidth) &&
                                    (psoCur->psiLoc->fCurY + pbmpCur->ulHeight - psoCur->psiLoc->usYVelocity ) < 1 )
                                {
                                    //printf("Y Boundry Hit 2.  fCurY = %f, fDestY = %f\n", psoCur->psiLoc->fCurY , psoCur->psiLoc->fDestinationY);

                                    if(!bSendMessage)
                                    {
                                        bSendMessage = TRUE;
                                        usMessage = MESSAGE_BOUNDRY_HIT;
                                    }
                                }
                                //  else
                                {
                                    float fOldY;
                                    fOldY = psoCur->psiLoc->fCurY;
                                    psoCur->psiLoc->fCurY -= psoCur->psiLoc->usYVelocity;
                                    //     if(CheckForCollision(psoCur))
                                    //        {
                                    //        psoCur->psiLoc->fCurY = fOldY;
                                    //        }
                                }
                            }
                        }

                    }

                    if((pbmpCur->ulWidth) &&
                        ((psoCur->psiLoc->fCurY + pbmpCur->ulHeight ) > bmpScene.ulHeight ) ||
                        ((psoCur->psiLoc->fCurX + pbmpCur->ulWidth  ) > bmpScene.ulWidth )    )
                    {
                        if(psoCur->psiLoc->fCurX  > bmpScene.ulWidth)
                            psoCur->psiLoc->fCurX += psoCur->psiLoc->usXVelocity;

                        if(psoCur->psiLoc->fCurY  > bmpScene.ulHeight)
                            psoCur->psiLoc->fCurY += psoCur->psiLoc->usYVelocity;

                        if(!bSendMessage)
                        {
                            bSendMessage = TRUE;
                            usMessage = MESSAGE_BOUNDRY_HIT;
                        }
                    }
                }

                if(psoCur->phiInfo->usHitState != HIT_STATE_NOT_HIT && psoCur->phiInfo->usHitControl == HIT_CONTROL_ON)
                {
                    bSendMessage = TRUE;
                    usMessage = MESSAGE_COLLISION;
                }


                if(bSendMessage)
                {
                    psoCur->usMessage = usMessage;
                    DosPostEventSem(psoCur->pshHeader->semThread);
                    //printf("Posted sem for %s\n", psoCur->pshHeader->pszSpriteName);
                }

            }

            bSendMessage = FALSE;
            usMessage = 0;

            psoCur = psoCur->pNextNode;
        }

        DosSleep(g_usAutoTime);
    }
#endif
    DisplayMessage("Auto Thread has ended.\n", NULL);
    usThreads--;
}

/*__________________________________________________________________________
|
|    Function:
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
BOOL StartAutoThread( VOID )
{
    PID pid;

    pid = _beginthread(AutoThread,
        0,
        (unsigned)10000,
        (PVOID)NULL);

    if(pid)
    {
        usThreads++;
        g_ulMemory += 10000;
#if DEV
        if(g_hWndStatus)
        {
            WinPostMsg(g_hWndStatus,
                DIVE_DISPLAY_THREADS,
                0L,
                (MPARAM)usThreads);

        }
#endif

        DisplayMessage("Started Auto Thread.\n", NULL);
        return(TRUE);
    }
    else
    {
        DisplayMessage("Unable to start Auto Thread.\n", NULL);
        return(FALSE);
    }
}

/*__________________________________________________________________________
|
|    Function: Check to see if the passed point lays in the given rectl.
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
BOOL CheckPoint(float fX1, float fY1, float fLX1, float fLY1,
                float fX2, float fY2)
{
    //                    CheckPoint(psoTest->psiLoc->fCurX,
    //                               psoTest->psiLoc->fCurY,
    //                               pbmpTest->ulWidth,
    //                               pbmpTest->ulHeight,
    //                               (psoCur->psiLoc->fCurX + pbmpCur->ulWidth),
    //                               psoCur->psiLoc->fCurY))

    if( (fX2 < fX1 + fLX1) && (fX2 > fX1) &&
        (fY2 < fY1 + fLY1) && (fY2 > fY1) ||
        (fX1 == fX2 && fY1 == fY2  )         )
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

BOOL DeleteSprite(PSPRITE_OBJECT psoDelete)
{
    PSPRITE_OBJECT psoCur;
    PSPRITE_OBJECT psoTmp;

    usThreads--;
    g_ulMemory -= 4000;
#if DEV
    if(g_hWndStatus)
    {
        WinPostMsg(g_hWndStatus,
            DIVE_DISPLAY_THREADS,
            0L,
            (MPARAM)usThreads);

    }
#endif


    g_usActiveSprites--;

    //Disable Multi-Sprite
    //psoDelete->pabInfo                  = NULL;
    psoDelete->pshHeader->usControlType = CONTROL_MANUAL;
    psoDelete->pshHeader->usState       = STATE_UNKNOWN;
    return(TRUE);
    if(psoStart == psoDelete)
    {
        DisplayMessage("Delete node.  List empty.\n", NULL);
        psoStart = NULL;
        return(TRUE);
    }

    psoCur = psoStart;
    while(psoCur)
    {
        if(psoCur->pNextNode == psoDelete)
        {
            psoTmp = psoDelete->pNextNode;

            psoCur->pNextNode = psoTmp;

            if(psoTmp)
                psoTmp->pPrevNode = psoCur;

            psoCur = NULL;


            //psoDelete->pabInfo = NULL;

            //DeleteBitmapList(psoDelete->pbmpAliveSprite   );
            //DeleteBitmapList(psoDelete->pbmpInactiveSprite);
            //DeleteBitmapList(psoDelete->pbmpDeadSprite    );

            DisplayMessage("Deleted sprite %s\n", psoDelete->pshHeader->pszSpriteName);
            DosCloseEventSem(psoDelete->pshHeader->semThread);
            FreeMem(psoDelete->pshHeader->pszSpriteName, strlen(psoDelete->pshHeader->pszSpriteName));
            memset(psoDelete, 0, sizeof(SPRITE_OBJECT));
            FreeMem(psoDelete, sizeof(SPRITE_OBJECT));

            return(TRUE);
        }

        psoCur = psoCur->pNextNode;
    }

    return(FALSE);
}

BOOL CheckForCollision(PSPRITE_OBJECT psoCur)
{
    PSPRITE_OBJECT psoHit;

    if(psoCur->phiInfo->usHitControl == HIT_CONTROL_ON)
    {
        if((psoHit = DetectCollision(psoCur)))
        {
            psoCur->phiInfo->psoHit = psoHit;
            psoCur->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
            psoCur->usMessage == MESSAGE_COLLISION;
        }
    }
}

/*__________________________________________________________________________
|
|    Function:
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
INT dorand( VOID )
{
    static usRand = 1;

    usRand++;
    if(usRand > 10000)
        usRand = 1;

    srand(usRand);
}

BOOL DeleteBitmapList(PBMPDATA pbmpBegBMP)
{
    PBMPDATA pCurBMP;
    PBMPDATA pDelBMP;

    pCurBMP = pbmpBegBMP;
    while(pCurBMP)
    {
        pDelBMP = pCurBMP->pNextNode;
        FreeMem(pCurBMP, sizeof(BMPDATA));
        pCurBMP = pDelBMP;
    }

    return(TRUE);
}

/*__________________________________________________________________________
|
|    Function: AutoMoveSprite
|
| Description: Handle moving the sprite twords its destination.
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|06/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL AutoMoveSprite(PSPRITE_OBJECT psoCur)
{
    BOOL bContinue = TRUE;
    PACTIVE_BITMAP_LIST pabCur;

    if(psoCur->pshHeader->usState == STATE_INACTIVE)
        return(FALSE);

    pabCur = psoCur->pabInfo;
    while(pabCur)
    {
        if(pabCur->pbiInfo == NULL)
        {
            return(bContinue);
        }

        if(pabCur->pbiInfo->pbmpActiveSprite)
        {
            if(psoCur->psiLoc->fCurX == psoCur->psiLoc->fDestinationX
                ||
                (
                (
                psoCur->psiLoc->fCurX < psoCur->psiLoc->fDestinationX &&
                psoCur->psiLoc->fDestinationX - psoCur->psiLoc->fCurX <= psoCur->psiLoc->usXVelocity
                )
                ||
                (
                psoCur->psiLoc->fCurX > psoCur->psiLoc->fDestinationX &&
                psoCur->psiLoc->fCurX - psoCur->psiLoc->fDestinationX <= psoCur->psiLoc->usXVelocity
                )
                )
                )
            {
                psoCur->psiLoc->bHitX = TRUE;
                if(psoCur->psiLoc->bHitY)
                {
                    psoCur->psiLoc->bHitX = FALSE;
                    psoCur->psiLoc->bHitY = FALSE;

                    psoCur->usMessage = MESSAGE_AUTOMOVE_DONE;

                    bContinue = FALSE;
                }
            }
            else
            {
                if(psoCur->psiLoc->fCurX < psoCur->psiLoc->fDestinationX)
                {
                    if((pabCur->pbiInfo->pbmpActiveSprite->ulWidth) &&
                        (psoCur->psiLoc->fCurX) > bmpScene.ulWidth  )
                    {
                        psoCur->usMessage = MESSAGE_BOUNDRY_HIT;
                        bContinue = FALSE;
                    }
                    else
                    {
                        psoCur->psiLoc->fCurX += psoCur->psiLoc->usXVelocity + pabCur->pbiInfo->pbmpActiveSprite->sIndex;
                    }
                }
                else
                {
                    if((pabCur->pbiInfo->pbmpActiveSprite->ulWidth) &&
                        (psoCur->psiLoc->fCurX + pabCur->pbiInfo->pbmpActiveSprite->ulWidth) < 1 )
                    {
                        psoCur->usMessage = MESSAGE_BOUNDRY_HIT;
                        bContinue = FALSE;
                    }
                    else
                    {
                        psoCur->psiLoc->fCurX -= psoCur->psiLoc->usXVelocity + pabCur->pbiInfo->pbmpActiveSprite->sIndex;
                    }
                }

            }

            if(psoCur->psiLoc->fCurY == psoCur->psiLoc->fDestinationY
                ||
                (
                (
                psoCur->psiLoc->fCurY < psoCur->psiLoc->fDestinationY &&
                psoCur->psiLoc->fDestinationY - psoCur->psiLoc->fCurY <= psoCur->psiLoc->usYVelocity
                )
                ||
                (
                psoCur->psiLoc->fCurY > psoCur->psiLoc->fDestinationY &&
                psoCur->psiLoc->fCurY - psoCur->psiLoc->fDestinationY <= psoCur->psiLoc->usYVelocity
                )
                )
                )
            {

                psoCur->psiLoc->bHitY = TRUE;
                if(psoCur->psiLoc->bHitX)
                {
                    //DisplayMessage("Y Move.\n");
                    psoCur->psiLoc->bHitX = FALSE;
                    psoCur->psiLoc->bHitY = FALSE;

                    psoCur->usMessage = MESSAGE_AUTOMOVE_DONE;

                    bContinue = FALSE;
                }
            }
            else
            {
                if(psoCur->psiLoc->fCurY < psoCur->psiLoc->fDestinationY)
                {
                    if((pabCur->pbiInfo->pbmpActiveSprite->ulWidth) &&
                        (psoCur->psiLoc->fCurY) > bmpScene.ulHeight )
                    {
                        //DisplayMessage("Y Boundry Hit 1.  fCurY = %f, fDestY = %f\n", psoCur->psiLoc->fCurY , psoCur->psiLoc->fDestinationY);
                        psoCur->usMessage = MESSAGE_BOUNDRY_HIT;
                        bContinue = FALSE;
                    }
                    else
                    {
                        psoCur->psiLoc->fCurY += psoCur->psiLoc->usYVelocity + pabCur->pbiInfo->pbmpActiveSprite->sIndex;
                    }
                }
                else
                {
                    if((pabCur->pbiInfo->pbmpActiveSprite->ulWidth) &&
                        (psoCur->psiLoc->fCurY + pabCur->pbiInfo->pbmpActiveSprite->ulHeight) < 1 )
                    {
                        psoCur->usMessage = MESSAGE_BOUNDRY_HIT;
                        bContinue = FALSE;
                    }
                    else
                    {
                        psoCur->psiLoc->fCurY -= psoCur->psiLoc->usYVelocity + pabCur->pbiInfo->pbmpActiveSprite->sIndex;
                    }
                }
            }
         }

         pabCur = pabCur->pNextNode;
    }

    return(bContinue);
}

/*__________________________________________________________________________
|
|    Function: AddActiveNode
|
| Description: Add a active sprite to the list
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|07/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
PACTIVE_BITMAP_LIST *AddActiveNode(PACTIVE_BITMAP_LIST *ppabList, PBMPDATA pbmpSprite, USHORT usBitmapID)
{
    PACTIVE_BITMAP_LIST pCurPab;
    PACTIVE_BITMAP_LIST pNewPab;
    PBITMAP_INFO pbiNew;

    pCurPab = *ppabList;
    pCurPab->pbiInfo = (PBITMAP_INFO)malloc(sizeof(BITMAP_INFO));
    memset(pCurPab->pbiInfo, 0, sizeof(BITMAP_INFO));

    pCurPab->pbiInfo->pbmpActiveSprite = pbmpSprite;
    printf("In Add ActiveNode\n");
    return(ppabList);

    pbiNew = (PBITMAP_INFO)malloc(sizeof(BITMAP_INFO));
    memset(pbiNew, 0, sizeof(BITMAP_INFO));

    pbiNew->pbmpActiveSprite = pbmpSprite;

    pCurPab = *ppabList;

    pCurPab->pbiInfo = pbiNew;

    return(ppabList);

    pNewPab = AllocMem(sizeof(ACTIVE_BITMAP_LIST));

    memset(pNewPab, 0, sizeof(ACTIVE_BITMAP_LIST));


    //
    // If we already have a list here,
    // find the end, and add to it.
    //
    if(*ppabList)
    {
        pCurPab = *ppabList;
        while(pCurPab->pNextNode != NULL)
        {
            pCurPab = pCurPab->pNextNode;
        }

        pCurPab->pNextNode = pNewPab;
        pCurPab = pCurPab->pNextNode;
    }
    else
    {

        //
        // New list.  Create the start of it.
        //
        pCurPab = *ppabList = pNewPab;
    }

    if(pCurPab)
    {
        pCurPab->pbiInfo = NULL;
        pCurPab->pbiInfo = (PBITMAP_INFO)malloc(sizeof(BITMAP_INFO));
        memset(pCurPab->pbiInfo, 0, sizeof(BITMAP_INFO));

        pCurPab->pbiInfo->pbmpActiveSprite = pbmpSprite;

        return(&pCurPab);
    }
    else
        return(NULL);

}

/*__________________________________________________________________________
|
|    Function: ActOnSprite
|
| Description: Called by the video layer to process each sprite.  This
|              includes blitting the sprite and performing hit detection
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|07/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL ActOnSprite(PACTIVE_BITMAP_LIST pabCur, PSPRITE_OBJECT psoCur)
{
    PSPRITE_OBJECT psoHit;
    PBITMAP_INFO   pbiInfo;
    PBMPDATA       pbmpCur = NULL;
    PBMPDATA       pbmpIndex;
    USHORT         usIndex;

    pbiInfo = pabCur->pbiInfo;

    if(!pbiInfo)
        return(FALSE);

    if(pbiInfo->pbmpActiveSprite)
    {

        //
        // The sprite has changed.  Set up the
        // info to reflect that change.
        //
        if(pbiInfo->pbmpActiveSprite != pbiInfo->pbmpLastActiveSprite)
        {
            pbiInfo->pbmpCurActiveSprite = pbiInfo->pbmpLastActiveSprite = pbiInfo->pbmpActiveSprite;
        }

        switch(pbiInfo->usLoopType)
        {
        case LOOP_FORWARD_ONCE:
            pbmpCur = pbiInfo->pbmpCurActiveSprite = pbiInfo->pbmpCurActiveSprite->pNextNode;
            psoCur->psiLoc->fCurX += psoCur->psiLoc->usXVelocity;
            psoCur->psiLoc->fCurY += psoCur->psiLoc->usYVelocity;
            if(pbmpCur == NULL)
            {
                psoCur->usMessage = MESSAGE_LOOP_DONE;
                pbiInfo->pbmpCurActiveSprite = NULL;
                pbiInfo->usLoopType = LOOP_STOP;
            }

            break;

        case LOOP_STOP:
            DisplayMessage("Hit LOOP_STOP.\n", NULL);
            break;

        case LOOP_FORWARD:
            pbmpCur = pbiInfo->pbmpCurActiveSprite;

            pbmpIndex = pbiInfo->pbmpCurActiveSprite;


            //
            // Point to the next sprite in the
            // list
            //
            for(usIndex = 0; usIndex < psoCur->psiLoc->usFrameRate; usIndex++)
            {

                if(pbmpIndex  == NULL)
                    pbmpIndex = psoCur->pbmpAliveSprite;

                pbmpIndex = pbmpIndex->pNextNode;
            }

            if(pbmpIndex  == NULL)
                pbmpIndex = psoCur->pbmpAliveSprite;

            pbiInfo->pbmpCurActiveSprite = pbmpIndex;
            if(pbiInfo->pbmpCurActiveSprite == NULL)
                pbiInfo->pbmpCurActiveSprite = pbiInfo->pbmpActiveSprite;

            break;
        }
    }


    //
    // Copy the sprite to the scene.
    //
    if(pbmpCur)
    {
        CopySprite(pbmpCur,
            &bmpScene,
            (LONG)psoCur->psiLoc->fCurY + pabCur->ulYOffset,
            (LONG)psoCur->psiLoc->fCurX + pabCur->ulXOffset);
    }

    if(psoCur->phiInfo->usHitControl == HIT_CONTROL_ON)
    {
        if((psoHit = DetectCollision(psoCur)))
        {
            psoCur->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
            psoCur->usMessage == MESSAGE_COLLISION;
        }
    }
}

/*__________________________________________________________________________
|
|    Function: CheckMultiCollison
|
| Description: Check if two sprites overlap (rectl)
|              //@RAT98 Ok, this is NOT the niceset function in this code <VBG>
|              I started looking at what it does and improving it, but it WORKS,
|              and I'm a bit afraid to screw with it at this point (Like,
|              gotta finish the Citibank code.. don't have time!)
|              When/if I port to Windows, I'll fix it.
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|07/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
PSPRITE_OBJECT CheckMultiCollision( PACTIVE_BITMAP_LIST pabCur, PSPRITE_OBJECT psoCur, PSPRITE_OBJECT psoTest)
{
    PACTIVE_BITMAP_LIST  pabTest;

    PBMPDATA pbmpTest;
    PBMPDATA pbmpst;
    PBMPDATA pbmpCur;

    pabTest = psoTest->pabInfo;
    while(pabTest)
    {
        if(pabTest->pbiInfo && pabCur->pbiInfo)
            if(pabTest->pbiInfo->pbmpActiveSprite && pabCur->pbiInfo->pbmpActiveSprite)
            {
                pbmpCur = pabCur->pbiInfo->pbmpActiveSprite;
                pbmpTest = pabTest->pbiInfo->pbmpActiveSprite;

                if(pbmpCur)
                {
                    if(pabTest->pbiInfo->pbmpActiveSprite && pbmpCur)
                    {
                        if((pabTest->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoTest->psiLoc->fCurX, psoTest->psiLoc->fCurY,
                            pbmpTest->ulWidth,
                            pbmpTest->ulHeight,
                            psoCur->psiLoc->fCurX,
                            psoCur->psiLoc->fCurY))
                        {
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;
                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoTest);
                        }

                        if((pabTest->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoTest->psiLoc->fCurX,
                            psoTest->psiLoc->fCurY,
                            pbmpTest->ulWidth,
                            pbmpTest->ulHeight,
                            psoCur->psiLoc->fCurX,
                            (psoCur->psiLoc->fCurY +
                            pbmpCur->ulHeight)))
                        {
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;
                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;


                            return(psoTest);
                        }


                        if((pabTest->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoTest->psiLoc->fCurX,
                            psoTest->psiLoc->fCurY,
                            pbmpTest->ulWidth,
                            pbmpTest->ulHeight,
                            (psoCur->psiLoc->fCurX + pbmpCur->ulWidth),
                            psoCur->psiLoc->fCurY))
                        {
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;
                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoTest);
                        }

                        if((pabTest->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoTest->psiLoc->fCurX,
                            psoTest->psiLoc->fCurY,
                            pbmpTest->ulWidth,
                            pbmpTest->ulHeight,
                            (psoCur->psiLoc->fCurX + pbmpCur->ulWidth),
                            (psoCur->psiLoc->fCurY +
                            pbmpCur->ulHeight)))
                        {
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;

                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoTest);
                        }

                        if((pabCur->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoCur->psiLoc->fCurX, psoCur->psiLoc->fCurY,
                            pbmpCur->ulWidth,
                            pbmpCur->ulHeight,
                            psoTest->psiLoc->fCurX,
                            psoTest->psiLoc->fCurY))
                        {
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;

                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoCur);
                        }

                        if((pabCur->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoCur->psiLoc->fCurX,
                            psoCur->psiLoc->fCurY,
                            pbmpCur->ulWidth,
                            pbmpCur->ulHeight,
                            psoTest->psiLoc->fCurX,
                            (psoTest->psiLoc->fCurY +
                            pbmpTest->ulHeight)))
                        {
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;

                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoCur);
                        }


                        if((pabCur->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoCur->psiLoc->fCurX,
                            psoCur->psiLoc->fCurY,
                            pbmpCur->ulWidth,
                            pbmpCur->ulHeight,
                            (psoTest->psiLoc->fCurX + pbmpTest->ulWidth),
                            psoTest->psiLoc->fCurY))
                        {
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;
                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoCur);
                        }

                        if((pabCur->pbiInfo->pbmpActiveSprite) &&
                            CheckPoint(psoCur->psiLoc->fCurX,
                            psoCur->psiLoc->fCurY,
                            pbmpCur->ulWidth,
                            pbmpCur->ulHeight,
                            (psoTest->psiLoc->fCurX + pbmpTest->ulWidth),
                            (psoTest->psiLoc->fCurY +
                            pbmpTest->ulHeight)))
                        {
                            psoCur->phiInfo->psoHit = psoTest;
                            psoTest->phiInfo->psoHit = psoCur;
                            psoCur->phiInfo->usHitState = HIT_STATE_HIT_OTHER;
                            psoTest->phiInfo->usHitState = HIT_STATE_HIT_BY_OTHER;
                            psoCur->usMessage = MESSAGE_COLLISION;
                            psoTest->usMessage = MESSAGE_COLLISION;

                            return(psoCur);
                        }
                    }
                }
            }

            pabTest = pabTest->pNextNode;
    }
}
