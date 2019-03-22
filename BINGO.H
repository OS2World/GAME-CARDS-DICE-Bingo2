                             /*-----------------------------------------------*/
#define SHAREWARE   0        /* shareware flag, 0=registered, 1=shareware     */
                             /* change the two values below for shareware     */
#define MAXPOINTERS   10     /* maximum number of pointers                 10 */
#define BINGOGAMES    16     /* minimum must be 2, see fnCreateBingoGames  16 */
#define MINNUMCARDS    1     /* minimum must be 1                           1 */
#define MAXNUMCARDS   10     /* maximum must not be less than MINNUMCARDS  12 */
#define MAXNUMPLAYERS 99     /* maximum number of virtual bingo players       */
#define SHARETIMER    10     /* shareware reminder (minutes)                  */
#define MAXHISCORES    5     /* maximum number of high scores               5 */
                             /*-----------------------------------------------*/

#define STACKSIZE           65536

#define DB_RAISED           0x0400
#define DB_DEPRESSED        0x0800
#define szClientClass       "WC_CLIENT"
#define szBingoClass        "WC_BINGO"
#define szDefaultFont       "Helv"

#define idFrameWindow       100

#define IDD_BINGOCARD       213
#define ID_TIMER            200
#define IDD_START           201
#define IDD_TIMER           204
#define IDD_NEWCARDS        205
#define ID_TIMERSHARE       206
#define IDD_STOP            207
#define ID_BINGOTIMER       208
#define TID_BINGOTIMER      209
#define IDD_STOPPRINT       210
#define IDD_NOBINGO         211
#define IDD_NOMONEY         212

/* Defines for pointers                                                       */

#define IDP_BINGO           220
#define IDP_PENCIL          221
#define IDP_MARKER          222
#define IDP_HAPYFACE        223
#define IDP_HAND            224
#define IDP_SPOT            225
#define IDP_CHECK           226
#define IDP_CLOVER          227
#define IDP_DOLLAR          228
#define IDP_STARTREK        229

/* Defines for bitmaps                                                        */

#define IDBMP_BINGO1        801
#define IDBMP_BINGO2        802
#define IDBMP_BINGO3        803
#define IDBMP_BINGO4        804
#define IDBMP_BINGO5        805
#define IDBMP_X             810
#define IDBMP_Y             811
#define IDBMP_Z             812
#define IDBMP_M             813
#define IDBMP_N             814
#define IDBMP_H             815
#define IDBMP_DIAMOND       816
#define IDBMP_4CORNERS      817
#define IDBMP_CENTER        818
#define IDBMP_INSQUARE      819
#define IDBMP_OUTSQUARE     820
#define IDBMP_ALLNO         821
#define IDBMP_CLASSIC       822
#define IDBMP_T             823
#define IDBMP_LUCKY7        824
#define IDBMP_HARDAWAY      825

/* Defines menu option identifiers (IDM_)                                     */

#define IDM_FILE            3000
#define IDM_START           3001
#define IDM_PAUSE           3002
#define IDM_NEXTNUM         3003
#define IDM_AUTOPLAY        3004
#define IDM_GAMES           3005
#define IDM_SETTINGS        3006
#define IDM_SCORE           3007
#define IDM_OPTIONS         3008
#define IDM_MARKALL         3009
#define IDM_SOUND           3010
#define IDM_STOP            3011
#define IDM_PRINT           3012
#define IDM_BINGO           3013
#define IDM_FONT            3014
#define IDM_CARDS           3015
#define IDM_CHATTER         3016
#define IDM_HISCORES        3017
#define IDM_EXIT            3999
#define IDM_HELP            4000
#define IDM_USINGHELP       4001
#define IDM_HELPPRODINFO    4002

/* Defines string identifiers (IDS_)                                          */

