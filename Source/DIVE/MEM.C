/*--------------------------------------------------------------------------
|
| MEM.C  - Memory Allocation/De-allocation using banks. (Up to 550 megs)
|
| Copyright 1995, Greg Ratajik
/*--------------------------------------------------------------------------\
|    Mem.c - Memory Allocation/De-allocation using banks. (Up to 550 megs)
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
|1.00  06/17/95  RATAJIK Initial Development
|1.00  05/09/98  Ratajik Released.
\_________________________________________________________________________*/

#define INCL_DOSPROCESS     /*  Process and thread support      */
#define INCL_DOSSEMAPHORES  /*  Semaphore support               */
#define INCL_DOSDATETIME    /*  Date & Time info                */
#define INCL_DOSMODULEMGR   /*  PM Dos Mangager                 */
#define INCL_DOSINFOSEG     /*  Information segment information */
#define INCL_DOSSESMGR
#define INCL_DOSMISC
#define INCL_DOSFILEMGR

#define INCL_DOSMEMMGR

/*______________________________________________________________________
|
|                         System Includes
|_____________________________________________________________________*/
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <dive.h>

/*______________________________________________________________________
|
|             Application Specific Includes & Defines
|_____________________________________________________________________*/
#include "common.h"

#define MAX_BUF_MEM 1050000
/*______________________________________________________________________
|
|  Gobal Local Variables                        |
|_____________________________________________________________________*/
PVOID   Mem       = NULL;       /* Pointer to memory object (pool)*/


                                /*______________________________________________________________________
                                |
                                |  Gobal Local Structures
|____________________________________________________________________*/

/*____________________________________________________________________
|
|  Function Prototypes.
|____________________________________________________________________*/
PVOID   CreateNewMemorySegment( VOID );

/*___________________________________________________________________________
|
|    Function: AllocMem
|
| Description: This function will return a pointer to a peice of memory, of
|              the requested size.  The minimum allocation size is 8 bytes.
|              If less then 8 bytes is allocated, 8 bytes will be used.
|
|              This function will allocate up to 270 banks of memory,
|              2 megs - 64 bytes per bank (539,982,720K or about 540 megs)
|
|              NOTE: OS/2 seems to have a problem if the total memory
|              alloc'd exceeds real memory  +  virtual memory (RAM/DRIVE).
|              It hard traps (trap-e)  Sometime in the future, a calc
|              for usable memory should be done. It should also be noted that
|              the max memory allocation per process under Warp is around
|              550 megs, so this function isn't going to go much higher... :)
|
|              This function has been test up to about 100 megs of allocation
|
|      Return: PVOID  (Pointer to the new memory)
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                C H A N G E    L O G                |
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|ID    Date      PRG     Change
|----- --------- ------- ----------------------------------------
|      06-20-95  RATAJIK Initial Developement
|___________________________________________________________________________*/
PVOID AllocMem(ULONG ulSize)
{
    static ULONG  ulTotal = 0;
    static ULONG  ulTotalOnBank = 0;
    static PVOID  MemoryBank[270];
    static INT    CurBank = -1;
    APIRET rc;
    PVOID  Mem = NULL;
    Mem = malloc(ulSize);
    if(!Mem)
    {
        DosBeep(500,900);
    }

    g_ulMemory += ulSize;
    return(Mem);



    //
    // If this is the first time, allocate
    // the buffer
    //
    if(CurBank == -1)
    {
        CurBank = 0;

        //
        // Allocate (but don't commit) the
        // memory, and prepar for SubAlloc's
        //
        MemoryBank[CurBank] = CreateNewMemorySegment();
    }

    ulTotal += ulSize;

    g_ulMemory += ulSize;

    //
    // If the new alloc would overflow
    // the bank, use the next bank. (The
    // banks should be changed to a linked
    // list so we can alloc aas much as
    // OS/2 can handle)
    //
    if(ulTotalOnBank + ulSize > MAX_BUF_MEM - 8048) // OS/2 needs 64 to manage the pool
    {
        CurBank++;


        //
        // If we don't have any more banks,
        // error out.
        //
        if(CurBank > 50)
        {
            printf("This program can only address up to %lu of memory...\n", CurBank * MAX_BUF_MEM);
            return(NULL);
        }

        printf("Switched to memory bank %i\n", CurBank);
        printf("Using memory bank %i.  Total on Bank: %lu  Total: %lu\n", CurBank, ulTotalOnBank, ulTotal);


        //
        // Allocate (but don't commit) the
        // memory, and prepar for SubAlloc's
        MemoryBank[CurBank] = CreateNewMemorySegment( );

        ulTotalOnBank = 0;
    }

    ulTotalOnBank += ulSize;

    //printf("Using memory bank %i.  Total on Bank: %lu  Total: %lu\n", CurBank, ulTotalOnBank, ulTotal);


    //
    // Allocate the memory from the
    // current bank.
    //
    rc = DosSubAllocMem( MemoryBank[CurBank], &Mem, ulSize );
    if( rc != 0 )
    {
        printf( "DosSubAllocMem error: return code = %u\n", rc);
        return(NULL);
    }
    else
        return(Mem);
}


/*___________________________________________________________________________
|
|    Function: CreateNewMemorySegment
|
| Description: This function will allocate a large piece of memory.  The
|              memory will be prepared to be SubAlloc'd.  NOTE: SubAlloc
|              COMMITS the memory, so we don't (nor should we)
|
|      Return: PVOID  (Pointer to the new memory)
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                C H A N G E    L O G                |
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|ID    Date      PRG     Change
|----- --------- ------- ----------------------------------------
|      06-20-95  RATAJIK Initial Developement
|___________________________________________________________________________*/
PVOID CreateNewMemorySegment( VOID )
{
    PVOID Mem;
    APIRET rc;


    //
    // Allocate, but don't commit, the
    // memory.
    //
    rc = DosAllocMem( &Mem, MAX_BUF_MEM, PAG_WRITE|PAG_READ );
    if (rc != 0)
    {
        printf("DosAllocMem error:  return code = %u\n", rc);
        return NULL;
    }
    else
        printf("Allocated 2,000,000\n");


    //
    // Make the entire memory object
    // available for Suballocation
    //
    rc = DosSubSetMem( Mem, DOSSUB_INIT | DOSSUB_SPARSE_OBJ, MAX_BUF_MEM );
    if (rc != 0)
    {
        printf("DosSubSetMem error:  return code = %u\n", rc);
        return NULL;
    }
    else
        printf("Memory object ready for suballocation.\n");

    return(Mem);
}

BOOL    FreeMem  ( PVOID pMem, ULONG ulSize )
{
    return(TRUE);
}
