/*--------------------------------------------------------------------------\
|                    Sprite.h - Main sprite header file
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

/*___________________________________
|                                    |
| Do or don't do hit detection       |
|                                    |
|___________________________________*/
#define HIT_CONTROL_ON          0       /* Do hit detection                */
#define HIT_CONTROL_OFF         1       /* Do not do hit detection         */

/*___________________________________
|                                    |
| Differnt types of hit codes.       |
|                                    |
|___________________________________*/
#define HIT_STATE_NOT_HIT       0       /* No collision                    */
#define HIT_STATE_HIT_OTHER     1       /* psoCur hit psoHit               */
#define HIT_STATE_HIT_BY_OTHER  2       /* psoCur was hit by psoHit        */
#define HIT_STATE_UNKNOWN       3       /* Something else...               */


/*___________________________________
|                                    |
| State the sprite is in.            |
|                                    |
|___________________________________*/
#define STATE_ACTIVE            0       /* Sprite is active                */
#define STATE_INACTIVE          1       /*                                 */
#define STATE_DEAD              2       /*                                 */
#define STATE_UNKNOWN           3       /*                                 */

/*___________________________________
|                                    |
| The type of sprite                 |
|                                    |
|___________________________________*/
#define TYPE_HUMAN_FOCUS        1       /* Sprite is the player sprite     */
#define TYPE_HUMAN_NONFOCUS     2       /* Sprite is a player ? (Missle)   */
#define TYPE_COMPUTER           3       /* Sprite is a Computer sprite     */
#define TYPE_COMPUTER_MISSLE    4       /* Sprite is a Computer missle     */

/*___________________________________
|                                    |
| Control type the sprite is in.     |
|                                    |
|___________________________________*/
#define CONTROL_MANUAL          0       /* Sprite is under manual control  */
#define CONTROL_AUTO            1       /* Sprite is under Auto control    */

/*___________________________________
|                                    |
| Message codes.                     |
|                                    |
|___________________________________*/
#define MESSAGE_WAITING         0       /*                                 */
#define MESSAGE_AUTOMOVE        1       /* Auto move                       */
#define MESSAGE_BOUNDRY_HIT     2       /* A Boundry was hit               */
#define MESSAGE_COLLISION       3       /* A collision took place          */
#define MESSAGE_AUTOMOVE_DONE   4       /* A automove request completed    */
#define MESSAGE_LOOP_DONE       5       /* A bitmap display loop completed.*/

/*___________________________________
|                                    |
| Types of looks to display BMPs of  |
| the sprite in                      |
|___________________________________*/
#define LOOP_FORWARD            0       /* Loop Forward, Continue          */
#define LOOP_FORWARD_ONCE       1       /* Loop Forward, Once              */
#define LOOP_FORWARD_BACK       2       /* Loop Forward, Loop Back,Continue*/
#define LOOP_FORWARD_BACK_ONCE  3       /* Loop Forward, Loop Back, Once   */
#define LOOP_STOP               4       /* Stay on current frame           */

/*___________________________________
|                                    |
| If the sprite was hit/hit something|
| this structure will give info      |
| the type hit.                      |
|___________________________________*/
typedef struct _HIT_INFO
{
    USHORT    usHitControl;        /* To hit or not to hit detect     */
    USHORT    usHitState;          /* If sprite is hitting anything   */

    struct _SPRITE_OBJECT *psoHit; /* If hit, pointer to BMP hitting  */
} HIT_INFO, *PHIT_INFO;

/*___________________________________
|                                    |
| Base information about the sprite. |
|                                    |
|___________________________________*/
typedef struct _SPRITE_HEADER
{
    PSZ       pszSpriteName;       /* Name of the sprite              */
    PSZ       pszSemName;          /* Name of the Sem                 */
    USHORT    usSpriteType;        /* Human or Computer               */
    USHORT    usControlType;       /* Type of control                 */
    USHORT    usState;             /* Current State of the sprite     */

    HEV       semThread;           /* Control thread - wake up        */
    PVOID     pfnSpriteFunction;   /* Thread controlling sprite       */
    PID       pidSpriteThread;     /* PID to the sprite's thread      */

} SPRITE_HEADER, *PSPRITE_HEADER;