#define IDS_TITLEBAR        1350
#define IDS_BINGOTITLE      1351
#define IDS_HELPTITLE       1352
#define IDS_HELPNAME        1353
#define IDS_EXITPROMPT      1354
#define IDS_BINGO           1355
#define IDS_BINGOTEXT       1356
#define IDS_TIMERBOX        1357
#define IDS_TIMERTEXT       1358
#define IDS_NEWCARDS        1359
#define IDS_NEWCARDSTEXT    1360
#define IDS_START           1361
#define IDS_STARTTEXT       1362
#define IDS_OTHERNAME       1363
#define IDS_HUMAN           1364
#define IDS_PAUSE           1365
#define IDS_PAUSETEXT       1366
#define IDS_PREGAME         1367
#define IDS_INPROGRESS      1368
#define IDS_GAMEOVER        1369
#define IDS_GAME            1370
#define IDS_HELPERROR       1371
#define IDS_NO_HELP         1372
#define IDS_ININAME         1373
#define IDS_STOP            1374
#define IDS_STOPTEXT        1375
#define IDS_SHARETITLE      1376
#define IDS_SHAREREGISTER   1377
#define IDS_SHAREORDER      1378
#define IDS_REGISTERED      1379
#define IDS_SHAREWARE       1380
#define IDS_EXIT            1381
#define IDS_ERROR_PRINT_1   1382
#define IDS_ERROR_PRINT_2   1383
#define IDS_ERROR_PRINT_3   1384
#define IDS_ERROR_PRINT_4   1385
#define IDS_PRINT_STATUS    1386
#define IDS_PRINTERROR      1387
#define IDS_PRINTTEXT       1388
#define IDS_STOPPRINT       1389
#define IDS_AUDIOERROR      1390
#define IDS_AUDIOOPENERR    1391
#define IDS_AUDIOLOADERR    1392
#define IDS_FONTERROR       1393
#define IDS_ERROR_FONT_1    1394
#define IDS_ERROR_MEMORY    1395
#define IDS_NOBINGO         1396
#define IDS_HALLTITLE       1398
#define IDS_PRIZETEXT       1399
#define IDS_CHAT_WAITPLAYER 1400
#define IDS_CHAT_WAITOTHER  1401
#define IDS_CHAT_WAITCLOSE  1402
#define IDS_CHAT_NUMBERLEFT 1403
#define IDS_CHAT_LETTER     1404
#define IDS_BINGOSHOWN      1405
#define IDS_INIHISCORES     1406
#define IDS_VERSION         1407
#define IDS_MONEY           1408
#define IDS_AUDIOPLAYERR    1409
#define IDS_PROFILEERROR    1410
#define IDS_PROFILE_SETTING 1411
#define IDS_PROFILE_HISCORE 1412
#define IDS_NOMONEY         1413
#define IDS_NOMONEYTEXT     1414

#define IDS_X               1501
#define IDS_Y               1502
#define IDS_Z               1503
#define IDS_M               1504
#define IDS_N               1505
#define IDS_H               1506
#define IDS_DIAMOND         1507
#define IDS_4CORNERS        1508
#define IDS_CENTER          1509
#define IDS_INSQUARE        1510
#define IDS_OUTSQUARE       1512
#define IDS_ALLNO           1513
#define IDS_CLASSIC         1514
#define IDS_T               1515
#define IDS_LUCKY7          1516
#define IDS_HARDAWAY        1517

/* Defines help identifiers (IDH_)                                            */

#define IDH_TABLE_MAIN          9999

#define IDH_SUBTABLE_MAIN       9100
#define IDH_MAIN                9110
#define IDH_MENU_FILE           9120
#define IDH_MENU_START          9121
#define IDH_MENU_PAUSE          9122
#define IDH_MENU_NEXTNUM        9127
#define IDH_MENU_AUTOPLAY       9123
#define IDH_MENU_GAMES          9124
#define IDH_MENU_SETTINGS       9125
#define IDH_MENU_SCORE          9128
#define IDH_MENU_EXIT           9126
#define IDH_MENU_STOP           9129
#define IDH_MENU_OPTIONS        9130
#define IDH_MENU_MARKALL        9131
#define IDH_MENU_SOUND          9132
#define IDH_MENU_PRINT          9133
#define IDH_MENU_FONT           9134
#define IDH_MENU_CARDS          9135
#define IDH_MENU_CHATTER        9136
#define IDH_MENU_HISCORES       9137
#define IDH_MENU_HELP           9180
#define IDH_MENU_HELPINDEX      9182
#define IDH_MENU_HELPGENERAL    9183
#define IDH_MENU_HELPUSING      9184
#define IDH_MENU_HELPKEYS       9185
#define IDH_MENU_HELPPRODINFO   9186
#define IDH_HELPKEYS            9187

