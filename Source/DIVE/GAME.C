/*--------------------------------------------------------------------------\
|                      Game.c - Game Control Layer.
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
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSMEMMGR

/*______________________________________________________________________
|
| Application Includes
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

#include "joyos2.h"

/*______________________________________________________________________
|
| Application Includes
|_____________________________________________________________________*/
#include "common.h"
#include "sprite.h"
#include "game.h"

#include "dive.rch"

/*______________________________________________________________________
|
| Global Variables
|_____________________________________________________________________*/
#define STOP_INDEX 3
#define BASE_SPEED 10
#define MAX_BITMAPS 12

#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))

USHORT usSpeed = BASE_SPEED;
BOOL   bFire   = FALSE;
USHORT usWeaponType;

/*______________________________________________________________________
|
|  Function Prototypes.
|______________________________________________________________________*/
VOID PlayerSprite             ( PSPRITE_OBJECT psoCur );
VOID ComputerSprite           ( PSPRITE_OBJECT psoCur );
VOID BulletSprite             ( PSPRITE_OBJECT psoCur );
VOID ComBulletSprite          ( PSPRITE_OBJECT psoCur );
VOID ComHeatMissleSprite      ( PSPRITE_OBJECT psoCur);
VOID SmartMissle              ( PSPRITE_OBJECT psoCur);
void _Optlink GameThread      ( VOID *);
VOID CleanupThread            ( VOID );
BOOL CreateWalker             ( VOID );
BOOL CreateComBulletSprite    ( PSPRITE_OBJECT psoCur);
BOOL CreateComHeatMissleSprite( PSPRITE_OBJECT psoCur);
BOOL CreateSmartMissle        ( PSPRITE_OBJECT psoCur);
BOOL GetJoyStickInfo          ( HFILE hGame );
BOOL AutoMoveSprite2          ( PSPRITE_OBJECT psoCur );
BOOL CreatePlayerSprite       ( float fStartX, float fStartY );

/*__________________________________________________________________________
|
|    Function: LoadSprites
|
| Description: This function will load the scene, pre-laod all sprites,
|              and create the player sprite.
|
|      Return: BOOL
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL LoadSprites(VOID)
{
    PSPRITE_OBJECT psoCur;
    BMPDATA        bmpLoad;
    INT j;


    //
    // Load the scene in.
    //
    //
    LoadBitmapFile ( "title.BMP",  &bmpScene );

    LoadBitmapFile ( "back.BMP", &bmpLayer1);
    //
    // We can use mutliple layers if we're doing parallax.
    //
#if 0
    LoadBitmapFile ( "layer2.BMP", &bmpLayer2);
    LoadBitmapFile ( "layer3.BMP", &bmpLayer3);
    LoadBitmapFile ( "layer4.BMP", &bmpLayer4);
    LoadBitmapFile ( "layer5.BMP", &bmpLayer5);
    LoadBitmapFile ( "layer6.BMP", &bmpLayer6);
#endif


    //
    // Even though we don't need these
    // right away, load them now, so we
    // don't have to wait while they are
    // created.
    //
    LoadBitmapFile ( "MECH1.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH2.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH3.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH4.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH5.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH6.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH7.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH8.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH9.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH10.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH12.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH13.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH14.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH15.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH16.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH17.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH18.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH19.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH20.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH21.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH22.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH23.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH24.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH25.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH26.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH27.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH28.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH29.BMP", &bmpLoad);
    LoadBitmapFile ( "MECH30.BMP", &bmpLoad);

    LoadBitmapFile ( "BULL1.BMP", &bmpLoad);
    LoadBitmapFile ( "BULL2.BMP", &bmpLoad);
    LoadBitmapFile ( "BULL3.BMP", &bmpLoad);
    LoadBitmapFile ( "BULL4.BMP", &bmpLoad);
    LoadBitmapFile ( "BULL5.BMP", &bmpLoad);
    LoadBitmapFile ( "BULL6.BMP", &bmpLoad);

    LoadBitmapFile ( "BULLD1.BMP", &bmpLoad);
    LoadBitmapFile ( "BULLD2.BMP", &bmpLoad);
    LoadBitmapFile ( "BULLD3.BMP", &bmpLoad);


    LoadBitmapFile ( "BYE1.BMP", &bmpLoad);
    LoadBitmapFile ( "BYE2.BMP", &bmpLoad);
    LoadBitmapFile ( "BYE3.BMP", &bmpLoad);
    LoadBitmapFile ( "BYE4.BMP", &bmpLoad);
    LoadBitmapFile ( "BYE5.BMP", &bmpLoad);
    LoadBitmapFile ( "BYE6.BMP", &bmpLoad);
    LoadBitmapFile ( "BYE7.BMP", &bmpLoad);

    LoadBitmapFile("SPIKE1.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE2.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE3.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE4.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE5.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE6.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE7.BMP", &bmpLoad);
    LoadBitmapFile("SPIKE8.BMP", &bmpLoad);
    LoadBitmapFile("SPIKED1.BMP", &bmpLoad);
    LoadBitmapFile("SPIKED2.BMP", &bmpLoad);
    LoadBitmapFile("SPIKED3.BMP", &bmpLoad);
    LoadBitmapFile("SPIKED4.BMP", &bmpLoad);
    LoadBitmapFile("SPIKED5.BMP", &bmpLoad);
    LoadBitmapFile("SPIKED6.BMP", &bmpLoad);

    LoadBitmapFile("WAVE1.BMP", &bmpLoad);
    LoadBitmapFile("WAVE2.BMP", &bmpLoad);
    LoadBitmapFile("WAVE3.BMP", &bmpLoad);
    LoadBitmapFile("WAVE4.BMP", &bmpLoad);
    LoadBitmapFile("WAVE5.BMP", &bmpLoad);
    LoadBitmapFile("WAVE6.BMP", &bmpLoad);
    LoadBitmapFile("WAVE7.BMP", &bmpLoad);
    LoadBitmapFile("WAVE8.BMP", &bmpLoad);
    LoadBitmapFile("WAVE9.BMP", &bmpLoad);
    LoadBitmapFile("WAVE10.BMP", &bmpLoad);
    LoadBitmapFile("WAVE11.BMP", &bmpLoad);
    LoadBitmapFile("WAVE12.BMP", &bmpLoad);
    LoadBitmapFile("WAVE13.BMP", &bmpLoad);

    LoadBitmapFile("LIGHT1.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT2.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT3.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT4.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT5.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT6.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT7.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT8.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT1d.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT2d.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT3d.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT4d.BMP", &bmpLoad);
    LoadBitmapFile("LIGHT5d.BMP", &bmpLoad);

    psoStart = NULL;

    CreatePlayerSprite(100, 150);

    return(TRUE);
}

/*__________________________________________________________________________
|
|    Function: HandlePlayerMove
|
| Description: This routine will process player keyboard intput.
|
|      Return: MRESULT
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|05/15/95  RATAJIK Initial Development.
|06/18/95  RATAJIK Added support for WCS Mark I
|__________________________________________________________________________*/
MRESULT  HandlePlayerMove(HWND hWndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PSPRITE_OBJECT psoCur;
    USHORT         fsKeyFlags;
    SHORT          x;
    static  PSPRITE_OBJECT psoPlayer = NULL;

    fsKeyFlags = (USHORT) SHORT1FROMMP ( mp1 );

    if( (CHARMSG(&msg)->fs & KC_CHAR) )
    {
        x = CHARMSG(&msg)->chr;

        // Thrustmaster WCS Mark I
        // Three way switch:
        //        Down  l
        //        Up    f
        //
        //    Right side, back button  b
        //    Right side, Top forward  m
        //    Right side, Top back     d
        //    Front, top               a
        //    Front, Back              u
        //    Front, left              s
        //
        //    Throttle (back to front) 123456789*

        switch ( x )
        {
        case '1':
            usSpeed = BASE_SPEED + 1;
            break;

        case '2':
            usSpeed = BASE_SPEED + 2;
            break;

        case '3':
            usSpeed = BASE_SPEED + 3;
            break;

        case '4':
            usSpeed = BASE_SPEED + 4;
            break;

        case '5':
            usSpeed = BASE_SPEED + 5;
            break;

        case '6':
            usSpeed = BASE_SPEED + 6;
            break;

        case '7':
            usSpeed = BASE_SPEED + 7;
            break;

        case '8':
            usSpeed = BASE_SPEED + 8;
            break;

        case '9':
            usSpeed = BASE_SPEED + 9;
            break;

        case '*':
            usSpeed = BASE_SPEED + 15;
            break;

        case 'a':
        case 'A':
            usWeaponType = WEAPON_PLASMA;
            WinSetDlgItemText(g_hWndStatus, TXT_WEAPON_TYPE, "Plasma Cannon");
            break;

        case 's':
        case 'S':
            usWeaponType = WEAPON_DUMB_MISSLE;
            WinSetDlgItemText(g_hWndStatus, TXT_WEAPON_TYPE, "Dumb Missle");
            break;

        case 'd':
        case 'D':
            usWeaponType = WEAPON_SMART_MISSLE;
            WinSetDlgItemText(g_hWndStatus, TXT_WEAPON_TYPE, "Smart Missle");
            break;

        case 'f':
        case 'F':
            usWeaponType = WEAPON_RAPTOR_MISSLE;
            WinSetDlgItemText(g_hWndStatus, TXT_WEAPON_TYPE, "Raptor Missle");
            break;

        case 'g':
        case 'G':
            usWeaponType = WEAPON_NOVA_BOMB;
            WinSetDlgItemText(g_hWndStatus, TXT_WEAPON_TYPE, "Nova Bombsle");
            break;

        case 'h':
        case 'H':
            usWeaponType = WEAPON_CARPET_BOMB;
            WinSetDlgItemText(g_hWndStatus, TXT_WEAPON_TYPE, "Plasma Carpet Bomb");
            break;

        default:
            x = CHARMSG(&msg)->chr;
            break;
        }
    }

    switch ( SHORT2FROMMP ( mp2 ) )
    {
    case VK_UP:
        if ( fsKeyFlags & KC_KEYUP )
        {
            winData.fUp   = FALSE;
            winData.usStopUp = STOP_INDEX;
        }
        else
        {
            winData.fUp   = TRUE;
            winData.fDown = FALSE;

        }
        break;

    case VK_DOWN:
        if ( fsKeyFlags & KC_KEYUP )
        {
            winData.fDown = FALSE;
            winData.usStopDown = STOP_INDEX;
        }
        else
        {
            winData.fDown = TRUE;
            winData.fUp   = FALSE;
        }
        break;

    case VK_LEFT:
        if ( fsKeyFlags & KC_KEYUP )
        {
            winData.fLeft = FALSE;
            winData.usStopLeft = STOP_INDEX;
        }
        else
        {
            winData.fLeft  = TRUE;
            winData.fRight = FALSE;
        }

        break;

    case VK_RIGHT:
        if ( fsKeyFlags & KC_KEYUP )
        {

            winData.fRight = FALSE;
            winData.usStopRight = STOP_INDEX;
        }
        else
        {
            winData.fRight = TRUE;
            winData.fLeft  = FALSE;
        }

        break;


    case VK_SPACE:
        if ( fsKeyFlags & KC_KEYUP )
        {
            bFire = FALSE;
        }
        else
        {
            bFire = TRUE;
        }
        break;
    }

    if(psoPlayer == NULL)
    {


        //
        // Figure out which sprite is the
        // players.  Let it know the new
        // info.
        //
        psoCur = psoStart;
        while(psoCur)
        {
            if(psoCur->pshHeader->usSpriteType == TYPE_HUMAN_FOCUS)
            {
                psoPlayer = psoCur;
                psoCur = NULL;
            }
            else
                psoCur = psoCur->pNextNode;
        }
    }
    else
    {

        psoPlayer->usWeaponType = usWeaponType;
        psoPlayer->psiLoc->usXVelocity = usSpeed;
        psoPlayer->psiLoc->usYVelocity = usSpeed;
    }

    WinSetDlgItemShort(g_hWndStatus, TXT_SPEED, usSpeed - BASE_SPEED, TRUE);
    return WinDefWindowProc ( hWndDlg, msg, mp1, mp2 );
}

