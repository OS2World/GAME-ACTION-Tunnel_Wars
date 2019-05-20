/*--------------------------------------------------------------------------\
|                   Barchu.h - Barch util header file
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
|1.00  04/05/95  RATAJIK Initial Development
|1.00  05/09/98  Ratajik Released.
\_________________________________________________________________________*/
#ifndef DIRECTION
#define DIRECTION extern
#endif

#include "barch.h"
#define BARCH_SIGNATURE 0xBA


typedef struct _BARCH_FILE_HEADER
{
    USHORT usSignature;
    ULONG  ulFiles;
    
    USHORT usArchiveVersion;
    USHORT usProductNumber ;
    CHAR   szArchiveName[50]  ;
    CHAR   szProductName[50]  ;
    CHAR   szArchiveDesc[255] ;
    
} BARCH_FILE_HEADER, *PBARCH_FILE_HEADER;

typedef struct _BARCH_HEADER
{
    CHAR	szFileName[CCHMAXPATH];
    USHORT usFileID  ;
    ULONG	ulOffset  ;
    ULONG	ulSize	  ;
    
} BARCH_HEADER, *PBARCH_HEADER;

//RECTION PBARCH_FILE_LIST     pBegEntry;
//RECTION BARCH_FILE_HEADER    g_bfhFile;
//RECTION ULONG                g_ulFiles;

PBARCH_INFO OpenArchiveForWrite(CHAR *szFileName);
BOOL ArchiveFile           ( PBARCH_INFO pbiInfo , CHAR *szFileName );
BOOL CloseArchiveForWrite  ( PBARCH_INFO pbiInfo );
PBARCH_FILE_LIST AddNewFileEntry(PBARCH_INFO ppbiInfo,  PBARCH_HEADER pEntry, CHAR *szData );
ULONG  GetFileSpace        ( PSZ pszFilename, CHAR chTargetDrive, BOOL fIncludeEAs );
ULONG  GetAllocUnit        ( CHAR chDrive );
ULONG  ReadFileHeader      ( PBARCH_INFO pbiInfo );
BOOL   ReadFileList        ( PBARCH_INFO pbiInfo );
BOOL   WriteFile           ( PBARCH_FILE_LIST pbhEntry, PSZ pszData );