#define IDH_SUBTABLE_OPTIONS    9200
#define IDH_OPTIONS             9210
#define IDH_OPTIONS_USERNAME    9216
#define IDH_OPTIONS_GAMETYPE    9211
#define IDH_OPTIONS_NUMCARDS    9212
#define IDH_OPTIONS_COMPUTER    9213
#define IDH_OPTIONS_DEFAULTS    9215
#define IDH_OPTIONS_NUMPLAYERS  9217
#define IDH_OPTIONS_HELP        9210

#define IDH_SUBTABLE_SETTINGS   9400
#define IDH_SETTINGS            9410
#define IDH_SETTINGS_COLOR      9411
#define IDH_SETTINGS_DRAWTIME   9412
#define IDH_SETTINGS_MARKALL    9413
#define IDH_SETTINGS_SOUND      9414
#define IDH_SETTINGS_DEFAULTS   9415
#define IDH_SETTINGS_NEWCARDS   9416
#define IDH_SETTINGS_SHOWINIT   9417
#define IDH_SETTINGS_PCSPEAKER  9418
#define IDH_SETTINGS_GAMEPROMPT 9420
#define IDH_SETTINGS_AUDIOCARD  9419
#define IDH_SETTINGS_POINTER    9421
#define IDH_SETTINGS_CHATTER    9422
#define IDH_SETTINGS_AUTOPLAY   9423
#define IDH_SETTINGS_BOARDTYPE  9424
#define IDH_SETTINGS_3DEFFECTS  9425
#define IDH_SETTINGS_HELP       9410

#define IDH_SUBTABLE_BINGOCARD  9345
#define IDH_BINGOCARD           9346
#define IDH_SUBTABLE_START      9300
#define IDH_START               9310
#define IDH_SUBTABLE_STOP       9370
#define IDH_STOP                9380
#define IDH_SUBTABLE_TIMER      9350
#define IDH_TIMER               9360
#define IDH_SUBTABLE_NEWCARDS   9330
#define IDH_NEWCARDS            9340
#define IDH_SUBTABLE_PRINT      9320
#define IDH_PRINT               9321
#define IDH_SUBTABLE_FONT       9325
#define IDH_FONT                9326
#define IDH_SUBTABLE_STOPPRINT  9328
#define IDH_STOPPRINT           9329
#define IDH_SUBTABLE_NOBINGO    9315
#define IDH_NOBINGO             9316
#define IDH_SUBTABLE_CARDS      9390
#define IDH_CARDS               9391
#define IDH_SUBTABLE_CHATTER    9395
#define IDH_CHATTER             9396
#define IDH_SUBTABLE_NOMONEY    9335
#define IDH_NOMONEY             9336

#define IDH_SUBTABLE_SCORE      9500
#define IDH_SCORE               9510
#define IDH_SCORE_RESET         9511
#define IDH_SCORE_HELP          9510

#define IDH_SUBTABLE_HISCORES   9550
#define IDH_HISCORES            9550
#define IDH_HISCORES_RESET      9551
#define IDH_HISCORES_HELP       9550

/* global constants                                                           */

#define szAppName   "BINGO"
#define szSettings  "Settings"
#define szHiScores  "High Scores"
#define szDebugFile "BINGO.DBG"

#define MONEYSIZE    2                 /* For $, this should be 2             */
#define FILESIZE     12
#define SCORESIZE    12
#define TITLESIZE    20
#define TEXTSIZE     80
#define ERRORSIZE    80
#define CHATSIZE     80
#define AUDIOERRSIZE 128
#define BUFFERSIZE   256