/*___________________________________
|                                    |
| Information about where the sprite |
| is, where it is going, and how     |
| fast it is going there.            |
|___________________________________*/
typedef struct _SPACE_INFO
{
    float     fCurX;               /* Current X coordinates           */
    float     fCurY;               /* Current Y coordinates           */

    float     fDestinationX;       /* X Coord where the Sprite is heading*/
    float     fDestinationY;       /* Y Coord where the Sprite is heading*/
    USHORT    usRateAspect;        /* % increase when auto-move          */
    USHORT    usFrameRate;

    BOOL      bHitX;               /* Sprite reached fDestinationX    */
    BOOL      bHitY;               /* Sprite reached fDestinationY    */

    USHORT    usXVelocity;         /* X velocity                      */
    USHORT    usYVelocity;         /* Y velocity                      */
} SPACE_INFO, *PSPACE_INFO;

/*___________________________________
|                                    |
| Information about the bitmaps used |
| to create sprite, and info about   |
| how those bmps are displayed.      |
|___________________________________*/
typedef struct _BITMAP_INFO
{
    USHORT     usBitmapID          ;  /* ID of bitmap - for Multipart */
    BOOL       bDisplaySprite      ;  /* Display Sprite               */
    USHORT     usLoopType          ;  /* Orders to display sprite     */
    PBMPDATA   pbmpActiveSprite    ;  /* Start of Active Sprites List */
    PBMPDATA   pbmpCurActiveSprite ;  /* Current Active Sprite playing*/
    PBMPDATA   pbmpLastActiveSprite;  /* Start of Last Active Sprites List */
} BITMAP_INFO, *PBITMAP_INFO;

/*___________________________________
|                                    |
| List of bitmaps to use.  To support|
| multi-part sprites.                |
|___________________________________*/
typedef struct _ACTIVE_BITMAP_LIST
{
    PBITMAP_INFO pbiInfo;
    ULONG        ulXOffset;
    ULONG        ulYOffset;

    struct _ACTIVE_BITMAP_LIST *pNextNode;
} ACTIVE_BITMAP_LIST, *PACTIVE_BITMAP_LIST;

/*_____________________________________________
|                                              |
| This structure containes information in the  |
| sprite linked list.                          |
|_____________________________________________*/
typedef struct _SPRITE_OBJECT
{
    PSPRITE_HEADER      pshHeader;     /* Sprite Header Information       */
    PHIT_INFO           phiInfo;       /* Sprite Hit information          */
    PSPACE_INFO         psiLoc;        /* Sprite Location/Dest Info       */
    PACTIVE_BITMAP_LIST pabInfo;       /* Sprite Bitmaps                  */

    USHORT         usMessage;          /* Need to change this into a List */

    USHORT         usWeaponType;
    PBMPDATA       pbmpAliveSprite   ; /* List of BMP's to display when alive.      */
    PBMPDATA       pbmpInactiveSprite; /* List of BMP's to display when not moving  */
    PBMPDATA       pbmpDeadSprite    ; /* List of BMP's to display when dead        */

    PVOID          pUserInfo;          /* Anything Else... (Game specific Information)*/

    struct _SPRITE_OBJECT *pPrevNode;
    struct _SPRITE_OBJECT *pNextNode;

} SPRITE_OBJECT, *PSPRITE_OBJECT;

DIRECTION PSPRITE_OBJECT   psoStart;

BOOL           CreateSprite      ( PSPRITE_OBJECT *ppsoList, CHAR *szSpriteName, USHORT usSpriteType, PVOID pfnSpriteFunction, PSPRITE_OBJECT *psoNewSprite);
PBMPDATA       AddBMPToSprite    ( PBMPDATA *pBMPList, CHAR *szBMPName );
PSPRITE_OBJECT DetectCollision   ( PSPRITE_OBJECT psoTest);
BOOL           ProximityCheck    ( PSPRITE_OBJECT psoTest, PSPRITE_OBJECT psoCheck);
BOOL           DeleteSprite      ( PSPRITE_OBJECT psoDelete );
BOOL           CheckForCollision ( PSPRITE_OBJECT psoCur);
INT            dorand            ( VOID );
BOOL           AutoMoveSprite    ( PSPRITE_OBJECT psoCur );
PACTIVE_BITMAP_LIST * AddActiveNode(PACTIVE_BITMAP_LIST *ppabList, PBMPDATA pbmpSprite, USHORT usBitmapID);
BOOL           StartSpriteThread  ( PSPRITE_OBJECT psoCur );
BOOL           ProcessSprite      ( PSPRITE_OBJECT psoCur );

