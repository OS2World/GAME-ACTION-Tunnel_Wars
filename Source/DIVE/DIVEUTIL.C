/*--------------------------------------------------------------------------\
|                   Diveutil.c - general program utilities.
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
|1.00  05/09/98          Released.
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


//#define DIRECTION
#include "common.h"
#include "dive.rch"

static PBITMAP_LIST pBegBit = NULL;
static ULONG ulSize = 0;
static ULONG ulRealSize = 0;

ULONG PutTmpFile(PSZ pszFile, PSZ pszData, USHORT usSize, PBMPDATA pbmpData );
ULONG LoadBitmapFileFromBarch ( unsigned char *pszFile, PBMPDATA pbmpData );
ULONG LoadBitmapFileFromFile ( unsigned char *pszFile, PBMPDATA pbmpData );

/****************************************************************************
*
* Name          : ReadFile
*
* Description   : It opens the file, and reads bitmap header and bitmap
*                 palette, and reads image data to the buffer allocated
*                 by DIVE APIs.
*
* Concepts      :
*
* Parameters    : Index for the file to open
*                 Pointer to the file name to open
*                 Pointer to window data
*
* Return        : 0 - succeed
*                 1 - fail
*
****************************************************************************/

ULONG LoadBitmapFile( unsigned char *pszFile, PBMPDATA pbmpData )
{
    //#ifndef BARCH
    //  LoadBitmapFileFromFile ( pszFile, pbmpData );
    //#else
    LoadBitmapFileFromBarch( pszFile, pbmpData );
    //#endif
}