#define OFF         0
#define ON          1
#define START       2
#define NO          0
#define YES         1
#define PROMPT      2
#define PC_SPEAKER  0
#define AUDIO_CARD  1
#define CARD_BINGO  0                  /* Card status, used in pcd->status    */
#define CARD_NONE   1
#define CARD_CHECK  2
#define CARD_CREATE 3
#define FOUND_BINGO 0
#define NO_BINGO    99
#define TEST        99                 /* used in IDM_BINGO - fnDisplayBingo  */
#define NOT_FOUND   99
#define CARD_COST   1                  /* $1 per card per game                */
#define MONEY_START 100                /* Starting money amount               */
#define VERTICAL    0
#define HORIZONTAL  1                  /* default                             */

#define PREGAME    0                   /* Used by bingostatus global variable */
#define INPROGRESS 1
#define GAMEOVER   2
#define BINGOSHOWN 3
#define PRINTING   4

#define SOUND_ERROR    0               /* used by fnSound                     */
#define SOUND_BINGO    1
#define SOUND_DRAWNUM  2
#define SOUND_CHIP     3
#define SOUND_NEWCARDS 4
#define SOUND_NEWGAME  5
#define SOUND_REGISTER 6
#define SOUND_QUIT     7
#define SOUND_EXIT     8
#define SOUND_WELCOME  9
#define SOUND_NOBINGO  10
#define SOUND_NUMBER   11
#define SOUND_LETTER   12
#define SOUND_WAIT     13
#define SOUND_CLOSE    14
#define OTHER_WON      20
#define PLAYER_WON     21

#define NO_CHIP        0
#define NORMAL_CHIP    1
#define O_CHIP         2
#define WIN_CHIP       3

#define CHAT_WAIT      1               /* Define chatter types                */
#define CHAT_NUMBER    2
#define CHAT_BINGO     3
#define CHAT_LETTER    4
#define CHAT_WAITCLOSE 5

#define PLAYER         1               /* Who owns this bingo card, you       */
#define OTHER          2               /* Other bingo hall players            */

#define TID_LOGOTIMER   TID_USERMAX - 1
#define UM_CREATECARDS  WM_USER + 1    /* user defined application PM messages*/
#define UM_NEWGAME      WM_USER + 2
#define UM_AUTOPLAY     WM_USER + 3
#define UM_GAMEOVER     WM_USER + 4
#define UM_GAMETYPE     WM_USER + 5
#define UM_MISSMARK     WM_USER + 6
#define UM_INITDLG      WM_USER + 7
#define UM_POINTER      WM_USER + 8
#define UM_STATUS       WM_USER + 9
#define UM_PRINT        WM_USER + 10
#define UM_STOPPRINT    WM_USER + 11
#define UM_CHAT         WM_USER + 12
#define UM_NUMBER       WM_USER + 13
#define UM_CREATECHAT   WM_USER + 14
#define UM_SOUND        WM_USER + 15

/* structure definitions                                                      */

typedef struct
{
  CHAR name[FACESIZE];
}FONTS;
typedef FONTS * PFONTS;

typedef struct                         /* Bingo card information              */
{
  ULONG    bingono[5][5];              /* Number in bingo square, [2][2] free */
  ULONG    bingoit[5][5];              /* Marked status of bingo square (0 .. */
  LONG     xline[6];                   /* Horizontal and vertical lines of    */
  LONG     yline[6];                   /* bingo card, for drawing purposes    */
  ULONG    status;                     /* Status of bingo card (BINGO or NONE */
  LONG     waitnumber;                 /* Waiting number                      */
  LONG     whichPlayer;                /* PLAYER, COMPUTER or OTHER           */
  HPOINTER hptr;                       /* Handle to mouse pointer             */
} CARDDATA;
typedef CARDDATA * PCARDDATA;

typedef struct                         /* Linked list of bingo card (up to 12)*/
{
  HWND      hwndCard;
  HWND      hwndClient;
  PCARDDATA pcd;
  PVOID     nextcard;
  PVOID     priorcard;
} CARDLIST;
typedef CARDLIST * PCARDLIST;

