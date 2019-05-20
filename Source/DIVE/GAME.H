#define WEAPON_NOVA_BOMB       1
#define WEAPON_RAPTOR_MISSLE   2
#define WEAPON_SMART_MISSLE    3
#define WEAPON_DUMB_MISSLE     4
#define WEAPON_PLASMA          5
#define WEAPON_CARPET_BOMB     6

BOOL LoadSprites(VOID);
BOOL StartGameThread( VOID );

BOOL CalibrateJoystick( HFILE hGame );
HFILE InitJoy( VOID );