ULONG LoadBitmapFileFromFile ( unsigned char *pszFile, PBMPDATA pbmpData )
{
    HFILE hFile;                 /* file handke                              */
    ULONG ulNumBytes;            /* output for number of bytes actually read */
    ULONG ulFileLength;          /* file length                              */
    PBYTE pbBuffer;              /* pointer to the image/ temp. palette data */
    ULONG ulScanLineBytes;       /* output for number of bytes a scan line   */
    ULONG ulScanLines;           /* output for height of the buffer          */
    PBITMAPFILEHEADER2 pImgHdr;  /* pointer to bitmapheader                  */
    ULONG i, j;
    PBYTE pbTmpDst;              /* temporaly destination pointer            */
    ULONG ulNumColor;            /* Number of colors for bitmap              */

    if(GetBitmap(pszFile, pbmpData))
        return(0);


        /* Attempt to open up the passed filename.
    */
    if ( DosOpen ( pszFile, &hFile, &ulNumBytes, 0L, FILE_NORMAL,
        OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
        OPEN_ACCESS_READONLY | OPEN_SHARE_DENYREADWRITE |
        OPEN_FLAGS_SEQUENTIAL | OPEN_FLAGS_NO_CACHE, 0L ) )
        return ( 1 );

        /* Get the legnth of the file.
    */
    DosSetFilePtr ( hFile, 0L, FILE_END, &ulFileLength );
    DosSetFilePtr ( hFile, 0L, FILE_BEGIN, &ulNumBytes );

    /* Allocate memory for bitmap file header
    */
    pImgHdr = AllocMem(sizeof(BITMAPFILEHEADER2));
    if(!pImgHdr)
    {
        DosClose ( hFile );
        return ( 1 );
    }

    /* Read from the beginning of the header to cbFix in BITMAPINFOHEADER
    ** to know the length of the header.
    */
    {
        ULONG ul = sizeof(BITMAPFILEHEADER2) - sizeof(BITMAPINFOHEADER2) + sizeof(ULONG);
        printf("");
    }
    if ( DosRead ( hFile, pImgHdr,
        sizeof(BITMAPFILEHEADER2) - sizeof(BITMAPINFOHEADER2) +
        sizeof(ULONG),
        &ulNumBytes ))
    {
        DosFreeMem ( (PVOID)pImgHdr );
        DosClose ( hFile );
        return ( 1 );
    }

    /* Read the rest of the header.
    */
    if ( DosRead ( hFile, (PBYTE)pImgHdr + ulNumBytes,
        pImgHdr->bmp2.cbFix - sizeof(ULONG),
        &ulNumBytes ))
    {
        DosFreeMem ( (PVOID)pImgHdr );
        DosClose ( hFile );
        return ( 1 );
    }

    ulNumColor = 1;

    /* Check the bitmap header format --  new or old one.
    */
    if ( pImgHdr->bmp2.cbFix != sizeof(BITMAPINFOHEADER) )
    {
    /*  Bitmap has new format (BITMAPFILEHEADER2)
        */

        /* Set how many color bitmap data is supporting
        */
        ulNumColor <<= pImgHdr->bmp2.cBitCount;


        /* Make sure bitmap is 256 color format */

        if ( ulNumColor != 256 )
        {
            DosFreeMem ( (PVOID)pImgHdr );
            DosClose ( hFile );
            return ( 1 );
        }


        /* Set bitmap width and height in pels.
        */
        pbmpData->ulWidth  = pImgHdr->bmp2.cx;
        pbmpData->ulHeight = pImgHdr->bmp2.cy;

        /* Calculate source line size.  It should be double word boundary.
        */
        pbmpData->ulLineSizeBytes = ((( pImgHdr->bmp2.cx *
            ( pImgHdr->bmp2.cBitCount >> 3 )) + 3 ) / 4 ) * 4;

            /* Alloate buffer for palette data in bitmap file.
        */
        pPalette = AllocMem(ulNumColor * sizeof(ULONG));
        if(!pPalette)
        {
            DosFreeMem ( (PVOID)pImgHdr );
            DosClose ( hFile );
            return ( 1 );
        }

        /* Read palette data.
        */
        if ( DosRead ( hFile, pPalette,
            ulNumColor * sizeof(ULONG),
            &ulNumBytes ))
        {
            DosFreeMem ( (PVOID)pImgHdr );
            DosFreeMem ( (PVOID)(pPalette) );
            DosClose ( hFile );
            return ( 1 );
        }
    }
    else
    {
    /*  Bitmap has old format (BITMAPFILEHEADER)
        */

        /* Set how many color bitmap data is supporting
        */
        ulNumColor <<= ((PBITMAPFILEHEADER)pImgHdr)->bmp.cBitCount;

        /* Set bitmap width and height in pels.
        */
        pbmpData->ulWidth  = ((PBITMAPFILEHEADER)pImgHdr)->bmp.cx;
        pbmpData->ulHeight = ((PBITMAPFILEHEADER)pImgHdr)->bmp.cy;

        /* Calculate source line size.  It should be double word boundary.
        */
        pbmpData->ulLineSizeBytes = ((( ((PBITMAPFILEHEADER)pImgHdr)->bmp.cx *
            ( ((PBITMAPFILEHEADER)pImgHdr)->bmp.cBitCount >> 3 )) + 3 ) / 4 ) * 4;

            /* Alloate buffer for temporally palette data in bitmap file
        */
        pbBuffer = AllocMem(ulNumColor * sizeof(RGB));
        if(!pbBuffer)
        {
            DosFreeMem ( (PVOID)pImgHdr );
            DosClose ( hFile );
            return ( 1 );
        }

        pPalette = AllocMem(ulNumColor * sizeof(ULONG));
        if(!pPalette)
        {
            DosFreeMem ( (PVOID)pImgHdr );
            DosFreeMem( (PVOID)pbBuffer );
            DosClose ( hFile );
            return ( 1 );
        }

        /* Read palette data
        */
        if ( DosRead ( hFile, pbBuffer,
            ulNumColor * sizeof(RGB),
            &ulNumBytes ))
        {
            DosFreeMem ( (PVOID)pImgHdr );
            DosFreeMem( (PVOID)pbBuffer );
            DosFreeMem( (PVOID)pPalette );
            DosClose ( hFile );
            return ( 1 );
        }

        /* Make each color from 3 bytes to 4 bytes.
        */
        pbTmpDst = pPalette;
        for ( i = 0; i < ulNumColor; i++ )
        {
            *pbTmpDst ++= *pbBuffer++;
            *pbTmpDst ++= *pbBuffer++;
            *pbTmpDst ++= *pbBuffer++;
            *pbTmpDst ++= 0;
        } /* endfor */

        DosFreeMem( (PVOID)pbBuffer );
    }


    /* Allocate a buffer for image data
    */

    pbmpData->pbBuffer = AllocMem(pbmpData->ulWidth * pbmpData->ulHeight);
    if(!pbmpData->pbBuffer)
    {
        DosFreeMem( (PVOID)pPalette );
        DosClose ( hFile );
        return( 1 );
    }

    if ( DosRead ( hFile, pbmpData->pbBuffer,
        pbmpData->ulLineSizeBytes * pbmpData->ulHeight,
        &ulNumBytes ))
    {
        DosFreeMem ( (PVOID)pbmpData->pbBuffer );
        DosFreeMem( (PVOID)pPalette );
        DosClose ( hFile );
        return( 1 );
    }

    DiveSetSourcePalette ( hDive, 0, ulNumColor, pPalette );

    /* Close the file and release the access to the image buffer
    */
    DosFreeMem( (PVOID)pImgHdr );
    DosClose ( hFile );

    pbmpData->bAlwaysPlay = FALSE;

    AddBitmapNode(pszFile, pbmpData);

    return ( 0 );
}