typedef struct                         /* Bingo board information             */
{
  LONG      board[15][5];              /* Board number, if 0 number is drawn  */
  CHAR      letter[6];                 /* Bingo board letter, B, I, N, G, O   */
  LONG      xboard[16];                /* Horizontal and vertical line        */
  LONG      yboard[6];                 /* of bingo board lines, for drawing   */
  BOOL      bingogames[BINGOGAMES][5][5];
  CHAR      gametypes[BINGOGAMES][TITLESIZE+1];
  LONG      bitmaps[BINGOGAMES];       /* Bitmap values (IDB_                 */
  HPOINTER  hptr[MAXPOINTERS];         /* Handle to mouse pointers            */

  ULONG     lcid;                      /* Font support                        */
  ULONG     totalFonts;
  CHAR      selFont[FACESIZE];
  FATTRS    fAttr;
  PFONTS    pFontnames;
                                       /* Print support                       */
  HWND      hwndPrint;
  PPRQINFO3 prq;
  PDRIVDATA pDriverData;
  TID       threadID;

  HINI        hini;                    /* profile initialization file handle  */
} BOARDDATA;
typedef BOARDDATA * PBOARDDATA;

typedef struct                         /* Bingo score information             */
{
  LONG  gamesplayed;
  LONG  cardsplayed;
  LONG  bingos;
  LONG  moneywon;
  LONG  moneyleft;
  LONG  othercardsplayed;
  LONG  otherbingos;
  LONG  othermoneywon;
} SCOREDATA;
typedef SCOREDATA * PSCOREDATA;

typedef struct                         /* Profile (INI) information           */
{
  BOOL  bool1;
  BOOL  bool2;
  BOOL  bool3;
  BOOL  bool4;
  BOOL  bool5;
  BOOL  bool6;
  BOOL  bool7;
  BOOL  bool8;
  BOOL  bool9;
  BOOL  bool10;
  ULONG ulong1;
  ULONG ulong2;
  ULONG ulong3;
  ULONG ulong4;
  ULONG ulong5;
  ULONG ulong6;
  ULONG ulong7;
  ULONG ulong8;
  ULONG ulong9;
  ULONG ulong10;
  CHAR  char20[20];
  CHAR  char40[40];
  CHAR  char80[80];
} PROFILEINFO;
typedef PROFILEINFO * PPROFILEINFO;

typedef struct                         /* High Scores (INI) information       */
{
  HINI  hini;
  CHAR  player[MAXHISCORES][TITLESIZE+1];
  LONG  score[MAXHISCORES];
} HISCORESDATA;
typedef HISCORESDATA * PHISCORESDATA;

typedef struct
{
  USHORT option;
  ULONG  letter;
  ULONG  number;
} SND;
typedef SND * PSND;

/* Function prototypes                                                        */

MRESULT EXPENTRY wpClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpClientWndProc, system)

MRESULT EXPENTRY wpBingoWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpBingoWndProc, system)

MRESULT EXPENTRY wpOptionsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpOptionsDlgProc, system)

MRESULT EXPENTRY wpSettingsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpSettingsDlgProc, system)

MRESULT EXPENTRY wpScoreDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpScoreDlgProc, system)

MRESULT EXPENTRY wpPrintDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpPrintDlgProc, system)

MRESULT EXPENTRY wpPrtStatDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpPrtStatDlgProc, system)

MRESULT EXPENTRY wpFontDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpFontDlgProc, system)

MRESULT EXPENTRY wpCardDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpCardDlgProc, system)

MRESULT EXPENTRY wpChatDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpChatDlgProc, system)

MRESULT EXPENTRY wpBingoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpBingoDlgProc, system)

MRESULT EXPENTRY wpHiScoresDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpHiScoresDlgProc, system)

MRESULT EXPENTRY wpAboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#pragma linkage (wpAboutDlgProc, system)

VOID  fnSetDefaultOptions( VOID );
VOID  fnSetDefaultSettings( VOID );
VOID  fnOptionsDlgInit( HWND hwnd );
VOID  fnSettingsDlgInit( HWND hwnd );
VOID  fnInitializeScores( VOID );
VOID  fnResetHiScores( VOID );

