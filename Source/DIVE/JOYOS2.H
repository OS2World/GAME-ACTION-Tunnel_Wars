
#ifndef JOYOS2_H
#define JOYOS2_H

/****************************************************************************/
#define GAME_VERSION    0x02b       /* 0.2b */
#define GAMEPDDNAME "GAME$   "
/****************************************************************************/

/****************************************************************************/
#define IOCTL_CAT_USER          0x80
#define GAME_GET_VERSION        0x01
#define GAME_GET_PARMS          0x02
#define GAME_SET_PARMS          0x03
#define GAME_GET_CALIB          0x04
#define GAME_SET_CALIB          0x05
#define GAME_GET_DIGSET         0x06
#define GAME_SET_DIGSET         0x07
#define GAME_GET_STATUS         0x10
#define GAME_GET_STATUS_BUTWAIT     0x11
#define GAME_GET_STATUS_SAMPWAIT    0x12
/****************************************************************************/

/****************************************************************************/
#define JOY_AX_BIT  0x01
#define JOY_AY_BIT  0x02
#define JOY_A_BITS  (JOY_AX_BIT|JOY_AY_BIT)
#define JOY_BX_BIT  0x04
#define JOY_BY_BIT  0x08
#define JOY_B_BITS  (JOY_BX_BIT|JOY_BY_BIT)
#define JOY_ALLPOS_BITS (JOY_A_BITS|JOY_B_BITS)

#define JOY_BUT1_BIT    0x10
#define JOY_BUT2_BIT    0x20
#define JOY_BUT3_BIT    0x40
#define JOY_BUT4_BIT    0x80
#define JOY_ALL_BUTS    (JOY_BUT1_BIT|JOY_BUT2_BIT|JOY_BUT3_BIT|JOY_BUT4_BIT)
/****************************************************************************/

/****************************************************************************/
typedef SHORT   GAME_POS;   /* some data formats require signed values */

typedef struct
{
    GAME_POS        x;
    GAME_POS        y;
}
GAME_2DPOS_STRUCT;

typedef struct
{
    GAME_2DPOS_STRUCT   A;
    GAME_2DPOS_STRUCT   B;
    USHORT          butMask;
}
GAME_DATA_STRUCT;

typedef struct
{
    GAME_POS        lower;
    GAME_POS        centre;
    GAME_POS        upper;
}
GAME_3POS_STRUCT;

typedef struct
{
    GAME_3POS_STRUCT    Ax;
    GAME_3POS_STRUCT    Ay;
    GAME_3POS_STRUCT    Bx;
    GAME_3POS_STRUCT    By;
}
GAME_DIGSET_STRUCT;
/****************************************************************************/

/****************************************************************************/
typedef struct
{
    GAME_DATA_STRUCT    curdata;
    USHORT          b1cnt;
    USHORT          b2cnt;
    USHORT          b3cnt;
    USHORT          b4cnt;
}
GAME_STATUS_STRUCT;

/* in use bitmasks originating in 0.2b */
#define GAME_USE_BOTH_OLDMASK   0x01    /* for backward compat with bool */
#define GAME_USE_X_NEWMASK  0x02
#define GAME_USE_Y_NEWMASK  0x04
#define GAME_USE_X_EITHERMASK   (GAME_USE_X_NEWMASK|GAME_USE_BOTH_OLDMASK)
#define GAME_USE_Y_EITHERMASK   (GAME_USE_Y_NEWMASK|GAME_USE_BOTH_OLDMASK)
#define GAME_USE_BOTH_NEWMASK   (GAME_USE_X_NEWMASK|GAME_USE_Y_NEWMASK)

/* only timed sampling implemented in version 0.2a */
#define GAME_MODE_TIMED     1   /* timed sampling */
#define GAME_MODE_REQUEST   2   /* request driven sampling */

/* only raw implemented in version 0.2a */
#define GAME_DATA_FORMAT_RAW    1   /* [l,c,r]   */
#define GAME_DATA_FORMAT_SIGNED 2   /* [-l,0,+r] */
#define GAME_DATA_FORMAT_BINARY 3   /* {-1,0,+1} */
#define GAME_DATA_FORMAT_SCALED 4   /* [-10,+10] */

typedef struct
{
    USHORT          useA;       /* new bitmasks: see above */
    USHORT          useB;
    USHORT          mode;       /* see consts above */
    USHORT          format;     /* see consts above */
    USHORT          sampDiv;    /* samp freq = 32 / n */
    USHORT          scale;      /* scaling factor */
    USHORT          res1;       /* must be 0 */
    USHORT          res2;       /* must be 0 */
}
GAME_PARM_STRUCT;

typedef struct
{
    GAME_3POS_STRUCT    Ax;
    GAME_3POS_STRUCT    Ay;
    GAME_3POS_STRUCT    Bx;
    GAME_3POS_STRUCT    By;
}
GAME_CALIB_STRUCT;
/****************************************************************************/

#endif
