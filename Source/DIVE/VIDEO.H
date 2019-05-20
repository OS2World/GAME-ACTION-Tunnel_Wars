VOID APIENTRY SpriteMove ( ULONG parm1 );
BOOL StartDiving( HWND hWndFrame, HWND hWndClient, HAB hAB, HMQ hMQ, PTID pTid);
BOOL SetupBlitter(HWND hWndDlg, HDIVE hDive, BOOL bVRNDisabled);
BOOL SetupDiveBuffer( VOID );
BOOL CopyOpSprite   ( BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, LONG lY, LONG lX );
BOOL CopySprite     ( BMPDATA *pbmpSprite,  BMPDATA *pbmpScene, LONG lY, LONG lX );