PBARCH_INFO OpenBarch(PSZ pszName)
{
    static BOOL        bFirst = TRUE;
    static PBARCH_INFO m_pbiInfo = NULL;

    /*___________________________________
    |                                    |
    | If this is the first time through, |
    | open and init the BARCH.           |
    |___________________________________*/
    if(bFirst)
    {
        bFirst = FALSE;

        /*___________________________________
        |                                    |
        | Open the BARCH file.               |
        |                                    |
        |___________________________________*/
        if(m_pbiInfo = OpenArchiveForRead("diver.bar"))
        {
            printf("Opened BARCH: %s.\n", "Diver.bar");
        }
        else
            return(NULL);
    }

    return(m_pbiInfo);
}

/*__________________________________________________________________________
|
|    Function: LoadBitmapFile
|
| Description: This function loads the indicated bitmap file into pbmpData.
|              If the bitmap has already been loaded, the loaded bitmap will
|              be returned.
|
|              This version of LoadBitmapFile loades the bitmaps from
|              a BARCH file instead of a bitmap file (same results)
|
|      Return:
|
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|                            C H A N G E    L O G
| -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
|
|Date      PRG     Change
|--------- ------- --------------------------------------------------------
|07/29/95  RATAJIK Converted to use BARCH.  Clean-up.
|__________________________________________________________________________*/
ULONG LoadBitmapFileFromBarch ( unsigned char *pszFile, PBMPDATA pbmpData )
{
    PBARCH_INFO pbiInfo;

    PBITMAPFILEHEADER2 pImgHdr;    /* pointer to bitmapheader                  */
    ULONG              ulNumBytes; /* output for number of bytes actually read */
    PBYTE              pbBuffer;   /* pointer to the image/ temp. palette data */

    ULONG              i, j;
    PBYTE              pbTmpDst;   /* temporaly destination pointer            */
    ULONG              ulNumColor; /* Number of colors for bitmap              */
    PSZ                pszData;    /* Data from Archive                        */
    ULONG              ulCurMemory=0; /* Where in pszData (eq. file pointer  */


    //
    // If the bitmap was already loaded,
    // return it.
    //
    if(GetBitmap(pszFile, pbmpData))
        return(0);

    if( (pbiInfo = OpenBarch(pszFile)) == NULL)
        return(FALSE);


    //
    // Get the bitmap from the BARCH.
    //
    if(pszData = GetFileFromArchive(pbiInfo, pszFile))
    {

        printf("Barch returned data: %s\n", pszFile);
        pImgHdr = AllocMem(sizeof(BITMAPFILEHEADER2));
        if(!pImgHdr)
        {
            return ( 1 );
        }


        //
        // Read from the start of the header
        // to cbFix, to get length (great...)
        ulNumBytes = sizeof(BITMAPFILEHEADER2) - sizeof(BITMAPINFOHEADER2) + sizeof(ULONG);
        memcpy(pImgHdr, pszData, ulNumBytes);

        ulCurMemory = ulNumBytes;


        //
        // Read the rest of the header in.
        //
        memcpy((PBYTE)pImgHdr + ulNumBytes,
            (PBYTE)(pszData + ulCurMemory),
            pImgHdr->bmp2.cbFix - sizeof(ULONG));

        ulCurMemory +=  pImgHdr->bmp2.cbFix - sizeof(ULONG);

        ulNumColor = 1;


        //
        // Figure out which BMP header it is.
        //
        if ( pImgHdr->bmp2.cbFix != sizeof(BITMAPINFOHEADER) )
        {

            //
            // It's the new header... process it.
            //

            //
            // See how many colors it supports.
            //
            ulNumColor <<= pImgHdr->bmp2.cBitCount;


            //
            // We're only going to allow 256 color
            //
            if ( ulNumColor != 256 )
            {
                DosFreeMem ( (PVOID)pImgHdr );
                return ( 1 );
            }


            //
            // Move over the width and height.
            //
            pbmpData->ulWidth  = pImgHdr->bmp2.cx;
            pbmpData->ulHeight = pImgHdr->bmp2.cy;


            //
            // Get the source line size, in double
            // word boundary.
            //
            pbmpData->ulLineSizeBytes = ((( pImgHdr->bmp2.cx *
                ( pImgHdr->bmp2.cBitCount >> 3 )) + 3 ) / 4 ) * 4;


            //
            // Allocate the palette
            //
            pPalette = AllocMem(ulNumColor * sizeof(ULONG));
            if(!pPalette)
            {
                DosFreeMem ( (PVOID)pImgHdr );
                return ( 1 );
            }


            //
            // Copy the palette.
            //
            memcpy( (PBYTE)pPalette,
                (PBYTE)(pszData + ulCurMemory),
                ulNumColor * sizeof(ULONG));

            ulCurMemory += ulNumColor * sizeof(ULONG);

        }
        else
        {

            //
            // The bitmap has the old format.
            //


            //
            // See how many colors it supports.
            //
            ulNumColor <<= ((PBITMAPFILEHEADER)pImgHdr)->bmp.cBitCount;


            //
            // We're only going to allow 256 color
            //
            if ( ulNumColor != 256 )
            {
                DosFreeMem ( (PVOID)pImgHdr );
                return ( 1 );
            }


            //
            // Move over the width and height.
            //
            pbmpData->ulWidth  = ((PBITMAPFILEHEADER)pImgHdr)->bmp.cx;
            pbmpData->ulHeight = ((PBITMAPFILEHEADER)pImgHdr)->bmp.cy;


            //
            // Get the source line size, in double
            // word boundary.
            pbmpData->ulLineSizeBytes = ((( ((PBITMAPFILEHEADER)pImgHdr)->bmp.cx *
                ( ((PBITMAPFILEHEADER)pImgHdr)->bmp.cBitCount >> 3 )) + 3 ) / 4 ) * 4;


            //
            // Allocate the palette
            //
            pbBuffer = AllocMem(ulNumColor * sizeof(RGB));
            if(!pbBuffer)
            {
                DosFreeMem ( (PVOID)pImgHdr );
                return ( 1 );
            }


            //
            // Allocate the palette
            //
            if(!(pPalette = (PBYTE)AllocMem(ulNumColor * sizeof(ULONG))))
            {
                DosFreeMem ( (PVOID)pImgHdr );
                DosFreeMem( (PVOID)pbBuffer );
                return ( 1 );
            }


            //
            // Copy the palette.
            //
            memcpy((PBYTE)pbBuffer,
                (PBYTE)(pszData + ulCurMemory),
                ulNumColor * sizeof(RGB));

            ulCurMemory += ulNumColor * sizeof(RGB);


            //
            // Change each color to 3 bytes to
            // 4 bytes.
            pbTmpDst = pPalette;
            for ( i = 0; i < ulNumColor; i++ )
            {
                *pbTmpDst ++= *pbBuffer++;
                *pbTmpDst ++= *pbBuffer++;
                *pbTmpDst ++= *pbBuffer++;
                *pbTmpDst ++= 0;
            }

            DosFreeMem( (PVOID)pbBuffer );
        }


        //
        // Allocate the buffer of image data.
        //
        pbmpData->pbBuffer = AllocMem(pbmpData->ulWidth * pbmpData->ulHeight);
        if(!pbmpData->pbBuffer)
        {
            DosFreeMem( (PVOID)pPalette );
            return( 1 );
        }


        //
        // Copy the image.
        //
        memcpy((PBYTE)pbmpData->pbBuffer,
            (PBYTE)(pszData + ulCurMemory),
            pbmpData->ulLineSizeBytes * pbmpData->ulHeight);

        ulCurMemory += pbmpData->ulLineSizeBytes * pbmpData->ulHeight;

        pbmpData->ulImage = 0;

        //
        // Set the Palette.
        //
        DiveSetSourcePalette ( hDive, 0, ulNumColor, pPalette );

        //
        // Close the file and release the access to the image buffer
        //
        DosFreeMem( (PVOID)pImgHdr );

        pbmpData->bAlwaysPlay = FALSE;


        //
        // Add the Bitmap to our list.
        //
        AddBitmapNode(pszFile, pbmpData);

        return ( 0 );
    }
}