/*__________________________________________________________________________
|
|    Function: PlayerSprite
|
| Description: Thread that handles the Player's Sprite.  This thread
|              handles keyboard/joystick I/O, movement, acting on
|              collision/boundry hits, and missle firing for the player
|              sprite.
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
VOID PlayerSprite(PSPRITE_OBJECT psoCur)
{
    PACTIVE_BITMAP_LIST *ppabRight;
    PACTIVE_BITMAP_LIST  pabLeft;
    PACTIVE_BITMAP_LIST  pabRight;

    BOOL bStop;
    HAB hAB;
    HMQ hMQ;
    BOOL bDying = FALSE;

    hAB = WinInitialize ( 0 );
    hMQ = WinCreateMsgQueue ( hAB, 0 );


    //
    // Set the current sprite, loop
    // forward on the sprite, and
    // indicate we are in manual mode
    //
    psoCur->pshHeader->usControlType  = CONTROL_MANUAL;

    pabLeft  = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 1);

    // Support of different view of the player
    // for different direction (disabled for this game)
    //

    //pabLeft = *ppabLeft;
    //ppabRight = AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 2);
    //pabRight = *ppabRight;
    //ppabLeft  = &pabLeft ;
    //ppabRight = &pabRight;
    //pabLeft  = *ppabLeft;
    //pabRight = *ppabRight;

    pabLeft->ulXOffset = 0;
    pabLeft->ulYOffset = 0;

    //pabRight->ulXOffset = pabLeft->pbiInfo->pbmpActiveSprite->ulWidth;
    //pabRight->ulYOffset = 0;

    pabLeft->pbiInfo->usLoopType         = LOOP_FORWARD;
    //pabRight->pbiInfo->usLoopType        = LOOP_FORWARD;

    //
    // Defaul the weapon type
    //
    usWeaponType = WEAPON_DUMB_MISSLE;


    //
    // Set the frame jump rate and horiz.
    // velocity.
    //
    psoCur->psiLoc->usFrameRate = 1;
    psoCur->psiLoc->usXVelocity = 3;


    //
    // Tells sprite engine and graphcs
    // engine this sprite is ready to go!
    //
    psoCur->pshHeader->usState           = STATE_ACTIVE;

    while(TRUE)
    {
        //
        // If the sprite was hit, handle it.
        //
        if(psoCur->usMessage == MESSAGE_COLLISION)
        {

            //
            // We only want to die if the Player
            // was shot by the computer.
            if(psoCur->phiInfo->psoHit->pshHeader->usSpriteType == TYPE_COMPUTER_MISSLE)
            {
                psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
                psoCur->psiLoc->usXVelocity = 0;


                sprintf (szTmp, "sprite %s collided with sprite %s\n", psoCur->pshHeader->pszSpriteName,
                    psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);
                g_sScore -= 10;
                WinPostMsg(g_hWndStatus, TW_SCORE, 0L, 0L);

                pabLeft = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 3);

                pabLeft->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;

                DisplayMessage(szTmp, NULL);
                bDying = TRUE;
            }

            psoCur->phiInfo->usHitState = HIT_STATE_NOT_HIT;
            psoCur->usMessage = 0;
            psoCur->phiInfo->psoHit = NULL;
        }

        if(psoCur->usMessage == MESSAGE_LOOP_DONE)
        {
            DisplayMessage("%s - got message - Loop Done.\n", psoCur->pshHeader->pszSpriteName);
            psoCur->psiLoc->fCurX = 250;
            psoCur->psiLoc->fCurY = 300;
            pabLeft  = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 1);
            pabLeft->pbiInfo->usLoopType         = LOOP_FORWARD;
            psoCur->pshHeader->usControlType  = CONTROL_MANUAL;
            psoCur->phiInfo->usHitControl = HIT_CONTROL_ON;
            psoCur->psiLoc->usXVelocity = 3;

            bDying = FALSE;
        }
        if(!bDying)
        {
            if(pabLeft)
            {
                //pabLeft  = *ppabLeft;

                //
                // Handle starting/stopping moving
                // left
                //
                if( winData.fLeft)
                {
                    if( (psoCur->psiLoc->fCurX - psoCur->psiLoc->usXVelocity ) > 1 )
                        psoCur->psiLoc->fCurX -= psoCur->psiLoc->usXVelocity;
                }


                //
                // Handle starting/stopping moving
                // right

                if( winData.fRight)
                {
                    if( ( (ULONG)psoCur->psiLoc->fCurX + pabLeft->pbiInfo->pbmpActiveSprite->ulWidth + psoCur->psiLoc->usXVelocity ) < bmpScene.ulWidth )
                        psoCur->psiLoc->fCurX += psoCur->psiLoc->usXVelocity;
                }


                //
                // Handle starting/stopping moving
                // up
                if(   winData.fUp )
                {
                    if ( (psoCur->psiLoc->fCurY + pabLeft->pbiInfo->pbmpActiveSprite->ulHeight + psoCur->psiLoc->usYVelocity ) < bmpScene.ulHeight)
                        psoCur->psiLoc->fCurY += psoCur->psiLoc->usYVelocity;
                }


                //
                // Handle starting/stopping moving
                // down
                //
                if( winData.fDown)
                {

                    if ( (psoCur->psiLoc->fCurY -psoCur->psiLoc->usYVelocity ) > 1)
                        psoCur->psiLoc->fCurY -= psoCur->psiLoc->usYVelocity;
                }
            }


            //
            // Fire a missle.
            //
            if(bFire)
            {
                static INT     j  = 0;
                PSPRITE_OBJECT psoNew;
                CHAR           szName[CCHMAXPATH];

                bFire = FALSE;

                if(winData.fRight || winData.fLeft || winData.fUp || winData.fDown)
                {

                    switch(usWeaponType)
                    {
                    case WEAPON_RAPTOR_MISSLE:
                        if(!CreateSprite(&psoStart, "RaptorMissle", TYPE_HUMAN_NONFOCUS, (PVOID)BulletSprite, &psoNew))
                        {
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE1.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE7.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE8.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE7.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "SPIKE1.BMP");

                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "SPIKEd1.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "SPIKEd2.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "SPIKEd3.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "SPIKEd4.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "SPIKEd5.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "SPIKEd6.BMP");

                        }

                        psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
                        psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

                        dorand();

                        psoNew->psiLoc->fDestinationX = GetRandom(1, (bmpScene.ulWidth));
                        psoNew->psiLoc->fDestinationY = GetRandom(1, (bmpScene.ulHeight));

                        psoNew->psiLoc->usXVelocity = GetRandom(2, 45);
                        psoNew->psiLoc->usYVelocity = GetRandom(2, 45);

                        psoNew->psiLoc->usXVelocity += psoCur->psiLoc->usXVelocity;
                        psoNew->psiLoc->usYVelocity += psoCur->psiLoc->usYVelocity;

                        if(winData.fRight)
                        {
                            psoNew->psiLoc->fDestinationX = bmpScene.ulWidth + 10;

                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX + psoCur->pbmpAliveSprite->ulWidth - psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fLeft)
                        {
                            psoNew->psiLoc->fDestinationX = -100;
                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX - psoCur->pbmpAliveSprite->ulWidth + psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fUp)
                        {
                            psoNew->psiLoc->fDestinationY = bmpScene.ulHeight + 10;

                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY + psoCur->pbmpAliveSprite->ulHeight - psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }

                        if(winData.fDown)
                        {
                            psoNew->psiLoc->fDestinationY = -100;
                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY - psoCur->pbmpAliveSprite->ulHeight + psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }


                        break;


                    case WEAPON_NOVA_BOMB:
                        if(!CreateSprite(&psoStart, "WaveMissle", TYPE_HUMAN_NONFOCUS, (PVOID)BulletSprite, &psoNew))
                        {
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE1.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE7.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE8.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE9.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE10.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE11.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE12.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE13.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE12.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE11.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE10.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE9.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE8.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE7.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "WAVE1.BMP");
                        }

                        psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
                        psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

                        dorand();

                        psoNew->psiLoc->fDestinationX = GetRandom(1, (bmpScene.ulWidth));
                        psoNew->psiLoc->fDestinationY = GetRandom(1, (bmpScene.ulHeight));

                        psoNew->psiLoc->usXVelocity = GetRandom(2, 45);
                        psoNew->psiLoc->usYVelocity = GetRandom(2, 45);

                        psoNew->psiLoc->usXVelocity += psoCur->psiLoc->usXVelocity;
                        psoNew->psiLoc->usYVelocity += psoCur->psiLoc->usYVelocity;

                        if(winData.fRight)
                        {
                            psoNew->psiLoc->fDestinationX = bmpScene.ulWidth + 10;

                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX + psoCur->pbmpAliveSprite->ulWidth - psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fLeft)
                        {
                            psoNew->psiLoc->fDestinationX = -100;
                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX - psoCur->pbmpAliveSprite->ulWidth + psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fUp)
                        {
                            psoNew->psiLoc->fDestinationY = bmpScene.ulHeight + 10;

                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY + psoCur->pbmpAliveSprite->ulHeight - psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }

                        if(winData.fDown)
                        {
                            psoNew->psiLoc->fDestinationY = -100;
                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY - psoCur->pbmpAliveSprite->ulHeight + psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }


                        break;

                    case WEAPON_PLASMA:
                        if(!CreateSprite(&psoStart, "PlasmaMissle", TYPE_HUMAN_NONFOCUS, (PVOID)BulletSprite, &psoNew))
                        {
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT1.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT7.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT8.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT7.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT1.BMP");

                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD1.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD2.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD3.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD4.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD5.BMP");
                        }

                        psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
                        psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

                        dorand();

                        psoNew->psiLoc->fDestinationX = GetRandom(1, (bmpScene.ulWidth));
                        psoNew->psiLoc->fDestinationY = GetRandom(1, (bmpScene.ulHeight));

                        psoNew->psiLoc->usXVelocity = GetRandom(2, 45);
                        psoNew->psiLoc->usYVelocity = GetRandom(2, 45);

                        psoNew->psiLoc->usXVelocity += psoCur->psiLoc->usXVelocity;
                        psoNew->psiLoc->usYVelocity += psoCur->psiLoc->usYVelocity;

                        if(winData.fRight)
                        {
                            psoNew->psiLoc->fDestinationX = bmpScene.ulWidth + 10;

                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX + psoCur->pbmpAliveSprite->ulWidth - psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fLeft)
                        {
                            psoNew->psiLoc->fDestinationX = -100;
                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX - psoCur->pbmpAliveSprite->ulWidth + psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fUp)
                        {
                            psoNew->psiLoc->fDestinationY = bmpScene.ulHeight + 10;

                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY + psoCur->pbmpAliveSprite->ulHeight - psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }

                        if(winData.fDown)
                        {
                            psoNew->psiLoc->fDestinationY = -100;
                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY - psoCur->pbmpAliveSprite->ulHeight + psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }


                        break;

                    default:
                        if(!CreateSprite(&psoStart, "DumbMissle", TYPE_HUMAN_NONFOCUS, (PVOID)BulletSprite, &psoNew))
                        {
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL1.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL6.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL5.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL4.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL3.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL2.BMP");
                            AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL1.BMP");

                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "BULLD1.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "BULLD2.BMP");
                            AddBMPToSprite(&psoNew->pbmpDeadSprite, "BULLD3.BMP");

                        }

                        psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
                        psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

                        dorand();

                        psoNew->psiLoc->fDestinationX = GetRandom(1, (bmpScene.ulWidth));
                        psoNew->psiLoc->fDestinationY = GetRandom(1, (bmpScene.ulHeight));

                        psoNew->psiLoc->usXVelocity = GetRandom(2, 45);
                        psoNew->psiLoc->usYVelocity = GetRandom(2, 45);

                        psoNew->psiLoc->usXVelocity += psoCur->psiLoc->usXVelocity;
                        psoNew->psiLoc->usYVelocity += psoCur->psiLoc->usYVelocity;

                        if(winData.fRight)
                        {
                            psoNew->psiLoc->fDestinationX = bmpScene.ulWidth + 10;

                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX + psoCur->pbmpAliveSprite->ulWidth - psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fLeft)
                        {
                            psoNew->psiLoc->fDestinationX = -100;
                            psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX - psoCur->pbmpAliveSprite->ulWidth + psoNew->psiLoc->usXVelocity;

                            if(!winData.fUp && !winData.fDown)
                                psoNew->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;
                        }

                        if(winData.fUp)
                        {
                            psoNew->psiLoc->fDestinationY = bmpScene.ulHeight + 10;

                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY + psoCur->pbmpAliveSprite->ulHeight - psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }

                        if(winData.fDown)
                        {
                            psoNew->psiLoc->fDestinationY = -100;
                            psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY - psoCur->pbmpAliveSprite->ulHeight + psoCur->psiLoc->usYVelocity;
                            if(!winData.fLeft && !winData.fRight)
                                psoNew->psiLoc->fDestinationX = psoCur->psiLoc->fCurX;
                        }


                        break;

                }
                StartSpriteThread(psoNew);
            }
        }
        }// End of if(!bDying)

        g_fCurYLoc = psoCur->psiLoc->fCurY;
        g_fCurXLoc = psoCur->psiLoc->fCurX;

        psoCur->usMessage = 0;
        DosSleep(g_usPlayerTime);
    }

    DisplayMessage("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);
    DeleteSprite(psoCur);
    _endthread();

}

/*__________________________________________________________________________
|
|    Function:  ComputerSprite
|
| Description:  Provide basic controls for a Computer Sprite.
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
VOID ComputerSprite(PSPRITE_OBJECT psoCur)
{
    BOOL bHitX = FALSE;
    BOOL bHitY = FALSE;
    INT  amount;
    USHORT rc;
    ULONG  ulPost;
    BOOL  bContinue = TRUE;
    PACTIVE_BITMAP_LIST pabSprite;
    INT j = 0;

    printf("In ComputerSprite()\n");


    //
    // Set up the init information.
    //
    psoCur->psiLoc->usXVelocity = 5;
    psoCur->psiLoc->usYVelocity = 0;
    psoCur->psiLoc->usFrameRate = 3;

    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 3);

    psoCur->psiLoc->fDestinationX = bmpScene.ulWidth + pabSprite->pbiInfo->pbmpActiveSprite->ulWidth;
    psoCur->psiLoc->fDestinationY = psoCur->psiLoc->fCurY;

    psoCur->pshHeader->usControlType = CONTROL_MANUAL;
    pabSprite->pbiInfo->usLoopType   = LOOP_FORWARD;
    psoCur->phiInfo->usHitControl    = HIT_CONTROL_ON;


    //
    // Tells sprite engine and graphcs
    // engine this sprite is ready to go!
    //
    psoCur->pshHeader->usState = STATE_ACTIVE;

    while(bContinue)
    {
        bContinue = AutoMoveSprite(psoCur);

        switch(psoCur->usMessage)
        {
        case MESSAGE_BOUNDRY_HIT:
            printf("Message: BoundryHit\n");//@RAT

            bContinue = FALSE;
            break;


            //
            // Ignore destination hits on this
            // type of missle (want to go out of
            //
        case MESSAGE_AUTOMOVE_DONE:
            printf("Message: Automove Done\n");//@RAT

            bContinue = TRUE;
            break;


            //
            // Handle a collision.
            //
        case MESSAGE_COLLISION:
            printf("Message: Collision\n");//@RAT

            if(psoCur->phiInfo->psoHit)
            {
                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_OTHER)
                {
                    sprintf (szTmp, "sprite %s collided with sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_BY_OTHER)
                {
                    sprintf (szTmp, "sprite %s was hit by sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }


                //
                // We only want to allow hits by a
                // player object.
                //
                if(psoCur->phiInfo->psoHit->pshHeader->usSpriteType     != TYPE_COMPUTER
                    && psoCur->phiInfo->psoHit->pshHeader->usSpriteType != TYPE_COMPUTER_MISSLE
                    && psoCur->phiInfo->psoHit->pshHeader->usSpriteType != TYPE_HUMAN_FOCUS
                    && psoCur->phiInfo->usHitControl                    != HIT_CONTROL_OFF) //@RAT98
                {
                    psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
                    psoCur->phiInfo->psoHit    = NULL;
                    psoCur->usMessage = 0;

                    psoCur->psiLoc->usXVelocity = 10; //@RAT
                    psoCur->psiLoc->usYVelocity = 0;
                    psoCur->psiLoc->usFrameRate = 1;

                    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 3);

                    pabSprite->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;

                    g_sScore += 10;
                }
            }
            break;


            //
            // We specified to only go through
            // the loop once.
            //
        case MESSAGE_LOOP_DONE:
            printf("%s - got message - LoopDone.\n", psoCur->pshHeader->pszSpriteName);
            DisplayMessage("%s - got message - LoopDone.\n", psoCur->pshHeader->pszSpriteName);

            psoCur->pshHeader->usControlType = CONTROL_MANUAL;

            bContinue = FALSE;
            break;
        }


        if(bContinue)
        {
            if(psoCur->phiInfo->usHitControl != HIT_CONTROL_OFF)
            {
                dorand();


                //
                // Figure out if we want to fire a
                // dumb missle.
                //
                if(GetRandom(1, 100) == 2)
                {
                    //CreateComBulletSprite(psoCur);
                }


                dorand();

                //
                // Figure out if we want to fire a
                // heat-seeker.
                //
                if(GetRandom(1, 100) == 2)
                {
                    CreateComHeatMissleSprite(psoCur);
                }

                dorand();

                //
                // Figure out if we want to fire a
                // Smart Missle. (VERY Mean!)
                //
                if(GetRandom(1, 500) == 25)
                {
                    CreateSmartMissle(psoCur);
                }

            }

            DosSleep(g_usPlayerTime);
        }
    }

    printf("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);

    DisplayMessage("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);
    DeleteSprite(psoCur);
    _endthread();
}


/*__________________________________________________________________________
|
|    Function: Player missle sprite
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
VOID BulletSprite(PSPRITE_OBJECT psoCur)
{
    BOOL bHitX = FALSE;
    BOOL bHitY = FALSE;
    INT  amount;
    USHORT rc;
    ULONG  ulPost;
    BOOL bContinue = TRUE;
    PACTIVE_BITMAP_LIST pabSprite;

    dorand();

    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 4);

    psoCur->pshHeader->usControlType = CONTROL_MANUAL;
    pabSprite->pbiInfo->usLoopType   = LOOP_FORWARD;
    psoCur->phiInfo->usHitControl    = HIT_CONTROL_ON;
    psoCur->psiLoc->usFrameRate      = 1;

    //
    // Tells sprite engine and graphcs
    // engine this sprite is ready to go!
    //
    psoCur->pshHeader->usState = STATE_ACTIVE;

    while(bContinue)
    {
        bContinue = AutoMoveSprite(psoCur);

        if(psoCur->usMessage == MESSAGE_COLLISION)
        {
            DisplayMessage("%s - got message - Collision.\n", psoCur->pshHeader->pszSpriteName);
            if(psoCur->phiInfo->psoHit)
            {
                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_OTHER)
                {
                    sprintf (szTmp, "sprite %s collided with sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_BY_OTHER)
                {
                    sprintf (szTmp, "sprite %s was hit by sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                psoCur->phiInfo->usHitState = HIT_STATE_NOT_HIT;


                //
                // We only want to allow hits by a
                // computer object.
                //
                if(psoCur->phiInfo->psoHit->pshHeader->usSpriteType  != TYPE_HUMAN_FOCUS &&
                    psoCur->phiInfo->psoHit->pshHeader->usSpriteType != TYPE_HUMAN_NONFOCUS
                    && psoCur->phiInfo->usHitControl                 != HIT_CONTROL_OFF) //@RAT98
                {
                    psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
                    psoCur->phiInfo->psoHit    = NULL;
                    psoCur->usMessage = 0;

                    psoCur->psiLoc->usXVelocity = 10;
                    psoCur->psiLoc->usYVelocity = 10;
                    psoCur->psiLoc->usFrameRate = 1;

                    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 3);

                    pabSprite->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;

                    g_sScore += 10;
                    //@RAT98
                    // bContinue = FALSE;
                }
            }
        }

        if(psoCur->usMessage == MESSAGE_LOOP_DONE)
        {
            DisplayMessage("%s - got message - Loop Done.\n", psoCur->pshHeader->pszSpriteName);

            bContinue = FALSE;

        }

        if(bContinue)
        {


            DosSleep(g_usPlayerTime);

            //DisplayMessage("waiting on  sem for %s\n", psoCur->pshHeader->pszSpriteName);
            //rc = DosWaitEventSem(psoCur->pshHeader->semThread, SEM_INDEFINITE_WAIT);
            //DisplayMessage("Got Posted sem for %s\n", psoCur->pshHeader->pszSpriteName);
            //rc = DosResetEventSem(psoCur->pshHeader->semThread, &ulPost);
        }
    }

    DisplayMessage("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);
    DeleteSprite(psoCur);

    _endthread();

}
/*__________________________________________________________________________
|
|    Function: Basic computer missle.
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
VOID ComBulletSprite(PSPRITE_OBJECT psoCur)
{
    BOOL bHitX = FALSE;
    BOOL bHitY = FALSE;
    INT  amount;
    USHORT rc;
    ULONG  ulPost;
    BOOL bContinue = TRUE;
    PACTIVE_BITMAP_LIST pabSprite;

    dorand();

    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 5);

    psoCur->pshHeader->usControlType = CONTROL_MANUAL;  // If Auto, it puts to many on the auto-thread.  Split auto up???

    pabSprite->pbiInfo->usLoopType  = LOOP_FORWARD_ONCE;

    psoCur->phiInfo->usHitControl  = HIT_CONTROL_ON;
    psoCur->psiLoc->usFrameRate = 1;


    //
    // Tells sprite engine and graphcs
    // engine this sprite is ready to go!
    //
    psoCur->pshHeader->usState           = STATE_ACTIVE;

    while(bContinue)
    {
        if(!AutoMoveSprite(psoCur))
        {
            //  if(psoCur->usMessage == MESSAGE_BOUNDRY_HIT)
            bContinue = FALSE;
            //  else
            //     {
            //      psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
            //      psoCur->phiInfo->psoHit    = NULL;
            //      psoCur->usMessage = 0;
            //
            //      //psoCur->pabInfo = NULL;
            //      pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 1);
            //
            //      pabSprite->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;
            //     }
        }


        switch(psoCur->usMessage)
        {
        case MESSAGE_COLLISION:
            if(psoCur->phiInfo->psoHit)
            {
                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_OTHER)
                {
                    sprintf (szTmp,"sprite %s collided with sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }
                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_BY_OTHER)
                {
                    sprintf (szTmp, "sprite %s was hit by sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                psoCur->phiInfo->usHitState = HIT_STATE_NOT_HIT;


                //
                // We only want to allow hits by a
                // computer object.
                //
                if(psoCur->phiInfo->psoHit->pshHeader->usSpriteType     != TYPE_COMPUTER
                    && psoCur->phiInfo->psoHit->pshHeader->usSpriteType  != TYPE_COMPUTER_MISSLE
                    && psoCur->phiInfo->usHitControl                     != HIT_CONTROL_OFF) //@RAT98
                {
                    psoCur->pshHeader->usState = STATE_UNKNOWN;
                    psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
                    psoCur->phiInfo->psoHit    = NULL;
                    psoCur->usMessage = 0;

                    g_sScore -= 10;

                    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 3);

                    pabSprite->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;

                    //bContinue = FALSE;
                }
            }

            break;


            //
            // We specified to only go through
            // the loop once.
            //
        case MESSAGE_LOOP_DONE:
            printf("%s - got message - LoopDone.\n", psoCur->pshHeader->pszSpriteName);
            DisplayMessage("%s - got message - LoopDone.\n", psoCur->pshHeader->pszSpriteName);

            psoCur->pshHeader->usControlType = CONTROL_MANUAL;
            psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
            psoCur->pshHeader->usState = STATE_UNKNOWN;

            bContinue = FALSE;
            break;
        }

        if(bContinue)
        {

            DosSleep(g_usPlayerTime);

            // //DisplayMessage("waiting on  sem for %s\n", psoCur->pshHeader->pszSpriteName);
            // rc = DosWaitEventSem(psoCur->pshHeader->semThread, SEM_INDEFINITE_WAIT);
            // //DisplayMessage("Got Posted sem for %s\n", psoCur->pshHeader->pszSpriteName);
            // rc = DosResetEventSem(psoCur->pshHeader->semThread, &ulPost);
        }
    }

    DisplayMessage("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);
    DeleteSprite(psoCur);

    _endthread();
}

/*__________________________________________________________________________
|
|    Function: Heat seeking
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
VOID ComHeatMissleSprite(PSPRITE_OBJECT psoCur)
{
    BOOL bHitX = FALSE;
    BOOL bHitY = FALSE;
    INT  amount;
    USHORT rc;
    ULONG  ulPost;
    BOOL bContinue = TRUE;
    PSPRITE_OBJECT psoPlayer;
    PSPRITE_OBJECT psoTmp;
    USHORT usCounter = 0;
    PACTIVE_BITMAP_LIST pabSprite;

    dorand();

    psoTmp = psoStart;
    while(psoTmp)
    {
        if(psoTmp->pshHeader->usSpriteType == TYPE_HUMAN_FOCUS)
        {
            psoPlayer = psoTmp;

            psoTmp = NULL;
        }
        else
            psoTmp = psoTmp->pNextNode;
    }

    if(psoPlayer)
    {
        psoCur->psiLoc->fDestinationX = psoPlayer->psiLoc->fCurX;
        psoCur->psiLoc->fDestinationY = psoPlayer->psiLoc->fCurY;

        psoCur->psiLoc->usXVelocity = psoPlayer->psiLoc->usXVelocity + 5;
        psoCur->psiLoc->usYVelocity = psoPlayer->psiLoc->usYVelocity + 5;
    }

    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 6);

    psoCur->pshHeader->usControlType = CONTROL_MANUAL;
    pabSprite->pbiInfo->usLoopType    = LOOP_FORWARD;
    psoCur->phiInfo->usHitControl  = HIT_CONTROL_ON;


    //
    // Tells sprite engine and graphcs
    // engine this sprite is ready to go!
    //
    psoCur->pshHeader->usState           = STATE_ACTIVE;

    while(bContinue)
    {
        bContinue = AutoMoveSprite(psoCur);

        if(psoCur->usMessage == MESSAGE_COLLISION)
        {
            if(psoCur->phiInfo->psoHit)
            {
                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_OTHER)
                {
                    sprintf (szTmp, "sprite %s collided with sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_BY_OTHER)
                {
                    sprintf (szTmp, "sprite %s was hit by sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                psoCur->phiInfo->usHitState = HIT_STATE_NOT_HIT;


                //
                // We only want to allow hits by a
                // computer object.
                //
                if(psoCur->phiInfo->psoHit->pshHeader->usSpriteType != TYPE_COMPUTER && psoCur->phiInfo->psoHit->pshHeader->usSpriteType != TYPE_COMPUTER_MISSLE
                    && psoCur->phiInfo->usHitControl != HIT_CONTROL_OFF) //@RAT98
                {
                    psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
                    psoCur->phiInfo->psoHit    = NULL;
                    psoCur->usMessage = 0;

                    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 3);

                    pabSprite->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;

                    g_sScore -= 10;
                    //bContinue = FALSE;
                }
            }
        }

        if(psoCur->usMessage == MESSAGE_AUTOMOVE_DONE)
        {
            DisplayMessage("%s auto move message rec.\n", psoCur->pshHeader->pszSpriteName);
            usCounter++;
            if(usCounter > 5)
                bContinue = FALSE;
            else
            {
                if(psoPlayer)
                {
                    psoCur->psiLoc->fDestinationX = psoPlayer->psiLoc->fCurX;
                    psoCur->psiLoc->fDestinationY = psoPlayer->psiLoc->fCurY;

                    psoCur->psiLoc->usXVelocity = psoPlayer->psiLoc->usXVelocity + 5;
                    psoCur->psiLoc->usYVelocity = psoPlayer->psiLoc->usYVelocity + 5;
                }
            }
        }

        if(psoCur->usMessage == MESSAGE_BOUNDRY_HIT)
        {
            usCounter++;
            if(usCounter > 5)
                bContinue = FALSE;
            else
            {
                if(psoPlayer)
                {
                    psoCur->psiLoc->fDestinationX = psoPlayer->psiLoc->fCurX;
                    psoCur->psiLoc->fDestinationY = psoPlayer->psiLoc->fCurY;

                    psoCur->psiLoc->usXVelocity = psoPlayer->psiLoc->usXVelocity + 5;
                    psoCur->psiLoc->usYVelocity = psoPlayer->psiLoc->usYVelocity + 5;
                }
            }

            DisplayMessage("%s boundry hit message rec.\n", psoCur->pshHeader->pszSpriteName);
        }

        if(psoCur->usMessage == MESSAGE_LOOP_DONE)
        {
            DisplayMessage("%s - got message - Loop Done.\n", psoCur->pshHeader->pszSpriteName);

            bContinue = FALSE;

        }

        if(bContinue)
        {
            psoCur->usMessage = 0;
            DosSleep(g_usPlayerTime);
        }
    }

    DisplayMessage("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);
    DeleteSprite(psoCur);

    _endthread();
}

/*__________________________________________________________________________
|
|    Function: SmartMissle
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
VOID SmartMissle(PSPRITE_OBJECT psoCur)
{
    BOOL bHitX = FALSE;
    BOOL bHitY = FALSE;
    INT  amount;
    USHORT rc;
    ULONG  ulPost;
    BOOL bContinue = TRUE;

    PSPRITE_OBJECT psoPlayer;
    PSPRITE_OBJECT psoTmp;
    USHORT usCounter = 0;
    PACTIVE_BITMAP_LIST pabSprite;

    dorand();

    psoTmp = psoStart;

    //
    // Tells sprite engine and graphcs
    // engine this sprite is ready to go!
    //
    psoCur->pshHeader->usState = STATE_ACTIVE;

    //
    // Find the player's sprite.
    //
    while(psoTmp)
    {
        if(psoTmp->pshHeader->usSpriteType == TYPE_HUMAN_FOCUS)
        {
            psoPlayer = psoTmp;

            psoTmp = NULL;
        }
        else
            psoTmp = psoTmp->pNextNode;
    }

    if(psoPlayer)
    {
        psoCur->psiLoc->fDestinationX = psoPlayer->psiLoc->fCurX;
        psoCur->psiLoc->fDestinationY = psoPlayer->psiLoc->fCurY;

        psoCur->psiLoc->usXVelocity = psoPlayer->psiLoc->usXVelocity + 5;
        psoCur->psiLoc->usYVelocity = psoPlayer->psiLoc->usYVelocity + 5;
    }

    dorand();

    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpAliveSprite, 7);

    psoCur->pshHeader->usControlType = CONTROL_MANUAL;
    pabSprite->pbiInfo->usLoopType   = LOOP_FORWARD;
    psoCur->phiInfo->usHitControl    = HIT_CONTROL_ON;
    psoCur->psiLoc->usFrameRate      = 1;

    while(bContinue)
    {
        psoCur->psiLoc->fDestinationX = psoPlayer->psiLoc->fCurX;
        psoCur->psiLoc->fDestinationY = psoPlayer->psiLoc->fCurY;

        bContinue = AutoMoveSprite(psoCur);

        if(psoCur->usMessage == MESSAGE_COLLISION)
        {
            if(psoCur->phiInfo->psoHit)
            {
                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_OTHER)
                {
                    sprintf (szTmp, "sprite %s collided with sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                if(psoCur->phiInfo->usHitState == HIT_STATE_HIT_BY_OTHER)
                {
                    sprintf (szTmp, "sprite %s was hit by sprite %s\n", psoCur->pshHeader->pszSpriteName,
                        psoCur->phiInfo->psoHit->pshHeader->pszSpriteName);

                    DisplayMessage(szTmp, NULL);
                }

                psoCur->phiInfo->usHitState = HIT_STATE_NOT_HIT;


                //
                // We only want to allow hits by a
                // computer object.
                //
                if(psoCur->phiInfo->psoHit->pshHeader->usSpriteType     != TYPE_COMPUTER
                    && psoCur->phiInfo->psoHit->pshHeader->usSpriteType != TYPE_COMPUTER_MISSLE
                    && psoCur->phiInfo->usHitControl                    != HIT_CONTROL_OFF) //@RAT98
                {
                    psoCur->phiInfo->usHitControl = HIT_CONTROL_OFF;
                    psoCur->phiInfo->psoHit    = NULL;
                    psoCur->usMessage = 0;

                    pabSprite = *AddActiveNode(&psoCur->pabInfo, psoCur->pbmpDeadSprite, 3);

                    pabSprite->pbiInfo->usLoopType = LOOP_FORWARD_ONCE;

                    g_sScore -= 10;
                    //bContinue = FALSE;
                }
            }
        }

        if(psoCur->usMessage == MESSAGE_AUTOMOVE_DONE)
        {
            psoCur->pshHeader->usControlType = CONTROL_MANUAL;
            DisplayMessage("%s auto move message rec.\n", psoCur->pshHeader->pszSpriteName);
            bContinue = FALSE;
        }

        if(psoCur->usMessage == MESSAGE_BOUNDRY_HIT)
        {
            psoCur->pshHeader->usControlType = CONTROL_MANUAL;
            bContinue = FALSE;
            DisplayMessage("%s boundry hit message rec.\n", psoCur->pshHeader->pszSpriteName);
        }

        if(psoCur->usMessage == MESSAGE_LOOP_DONE)
        {
            DisplayMessage("%s - got message - Loop Done.\n", psoCur->pshHeader->pszSpriteName);

            bContinue = FALSE;
        }

        if(bContinue)
        {
            psoCur->usMessage = 0;
            DosSleep(g_usPlayerTime);

            // DisplayMessage("waiting on  sem for %s\n", psoCur->pshHeader->pszSpriteName);
            // rc = DosWaitEventSem(psoCur->pshHeader->semThread, SEM_INDEFINITE_WAIT);
            // DisplayMessage("Got Posted sem for %s\n", psoCur->pshHeader->pszSpriteName);
            // rc = DosResetEventSem(psoCur->pshHeader->semThread, &ulPost);
        }
    }

    DisplayMessage("%s sprite thread done.\n", psoCur->pshHeader->pszSpriteName);
    DeleteSprite(psoCur);

    _endthread();
}

/*__________________________________________________________________________
|
|    Function: Start the Game Engine
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
|06/15/95  RATAJIK Initial Development.
|__________________________________________________________________________*/
BOOL StartGameThread( VOID )
{
    PID pid;

    pid = _beginthread(GameThread,
        0,
        (unsigned)5000,
        (PVOID)NULL);

    if(pid)
    {
        usThreads++;
        g_ulMemory += 5000;
        DisplayMessage("Started Game Thread.\n", NULL);

    }
    else
    {
        DosBeep(100, 10);
        DisplayMessage("Unable to start Game Thread.\n", NULL);

    }

    return;

    // Enable this if to delete sprites
    // WARNING:  Will not work unless
    // blocking is put back in!
    //
    #if 0
    pid = _beginthread((void *)(void *)CleanupThread,
        0,
        (unsigned)4000,
        (PVOID)NULL);

    if(pid)
    {
        usThreads++;
        g_ulMemory += 4000;
        DisplayMessage("Started Cleanup Thread.\n", NULL);

    }
    else
    {
        DosBeep(100, 10);
        DisplayMessage("Unable to start Clean Thread.\n", NULL);
        return(FALSE);
    }
    #endif

    return(TRUE);
}