VOID  fnLoadAudioModules( VOID );
VOID  fnFreeAudioModules( VOID );
VOID  fnAudioCardInit( HWND hwnd );
VOID  fnAudioCardClose( VOID );
VOID  fnSoundThread( VOID );
VOID  fnSound( USHORT option, ULONG letter, ULONG number );
LONG  fnSoundProcess( PSND pSoundData );

VOID  fnDrawCard( HPS hps, PCARDDATA pcd, LONG left, LONG bottom,
                  LONG width, LONG height, BOOL type );
VOID  fnDrawMarker( HPS hps, PCARDDATA pcd, LONG x, LONG y, ULONG chipType );

VOID  fnToUpperString( CHAR *string );

/* global variables                                                           */

LONG   bingonumber;                 /* current number drawn and its           */
LONG   numbersdrawn;                /* count of numbers drawn so far in game  */
USHORT bingoletter;                 /* corresponding letter                   */
USHORT bingostatus;                 /* status of game (PREGAME, INPROGRESS,etc*/

BOOL   autoplay;                    /* automated play (1) or manual play (0)  */
ULONG  numberofcards;               /* number of cards (1-12)                 */
ULONG  numberofplayers;             /* number of other players (0-100)        */
ULONG  prizemoney;                  /* prize money                            */
ULONG  gametype;                    /* gametype (0-BINGOGAMES)                */
ULONG  drawtime;                    /* draw time interval (milliseconds)      */
BOOL   markall;                     /* mark all (0=OFF, 1=ON)                 */
BOOL   sound;                       /* sound (0=OFF, 1=ON)                    */
BOOL   chatter;                     /* chatter (0=OFF, 1=ON)                  */
BOOL   audiotype;                   /* audiotype (0=PC speaker, 1=Audio Card) */
ULONG  newcards;                    /* new cards option (1,2,3)               */
ULONG  chipcolor;                   /* bingo chip color (0-15)                */
BOOL   showstart;                   /* show options at start (0=NO, 1=YES)    */
BOOL   gameprompt;                  /* prompt for new game (0=NO, 1=YES)      */
BOOL   boardtype;                   /* board orientation (0=VERT, 1=HORIZONTAL*/
ULONG  pointer;                     /* mouse pointer (0-5)                    */
BOOL   effects3d;                   /* 3D effects (0=OFF, 1=ON)               */
USHORT audioDevId;                  /* audio device id                        */

PBOARDDATA    pbd;                  /* pointer to bingo board data struct     */
PSCOREDATA    psd;                  /* pointer to score data struct           */
PHISCORESDATA phsd;                 /* pointer to high scores data struct     */
PCARDLIST     start, last;          /* start and end address of linked list   */
PCARDLIST     startOther, lastOther;/* start and end address of linked list   */

HAB         hab;                    /* application anchor block handle        */
HWND        hwndBoard;              /* window handle of bingo board client    */
HWND        hwndChat;               /* window handle of Chatter dialog window */
HWND        hwndHelp;               /* window handle of help                  */

CHAR szTitle[TITLESIZE+1];          /* globally used title and text strings   */
CHAR szText[TEXTSIZE+1];
CHAR szName[TITLESIZE+1];           /* player name (default found IDS_)       */
CHAR szMoney[MONEYSIZE+1];

APIRET  os2mm;                      /* 0 if MMPM/2 not found, 1 is available  */
APIRET  os2wave;                    /* 0 if BINGOWAV.DLL not found, 1 found   */

FILE *wptrDebug;                    /* File pointer to debug file             */
BOOL debug;                         /* Debug flag (0=OFF, 1=ON)               */

BOOL networkPlay;                   /* Network play flag (0=OFF, 1=ON)        */

HMODULE hModMDM;                    /* handle of MMPM/2 DLL module            */
HMODULE hModMMIO;                   /* handle of MMPM/2 DLL module            */
HMODULE hModWAVE;                   /* handle of BINGOWAV.DLL module          */
PFN     mciSendCmd;                 /* address of MMPM/2 function             */
PFN     mciGetError;                /* address of MMPM/2 function             */
PFN     ioOpen;                     /* address of MMPM/2 function             */
PFN     ioClose;                    /* address of MMPM/2 function             */
HMMIO   hmmioWave[TOTAL_WAVES];