BOOL GetBitmap(CHAR *pszFile, PBMPDATA pbmpData)
{
    PBITMAP_LIST pCurBit;

    pCurBit = pBegBit;
    while(pCurBit)
    {
        if(!(strcmp(pCurBit->pszBitmapName, pszFile)))
        {
            memcpy(pbmpData, pCurBit->pbmpData, sizeof(BMPDATA));

            pbmpData->ulImage = 0;
            //pbmpData->ulImage          = pCurBit->pbmpData->ulImage;
            pbmpData->pbBuffer         = pCurBit->pbmpData->pbBuffer;
            pbmpData->ulWidth          = pCurBit->pbmpData->ulWidth;
            pbmpData->ulHeight         = pCurBit->pbmpData->ulHeight;
            pbmpData->ulLineSizeBytes  = pCurBit->pbmpData->ulLineSizeBytes;

            pbmpData->pNextNode = NULL;

            DisplayMessage("Returning already alloced bitmap: %s\n", pCurBit->pszBitmapName);
            return(TRUE);
        }

        pCurBit = pCurBit->pNextNode;
    }

    return(FALSE);
}

PBITMAP_LIST  AddBitmapNode(CHAR *pszFile, PBMPDATA pbmpData)
{
    PBITMAP_LIST pNewBit;
    PBITMAP_LIST pCurBit;
    HPS    hPS;


    //
    // Allocate the new node.
    //
    pNewBit = (PBITMAP_LIST)AllocMem(sizeof(BITMAP_LIST));


    //
    // Clear the new memory out.
    //
    memset(pNewBit, 0, sizeof(BITMAP_LIST));


    //
    // Find the last node, and add the
    // new node ot the end of the list.
    //
    if(pBegBit == NULL)
    {
        pCurBit = pBegBit = pNewBit;
    }
    else
    {
        //
        // Find the last node in the list
        //
        for(pCurBit = pBegBit; pCurBit->pNextNode != NULL; pCurBit = pCurBit->pNextNode);


        //
        // Set pNextNode in the current node
        // to the next node.
        pCurBit->pNextNode = pNewBit;
        pCurBit = pNewBit;
    }


    //
    // Allocate enough room for the
    // strings.
    pCurBit->pszBitmapName = (PSZ)AllocMem(strlen(pszFile) + 1);


    //
    // Copy the strings.
    //
    strcpy(pCurBit->pszBitmapName, pszFile);


    //
    // Set the default values.
    //
    pCurBit->pbmpData = AllocMem(sizeof(BMPDATA));
    memcpy(pCurBit->pbmpData, pbmpData, sizeof(BMPDATA));

    pCurBit->pbmpData->ulImage          = pbmpData->ulImage        ;
    pCurBit->pbmpData->ulImage          = 0;
    pCurBit->pbmpData->pbBuffer         = pbmpData->pbBuffer       ;
    pCurBit->pbmpData->ulWidth          = pbmpData->ulWidth        ;
    pCurBit->pbmpData->ulHeight         = pbmpData->ulHeight       ;
    pCurBit->pbmpData->ulLineSizeBytes  = pbmpData->ulLineSizeBytes;

    DisplayMessage("Adding Bitmap: %s\n", pCurBit->pszBitmapName);

    return(pCurBit);
}

VOID DisplayMessage(CHAR *szMessage, CHAR *szParm)
{
    CHAR szOut[255];

    return;

    if(szParm)
        sprintf (szOut, szMessage, szParm);
    else
        strcpy(szOut, szMessage);


    printf("--> Diver: %s", szOut);
    return;
    if(g_hWndStatus)
    {
        PSZ pszText = malloc(strlen(szOut) + 1);
        strcpy(pszText, szOut);

        WinPostMsg(g_hWndStatus,
            DIVE_DISPLAY_MESSAGE,
            0L,
            (MPARAM)pszText);
    }
}