/*__________________________________________________________________________
|
|    Function: GameThread
|
| Description: This is the engine that controls the events in the game.
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
void _Optlink GameThread               ( VOID *p)
{
    BOOL bFirst = TRUE;
    INT  i;
    ULONG ulTime = 0;

    DisplayMessage("Game Thread started.\n", NULL);

    //
    // FUTURE:  Add run sem here.
    //
    while (TRUE)
    {
        if ( winData.fStart )
        {
            if(hJoystick)
                GetJoyStickInfo(hJoystick);

            if(bFirst)
            {
                bFirst = FALSE;
                CreateWalker();
            }

            dorand();

            //
            // Determine if we want to create a walker.
            //
            if(ulTime > 75)
            {
                if((i = GetRandom(1, 10)) == 1)
                {
                    ulTime = 0;
                    CreateWalker();
                }

            }

            ulTime++;
        }
        else
            ;

        DosSleep(25L);
    }

    usThreads--;
}

/*__________________________________________________________________________
|
|    Function: CreateWalker
|
| Description: Create the Walker sprite.
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
BOOL CreateWalker( VOID )
{
    static INT            j = 0;
    PSPRITE_OBJECT psoCur;
    CHAR           szName[CCHMAXPATH];
    PBMPDATA       pbmpNewNode;

    j++;

    if(!CreateSprite(&psoStart, "Robot", TYPE_COMPUTER, (PVOID)ComputerSprite, &psoCur))
    {
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH1.BMP");
        pbmpNewNode->sIndex = 3;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH2.BMP");

        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH3.BMP");

        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH4.BMP");
        pbmpNewNode->sIndex = -1;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH5.BMP");
        pbmpNewNode->sIndex = -2;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH6.BMP");
        pbmpNewNode->sIndex = -2;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH7.BMP");
        pbmpNewNode->sIndex = -3;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH8.BMP");
        pbmpNewNode->sIndex = -3;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH9.BMP");
        pbmpNewNode->sIndex = -4;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH10.BMP");
        pbmpNewNode->sIndex = -5;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH12.BMP");
        pbmpNewNode->sIndex = -3;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH13.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = -2;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH14.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = -1;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH15.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = 0;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH16.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = 0;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH17.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = -1;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH18.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = -2;

        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH20.BMP");
        pbmpNewNode->sIndex = -2;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH21.BMP");
        pbmpNewNode->sIndex = -3;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH22.BMP");
        pbmpNewNode->sIndex = -3;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH23.BMP");
        pbmpNewNode->sIndex = -3;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH24.BMP");
        pbmpNewNode->sIndex = -4;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH25.BMP");
        pbmpNewNode->sIndex = -5;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH26.BMP");
        pbmpNewNode->sIndex = -4;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH27.BMP");
        pbmpNewNode->sIndex = -3;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH28.BMP");
        pbmpNewNode->sIndex = -2;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH29.BMP");
        pbmpNewNode->sIndex = -1;
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "MECH30.BMP");
        pbmpNewNode->sIndex = 0;
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE1.BMP");

        AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE2.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE3.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE4.BMP");
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE5.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = 3;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE6.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = 3;
        pbmpNewNode = AddBMPToSprite(&psoCur->pbmpDeadSprite, "BYE7.BMP");
        pbmpNewNode->bAlwaysPlay = TRUE;
        pbmpNewNode->sIndex = 3;
    }

    psoCur->psiLoc->fCurX = -90;
    psoCur->psiLoc->fCurY = 1;

    StartSpriteThread(psoCur);
}

/*__________________________________________________________________________
|
|    Function: InitJoy
|
| Description: Init the joystick driver
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
HFILE InitJoy( VOID )
{
    APIRET                  rc;
    HFILE                   hGame;
    ULONG                   action;
    ULONG                   version;
    GAME_PARM_STRUCT        gameParms;
    GAME_CALIB_STRUCT       gameCalib;
    GAME_STATUS_STRUCT      gameStatus;
    ULONG                   parmLen, dataLen;

    rc = DosOpen(
        GAMEPDDNAME,
        &hGame,
        &action,
        0,
        FILE_READONLY,
        FILE_OPEN,
        OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE,
        NULL);


    return(hGame);
}

/*__________________________________________________________________________
|
|    Function: CalibrateJoystick
|
| Description: Set the joystick up.
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
BOOL CalibrateJoystick( HFILE hGame )
{
    APIRET                  rc;

    ULONG                   action;
    ULONG                   version;
    GAME_PARM_STRUCT        gameParms;
    GAME_CALIB_STRUCT       gameCalib;
    GAME_STATUS_STRUCT      gameStatus;
    ULONG                   dataLen;
    ULONG                   parmLen = sizeof(gameCalib);

    gameParms.sampDiv = 32;
    parmLen = sizeof(gameParms);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER,  GAME_SET_PARMS,
        &gameParms, parmLen, &parmLen,
        NULL, 0, NULL);
    if (rc != 0)
    {
        //        return(FALSE);

    }

#if 0
    DosBeep(800,100);
    dataLen = sizeof(gameStatus);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER, GAME_GET_STATUS_BUTWAIT,
        NULL, 0, NULL,
        &gameStatus, dataLen, &dataLen);

    if (rc != 0)
    {

        return(FALSE);
    }

    DosBeep(800,100);
    dataLen = sizeof(gameStatus);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER, GAME_GET_STATUS_BUTWAIT,
        NULL, 0, NULL,
        &gameStatus, dataLen, &dataLen);

    if (rc != 0)
    {

        return(FALSE);
    }


    DosBeep(800,100);
    dataLen = sizeof(gameStatus);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER, GAME_GET_STATUS_BUTWAIT,
        NULL, 0, NULL,
        &gameStatus, dataLen, &dataLen);

    if (rc != 0)
    {

        return(FALSE);
    }

    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER, GAME_SET_CALIB,
        &gameCalib, parmLen, &parmLen,
        NULL, 0, NULL);

#endif
    dataLen = sizeof(version);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER,  GAME_GET_VERSION,
        NULL, 0, NULL,
        &version, dataLen, &dataLen);
    if (rc != 0)
    {
        return(FALSE);

    }
    dataLen = sizeof(gameParms);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER,  GAME_GET_PARMS,
        NULL, 0, NULL,
        &gameParms, dataLen, &dataLen);
    if (rc != 0)
    {

        return(FALSE);
    }

    if ((gameParms.useA == 0) && (gameParms.useB == 0))
    {
        return(FALSE);
        DosClose(hGame);
        exit(0);
    }
#if 0
    if (gameParms.useA & GAME_USE_BOTH_NEWMASK)
        cout << "Joystick A in use" << endl;
    else if (gameParms.useA & GAME_USE_X_NEWMASK)
        cout << "Joystick Ax in use" << endl;
    else if (gameParms.useA & GAME_USE_Y_NEWMASK)
        cout << "Joystick Ay in use" << endl;

    if (gameParms.useB & GAME_USE_BOTH_NEWMASK)
        cout << "Joystick B in use" << endl;
    else if (gameParms.useB & GAME_USE_X_NEWMASK)
        cout << "Joystick Bx in use" << endl;
    else if (gameParms.useB & GAME_USE_Y_NEWMASK)
        cout << "Joystick By in use" << endl;
#endif
    dataLen = sizeof(gameCalib);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER,  GAME_GET_CALIB,
        NULL, 0, NULL,
        &gameCalib, dataLen, &dataLen);
    if (rc != 0)
    {
        return(FALSE);
        exit(-1);
    }
}

/*__________________________________________________________________________
|
|    Function: GetJoystickInfo
|
| Description: Get button states/current pos of the joystick
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
BOOL GetJoyStickInfo( HFILE hGame)
{
    APIRET                  rc;
    ULONG                   action;
    ULONG                   version;
    GAME_PARM_STRUCT        gameParms;
    GAME_CALIB_STRUCT       gameCalib;
    GAME_STATUS_STRUCT      gameStatus;
    ULONG                   parmLen, dataLen;

    dataLen = sizeof(gameStatus);
    rc = DosDevIOCtl(
        hGame,
        IOCTL_CAT_USER, GAME_GET_STATUS,
        NULL, 0, NULL,
        &gameStatus, dataLen, &dataLen);
    if (rc != 0)
    {

        return(FALSE);
    }

    {
        SHORT x;
        SHORT y;

        x = gameStatus.curdata.A.x;
        y = gameStatus.curdata.A.y;

        if(x == 134 || (x < 134 + 15 && x > 134 - 15))
        {
            winData.fLeft   = FALSE;
            winData.fRight   = FALSE;

            WinSetDlgItemText(g_hWndStatus, TXT_MOV_X, "Center");
        }
        else
        {
            if(x > 134)
            {
                winData.fRight = TRUE;
                winData.fLeft  = FALSE;

                WinSetDlgItemText(g_hWndStatus, TXT_MOV_X, "Right");
            }
            else
            {
                winData.fRight = FALSE;
                winData.fLeft  = TRUE;

                WinSetDlgItemText(g_hWndStatus, TXT_MOV_X, "Left");
            }

        }

        if(y == 73  || (y < 73  + 15 && y > 73  - 15))
        {
            winData.fUp      = FALSE;
            winData.fDown    = FALSE;

            WinSetDlgItemText(g_hWndStatus, TXT_MOV_Y, "Center");
        }
        else
        {
            if(y < 73 )
            {
                winData.fUp    = TRUE;
                winData.fDown  = FALSE;

                WinSetDlgItemText(g_hWndStatus, TXT_MOV_Y, "Up");
            }
            else
            {
                winData.fUp    = FALSE;
                winData.fDown  = TRUE;

                WinSetDlgItemText(g_hWndStatus, TXT_MOV_Y, "Down");
            }

        }

        WinSetDlgItemShort(g_hWndStatus, TXT_JOY_X, x, TRUE);
        WinSetDlgItemShort(g_hWndStatus, TXT_JOY_Y, y, TRUE);

        if(gameStatus.b1cnt)
        {
            bFire = TRUE;
            WinSetDlgItemText(g_hWndStatus, TXT_BUT_1, "Fire");
            {
                static MCI_PLAY_PARMS   mciPlayParameters;
                ULONG rc;
                MCI_SEEK_PARMS mseekp;

                rc = mciSendCommand(g_usPlayer, MCI_SEEK, MCI_TO_START, (PVOID)&mseekp,0);
                printf("seekrc=%lu\n", rc);
                rc = mciSendCommand(g_usPlayer,
                    MCI_PLAY,
                    0,
                    (PVOID) &mciPlayParameters,
                    0);
                printf("playrc=%lu\n", rc);
            }

        }
        else
        {
            //bFire = FALSE;

            WinSetDlgItemText(g_hWndStatus, TXT_BUT_1, " ");
        }


        if(gameStatus.b2cnt)
        {
            WinSetDlgItemText(g_hWndStatus, TXT_BUT_2, "Lock");
        }
        else
        {
            WinSetDlgItemText(g_hWndStatus, TXT_BUT_2, " ");
        }

    }
}

/*__________________________________________________________________________
|
|    Function: CreateComBulletSprite
|
| Description: Create a simple computer bullet sprite
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
BOOL CreateComBulletSprite(PSPRITE_OBJECT psoCur)
{
    PSPRITE_OBJECT psoNew;
    CHAR szName[CCHMAXPATH];
    static INT j = 0;

    sprintf (szName, "%u%s", j, "ComBullet");

    j++;
    if(!CreateSprite(&psoStart, "ComputerMissle", TYPE_COMPUTER_MISSLE, (PVOID)ComBulletSprite, &psoNew))
    {
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT1.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT2.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT3.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT4.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT5.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT6.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT7.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT8.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT7.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT6.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT5.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT4.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT3.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT2.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "LIGHT1.BMP");

        AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD1.BMP");
        AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD2.BMP");
        AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD3.BMP");
        AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD4.BMP");
        AddBMPToSprite(&psoNew->pbmpDeadSprite, "LIGHTD5.BMP");
    }

    psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
    psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

    dorand();
    {
        PSPRITE_OBJECT psoTmp = psoStart;
        while(psoTmp)
        {

            if(psoTmp->pshHeader->usSpriteType == TYPE_HUMAN_FOCUS)
            {
                float fFromX;
                float fFromY;
                float fTargetX;
                float fTargetY;
                float ulWidth ;
                float ulHeight;
                float x, y, v;

                ulWidth  = bmpScene.ulWidth ;
                ulHeight = bmpScene.ulHeight;

                fFromX = psoNew->psiLoc->fDestinationX;
                fFromY = psoNew->psiLoc->fDestinationY;

                fTargetX = psoTmp->psiLoc->fCurX;
                fTargetY = psoTmp->psiLoc->fCurY;

                v = GetRandom((psoTmp->psiLoc->usXVelocity+5), 40);

                psoNew->psiLoc->usXVelocity += v;
                psoNew->psiLoc->usYVelocity += v;

                if(fFromX < fTargetX)
                {
                    x = fTargetX - fFromX;
                }
                else
                {
                    x = fFromX - fTargetX;
                }

                if(fFromY < fTargetY)
                {
                    y = fTargetY - fFromY;
                }
                else
                {
                    y = fFromY - fTargetY;
                }

                psoNew->psiLoc->fDestinationX = fTargetX;
                psoNew->psiLoc->fDestinationY = fTargetY;

                if(x > y)
                    psoNew->psiLoc->usXVelocity += ( x / y);
                else
                    psoNew->psiLoc->usYVelocity += ( y / x);

                psoTmp = NULL;
            }
            else
                psoTmp = psoTmp->pNextNode;
        }
    }


    StartSpriteThread(psoNew);
}

/*__________________________________________________________________________
|
|    Function: CreateComHeatSprite
|
| Description: Create a jump-heat seaker sprite.
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
BOOL CreateComHeatMissleSprite(PSPRITE_OBJECT psoCur)
{
    static INT            j = 0;
    PSPRITE_OBJECT psoNew;
    CHAR           szName[CCHMAXPATH];

    sprintf (szName, "%u%s", j, "ComHeat");
    j++;

    if(!CreateSprite(&psoStart, "ComputerMissle", TYPE_COMPUTER_MISSLE, (PVOID)ComHeatMissleSprite, &psoNew))
    {
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL1.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL2.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL3.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL4.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL5.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL6.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL5.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL4.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL3.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL2.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL1.BMP");
    }

    psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
    psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

    StartSpriteThread(psoNew);
}

/*__________________________________________________________________________
|
|    Function: CreateSmartMissle
|
| Description: Create a smart (tracker) missle.
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
BOOL CreateSmartMissle(PSPRITE_OBJECT psoCur)
{
    static INT            j = 0;
    PSPRITE_OBJECT psoNew;
    CHAR           szName[CCHMAXPATH];

    sprintf (szName, "%u%s", j, "ComHeat");
    j++;

    if(!CreateSprite(&psoStart, "ComputerMissle", TYPE_COMPUTER_MISSLE, (PVOID)SmartMissle, &psoNew))
    {
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL1.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL2.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL3.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL4.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL5.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL6.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL5.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL4.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL3.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL2.BMP");
        AddBMPToSprite(&psoNew->pbmpAliveSprite, "BULL1.BMP");
    }

    psoNew->psiLoc->fCurX = psoCur->psiLoc->fCurX;
    psoNew->psiLoc->fCurY = psoCur->psiLoc->fCurY;

    dorand();

    StartSpriteThread(psoNew);
}

//
// Not used right now...
// (need to re-enable sem wait's for threads)
//
VOID CleanupThread( VOID )
{
#if 0
    //@MPS

    PSPRITE_OBJECT psoDelete;
    PSPRITE_OBJECT psoTmp;
    PSPRITE_OBJECT psoMark;


    while(TRUE)
    {
        psoDelete = psoStart;
        while(psoDelete)
        {
            if(psoDelete->pshHeader->usState == STATE_UNKNOWN)
            {
                if(psoStart == psoDelete)
                {
                    //DisplayMessage("Delete node.  List empty.\n", NULL);
                    //psoStart = NULL;
                    // DosBeep(100,50);DosBeep(100,50);DosBeep(100,50);
                    //return(TRUE);
                }
                else
                {
                    psoMark = psoDelete->pNextNode;
                    psoTmp = psoDelete->pNextNode;

                    if(psoTmp)
                        psoTmp->pPrevNode = psoDelete->pPrevNode;

                    psoTmp = psoDelete->pPrevNode;

                    if(psoTmp)
                        psoTmp->pNextNode = psoDelete->pNextNode;

                    psoDelete->pbiInfo->pbmpActiveSprite = NULL;

                    DosSleep(5000L);


                    DisplayMessage("CleanThread processed: Deleted sprite %s\n", psoDelete->pshHeader->pszSpriteName);

                    FreeMem(psoDelete->pshHeader->pszSpriteName, strlen(psoDelete->pshHeader->pszSpriteName));
                    DosCloseEventSem(psoDelete->pshHeader->semThread);

                    DeleteBitmapList(psoDelete->pbmpAliveSprite   );
                    DeleteBitmapList(psoDelete->pbmpInactiveSprite);
                    DeleteBitmapList(psoDelete->pbmpDeadSprite    );

                    FreeMem(psoDelete->pshHeader, sizeof(PSPRITE_HEADER));
                    FreeMem(psoDelete->phiInfo  , sizeof(PHIT_INFO     ));
                    FreeMem(psoDelete->psiLoc   , sizeof(PSPACE_INFO   ));
                    FreeMem(psoDelete->pbiInfo  , sizeof(PBITMAP_INFO  ));
                    memset(psoDelete, 0, sizeof(SPRITE_OBJECT));
                    FreeMem(psoDelete, sizeof(SPRITE_OBJECT));
                    psoDelete = psoMark;

                }
            }

            if(psoDelete)
            {
                DosSleep(100L);
                psoDelete = psoDelete->pNextNode;
            }
        }

        DosSleep(500L);
    }

#endif
}

/*__________________________________________________________________________
|
|    Function: AutoMoveSprite2
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
BOOL AutoMoveSprite2(PSPRITE_OBJECT psoCur)
{
    BOOL bContinue = TRUE;
    PACTIVE_BITMAP_LIST pabCur;


    pabCur = psoCur->pabInfo;
    while(pabCur)
    {
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
                        (psoCur->psiLoc->usXVelocity + psoCur->psiLoc->fCurX + pabCur->pbiInfo->pbmpActiveSprite->ulWidth  ) > bmpScene.ulWidth  )
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
                        (psoCur->psiLoc->fCurX + pabCur->pbiInfo->pbmpActiveSprite->ulWidth - psoCur->psiLoc->usXVelocity ) < 1 )
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
                        (psoCur->psiLoc->usYVelocity + psoCur->psiLoc->fCurY + pabCur->pbiInfo->pbmpActiveSprite->ulHeight ) > bmpScene.ulHeight )
                    {
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
                        (psoCur->psiLoc->fCurY + pabCur->pbiInfo->pbmpActiveSprite->ulHeight - psoCur->psiLoc->usYVelocity ) < 1 )
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
|    Function: CreatePlayerSprite
|
| Description: Create the player sprite
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
BOOL CreatePlayerSprite(float fStartX, float fStartY)
{
    PSPRITE_OBJECT psoCur;

    //
    // Create the players sprite.
    //
    //
    if(!CreateSprite(&psoStart, "Player Sprite", TYPE_HUMAN_FOCUS, (PVOID)PlayerSprite, &psoCur))
    {
        //
        // Load the active bitmaps for this
        // sprite.
        //
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L1.BMP");
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L2.BMP");
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L3.BMP");
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L4.BMP");
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L5.BMP");
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L6.BMP");
        AddBMPToSprite(&psoCur->pbmpAliveSprite, "L7.BMP");

        //
        // Load the dead bitmaps for this
        // sprite.
        //
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD1.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD2.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD3.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD4.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD5.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD6.BMP");
        AddBMPToSprite(&psoCur->pbmpDeadSprite, "LD7.BMP");
    }

    //
    // Set the start point for the sprite
    //
    //
    psoCur->psiLoc->fCurX = fStartX;
    psoCur->psiLoc->fCurY = fStartY;

    return(TRUE);
}
