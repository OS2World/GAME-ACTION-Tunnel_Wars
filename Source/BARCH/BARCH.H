/*--------------------------------------------------------------------------\
|                   Barch.h - Main barch header file
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

#define BARCH_SIGNATURE 0xBA
#define BARCH_VERSION   1

typedef struct _BARCH_FILE_LIST
{
    USHORT usFileID  ;
    ULONG	ulOffset  ;
    ULONG	ulSize	  ;
    CHAR	szFileName[CCHMAXPATH];
    CHAR	*szData   ;
    
    struct _BARCH_FILE_LIST *pPrevNode;
    struct _BARCH_FILE_LIST *pNextNode;
    
} BARCH_FILE_LIST, *PBARCH_FILE_LIST;

typedef struct _BARCH_INFO
{
    FILE             *fpBarchFile;
    ULONG            ulFiles     ;
    
    USHORT           usArchiveVersion;
    USHORT           usProductNumber ;
    CHAR             szArchiveName[50]  ;
    CHAR             szProductName[50]  ;
    CHAR             szArchiveDesc[255] ;
    
    PBARCH_FILE_LIST pBegEntry   ;
    
} BARCH_INFO, *PBARCH_INFO;


PSZ          ReadFileFromArchive( PBARCH_INFO pbiInfo, PBARCH_FILE_LIST pbhEntry );
PBARCH_INFO  OpenArchiveForRead ( CHAR *szFileName);
BOOL         CloseArchiveForRead( PBARCH_INFO pbiInfo);
PSZ          GetFileFromArchive ( PBARCH_INFO pbiInfo, CHAR *szFileName );
ULONG        GetFileSizeFromArchive(PBARCH_INFO pbiInfo, CHAR *szFileName);
BOOL         WriteFile(PBARCH_FILE_LIST pbhEntry, PSZ pszData);

