{-----------------------}
{ MainMenu              }
{-----------------------}
PROCEDURE MainMenu                  ;

CONST
{------------------------------------------------------------------------------}
{Edit Buffer Keys MidiConfig Active Filter Options Ports Help Disk Various Quit}
{2    7      14   19         30     37     44      52    58   63   68      76  }
{------------------------------------------------------------------------------}
B = 'Ini[.]Cde[.]Syx[.]Flt[.]RSt[.]Bff[.]PortIn[..]Out[....]B[     ]O[     ]S[     ]';

T : TType=( 'Edit','Buffer','Keys','MidiConfig','Active','Filter',
            'Options','Ports','Help','Disk','Various','Quit' )   ;

  SubT: ARRAY [0..11] OF SubTType = ( { Str16 }
 ('*EDITOR:','Edit Buffer','Clear-Fill','Data CheckSum','XSys CheckSum',
  'BlankChr[ ][   ]','FillChr [ ][   ]','Search','Replace',''),
 ('*BUFFER DATA:','Decimal Dump','Ranges','EnableBuff[NOS]','SetCurBuff[NOS]',
  'ClearBuff [NOS]','','','',''),
 ('*PROG KEYS:','1DiskLink','2BufferLink','3Startup','4Remote','','','','',''),
 ('Edit Config','Keyboard Split','Controllers','Patch Define',
  '*CFG FILES:','Save Config','Load Config','','',''),
 ('*MIDI TROUGH:','Start Run','Buffer Position','File Input[ ]',
  'Port Input[*]','','','','',''),
 ('*FILTERS SETUP:','Configure','','','','','','','',''),
 ('BufferStorage   ','SystemExclusive ','CodeDisplay     ','FilterEnable    ',
  'PreInitialise   ','RunningStatus   ','TimeInfoStorage ','AutoBufferClear ',
  '                ','*[ENTER]=Toggle '),
 ('*HW PORTS:  ','Input  [..] ','Output [....]','','','','','','',''),
 ('*HELP MENU:','1Hardware Config','2Main Menu','3Buffer Display','4Active-Run',
  '5Filter Config','6Editor','7Prog Keys','8Midi Config','9KeySplit'),
 ('*FILES:','6Buffer Data','5Patch Translate','7Directory','8Configuration',
  '*PROG KEYS:','1Buffer-Linked','2File-Linked','3Remote Keybrd','4All ProgKeys'),
 ('*POLLING:','1Code Xmit','2Receiver','3Buff Xmit','','','','','',''),
 ('DOS','ESC','','','','','','','','')  ) ;

TPos   : TPosType = ( 2 , 7 , 14, 19, 30, 37, 44, 52, 58, 63, 68, 76 ) ;
TChr   : TChrType = ('E','B','K','M','A','F','O','P','H','D','V','Q' ) ;

{                 'E''B''K''M''A''F''O''P''H''D''V''Q'                 }
NumOp  : TPosType=(9, 6, 5 ,7 ,5 ,2 ,10,3 ,10,10,4 ,2  );{ # SubOptions}
SizWinX: TPosType=(17,17,14,16,16,14,24,25,20,15,10,3  );{ X Width     }
SizWinY: TPosType=(18,18,7 ,9 ,7 ,4 ,12,12,14,12,6 ,3  );{ Y Width     }

VAR
  OpenWin, Quit    : BOOLEAN ;
  Choice1, Choice2 : CHAR    ;
  SubOp            : CHAR    ;
  X, Y, n          : INTEGER ;
{-------------------------------}
PROCEDURE DoSubOp ( VAR C:CHAR) ;
{-------------------------------}
BEGIN
     CASE Choice1 OF
             'Q' : IF (SubOp='E') THEN C:=#27 ELSE Quit:=TRUE   ;
     'O','D','P' : DoOptions (SubOp)    ; { Misc.Options + Files}
         'E','K' : Edit (SubOp)         ; { Editor + Prog Keys  }
             'H' : DoHelp (SubOp)       ; { Help Pages          }
             'F' : DoFilter             ; { Midi Filter Setup   }
             'M' : ChannelMatrix (SubOp); { Channel Config.     }
             'A' : DoReadWrite (SubOp)  ; { Activate Run        }
             'B' : DoBufferDump (SubOp) ; { Buffer Dump         }
             'V' : CASE SubOp OF          { Various             }
                   '1': Do8250          ; { 8250 Byte Send      }
                   '2': Do8250Read      ; { 8250 UART Read      }
                   '3': SendBuffer      ; { Buffer Send         }
                   END                  ;
     END                                ;
END                                     ;
{----------------------------------------------------------}
PROCEDURE WriteBottom                                      ;
{----------------------------------------------------------}
PROCEDURE WriteBuff (B: BuffType; Xx: INTEGER; Sel:BOOLEAN);
CONST ColArr : ARRAY [0..3] OF BYTE = (0,15,3,11)          ;
BEGIN
  TEXTCOLOR (ColArr [BYTE(Sel)+BYTE(B.Num=CurrentBuff)*2]) ;
  GOTOXY (Xx , 1 )                                         ;
  WRITE  (B.Pos:5 )                                        ;
END                                                        ;
BEGIN
   DoWindow (LightGray,Blue,1,25,80,25,1,0) ;
   WRITE (B)                              ;
   TEXTCOLOR (Black)                      ;
   GOTOXY (5 , 1) ; WRITE (OnOff1[Q.InitStrEnable]    ) ;
   GOTOXY (11, 1) ; WRITE (OnOff1[Q.CodeOption]       ) ;
   GOTOXY (17, 1) ; WRITE (OnOff1[Q.SysExOption]      ) ;
   GOTOXY (23, 1) ; WRITE (OnOff1[Q.FilterEnable]     ) ;
   GOTOXY (29, 1) ; WRITE (OnOff1[Q.RStatEnable]      ) ;
   GOTOXY (35, 1) ; WRITE (OnOff1[Q.BuffOption]       ) ;
   TEXTCOLOR (LightRed)                               ;
   GOTOXY (44, 1) ; WRITE (PC[C[1].PortIn]          ) ;
   GOTOXY (51, 1) ; WritePortOut (C[1].SysPortOut,'') ;
   WriteBuff ( NBuff, 58, BuffN)                  ;
   WriteBuff ( OBuff, 66, BuffO)                  ;
   WriteBuff ( SBuff, 74, BuffS)                  ;
   TEXTCOLOR (Blue)                               ;
END                                               ;
{-------------------------------------------------}
PROCEDURE SelectColor ( Yy: INTEGER; CChar: CHAR) ;
{-------------------------------------------------}
BEGIN
  IF (Yy=Y) THEN
  BEGIN
    IF CChar<>#13 THEN TEXTATTR := CyanBlack
                  ELSE TEXTATTR := CyanWhite  ;
  END ELSE TEXTATTR := LightGrayBlue          ;
END                                           ;
{------------------------------------------------------}
PROCEDURE UpDateOptionsWin ( Y: INTEGER;  B:CHAR )     ;
{------------------------------------------------------}
VAR  N : INTEGER                                       ;

PROCEDURE WriteOp (Op: BYTE) ;
BEGIN
  SelectColor (N, B)         ;
  GOTOXY (19, N)             ;
  WRITE ('[',OnOff[Op],']')  ;
  INC (N)                    ;
END                          ;
BEGIN
  N := 1                    ;
  WriteOp (Q.BuffOption   ) ;
  WriteOp (Q.SysExOption  ) ;
  WriteOp (Q.CodeOption   ) ;
  WriteOp (Q.FilterEnable ) ;
  WriteOp (Q.InitStrEnable) ;
  WriteOp (Q.RStatEnable  ) ;
  WriteOp (Q.TimeEnable   ) ;
  WriteOp (Q.AutoClr      ) ;
END                         ;
{------------------------------------------------------}
PROCEDURE UpDateHWWin ( Y: INTEGER;  B:CHAR )          ;
{------------------------------------------------------}
VAR  N : INTEGER                                       ;

PROCEDURE WriteOp  ;
BEGIN
  SelectColor (N, B)                                ;
  GOTOXY (10, N)                                    ;
  TEXTCOLOR (LightRed)                              ;
  IF N=2 THEN WRITE (PC[C[1].PortIn])
         ELSE WritePortOut (C[1].SysPortOut,'')     ;
  INC (N)                                           ;
END                                                 ;
BEGIN
    N := 2            ;
    WritePortCfg (5)  ;
    WriteOp           ;
    WriteOp           ;
END                   ;
{------------------------------------------------------}
PROCEDURE UpDateEditWin (Y:INTEGER; C:CHAR )           ;
{------------------------------------------------------}
CONST
    S2 = '³BUFFERS STATUS : ³' + NL +
         '³   Size ENA CURR ³' + NL +
         '³N:60000 Yes *    ³' + NL +
         '³O:               ³' + NL +
         '³S:               ³' + NL ;

PROCEDURE WriteB (B:BuffType; Z:INTEGER; O:BOOLEAN)     ;
VAR BStr      : Str10                                   ;
BEGIN
  IF (CurrentBuff=B.Num) THEN BStr:=' *' ELSE BStr:='  ';
  BStr := ':' + OnOff[BYTE(O)] + BStr                   ;
  GOTOXY (4,Z) ; WRITE ( B.Max:5, BStr )                ;
END                                                     ;
PROCEDURE WriteC (B:BYTE; L:INTEGER) ;
BEGIN
  SelectColor (L, C)                 ;
  GOTOXY (11,L) ; WRITE ( CHR(B) )   ;
  GOTOXY (14,L) ; WRITE ( B:3    )   ;
END                                  ;
BEGIN
  TEXTATTR := LightGrayBlue            ;
  GOTOXY (1,11)                        ;
  WRITE ( StrL(19,0),NL,S2,StrL(19,1)) ;

  TEXTCOLOR (Black)              ;
  WriteB ( NBuff ,14, BuffN) ;
  WriteB ( OBuff ,15, BuffO) ;
  WriteB ( SBuff ,16, BuffS) ;
  IF X=0 THEN
  BEGIN
    WriteC (Q.BlankChar, 6) ;
    WriteC (Q.FillChar,  7) ;
  END                       ;
END                         ;
{---------------------------------------------------------------------}
FUNCTION FindTPos ( C: CHAR; TChr: TChrType; Num: INTEGER ) : INTEGER ;
{---------------------------------------------------------------------}
VAR i : INTEGER    ;
BEGIN
  FindTPos := Num+1                 ;
  FOR i := 0 TO Num DO
  IF C = TChr[i] THEN FindTPos := i ;
END                                 ;
{---------------------------------------------------------------------------}
PROCEDURE WriteTop ( T: TType; TPos: TPosType; TChr: TChrType;
                     Num: INTEGER; n: INTEGER )              ;
{---------------------------------------------------------------------------}
VAR i : INTEGER             ;
BEGIN
   X := n                                         ;
   DoWindow (LightGray, Blue, 1, 1, 80, 1, 1,0)   ;
   FOR i := 0 TO Num DO BEGIN
       GOTOXY (TPos[i], 1)       ;
       WRITE (T[i])              ;
       GotoColorStr (Black,TPos[i],1,TChr[i]) ;
       TEXTATTR := BlackYellow   ;
       GOTOXY (TPos[X], 1)       ;
       WRITE (T[X])              ;
       TEXTATTR := LightGrayBlue ;
   END                           ;
END                              ;
{-----------------------------}
PROCEDURE LogoTopBottom       ;
{-----------------------------}
CONST
  S1 = '³            Ü    Ü Ü       Ü SoftByte ³'+NL+
       '³   ÛÛßßÛßßÛ Ü ÜÜÜÛ Ü Ü   Ü Ü ÜÜÜ ÜÜÜ  ³'+NL+
       '³   ÛÛ  Û  Û Û ÛÜÜÛ Û Û Ü Û Û ÛÜÜ ÛÜÜ  ³'+NL+
       '³   ÛÛ  Û  ß          ßßßßß   ÜÜÛ ÛÜÜ  ³'+NL+
       '³   ÛÛ    MIDI CONTROLLER 1.1          ³'+NL ;
BEGIN
   DoWindow (Blue     ,Yellow,1, 1,80,25,1,0) ;
   DoWindow (Blue     ,Yellow,10,7,49,13,1,1) ;
   DoWindow (Blue     ,Yellow,10,7,51,13,0,0) ;
   WRITE ( StrL(40,0),NL,S1,StrL(40,1))       ;
   WriteBottom                                ;
   WriteTop ( T, TPos, TChr, 11, X )          ;
END                                           ;

{--------------------------------}
PROCEDURE AdjustY (NOp:INTEGER)  ;
{--------------------------------}
BEGIN
  IF (Y>NOp) THEN Y := 1         ;
  IF (Y<1)   THEN Y := NOp       ;
END                              ;
{-----------------------------------------------------}
PROCEDURE MoveCur ( VAR C   : CHAR;    Num: INTEGER   ;
                    VAR X, Y: INTEGER; NOp: INTEGER ) ;
{-----------------------------------------------------}
VAR D : CHAR                              ;
BEGIN
  C := 'Q'                          ;
  D := READKEY                      ;
  CASE D OF
       ArR: INC (X)                 ;
       ArL: DEC (X)                 ;
       ArD: BEGIN
              INC (Y)               ;
              C := 'D'              ;
            END                     ;
       ArU: BEGIN
              DEC (Y)               ;
              C := 'U'              ;
            END                     ;
       ELSE C := ' '                ;
  END                               ;

  AdjustY (NOp)                     ;
  IF X<0   THEN X:=Num                    ;
  IF X>Num THEN X:= 0                     ;
END                                       ;
{-------------------------------------------------------------------}
FUNCTION SubWindow (S:SubTType; NOp,XStart,SWinX,SWinY:INTEGER):CHAR;
{-------------------------------------------------------------------}
VAR i, n         : INTEGER                                          ;
    C            : CHAR                                             ;
    B            : Str20                                            ;

CONST
    Clr  : STRING[26] = '                          '                ;
    As   = '*'                                                      ;

PROCEDURE XYWindow ;
BEGIN
  DoWindow (LightGray,Blue,XStart-1,2,XStart+SWinX  ,SWinY  ,1,2) ;
  DoWindow (LightGray,Blue,XStart-1,2,XStart+SWinX+1,SWinY  ,0,0) ;
END                                                               ;
PROCEDURE WriteB ( YPos:INTEGER; A:Str10 )             ;
BEGIN
  IF S[YPos][1] IN ['0'..'9',As] THEN n:=2 ELSE n:=1   ;
  SelectColor (YPos+1, C)                              ;
  IF (S[YPos][1]=As) THEN TEXTATTR := LightGrayBlack   ;
  GOTOXY (1, YPos+1)                                   ;
  WRITE (' ',Clr)                                      ;
  GOTOXY (1, YPos+1)                                   ;
  B := COPY (S[YPos], n, LENGTH(S[YPos])-n+1)          ;
  WRITE (' ', B,A)                                     ;
END                                                    ;

BEGIN
  Clr[0] := CHR (SWinX+1)      ;  { Clear Black Bar String }
  Y := 1                       ;
  C := 'D'                     ;

  XYWindow                     ;

  REPEAT
    Choice1:= TChr[X]                ;    { Main Menu Option: E-Q }
    TEXTATTR := LightGrayBlue        ;

    FOR i := 0 TO NOp-2 DO WriteB (i, NL) ;
    WriteB (i+1,'')                       ;

    IF (S[Y-1][1]=As) THEN BEGIN
                             IF (C='D') THEN INC (Y) ;
                             IF (C='U') THEN DEC (Y) ;
                           END                       ;
    AdjustY (NOp)                                    ;

    WriteB (Y-1,'')                           ;

    CASE X OF
    0,1: UpDateEditWin (Y, C)        ;
      6: UpDateOptionsWin (Y, C)     ;
      7: UpDateHWWin (Y, C)          ;
      8: BEGIN
           GOTOXY (1,12)             ;
           WRITE ('[',TimerCnt:6,']');
         END                         ;
    END                              ;

    GOTOXY (1,Y)                   ;

    C := UPCASE(READKEY)                           ;
    CASE C OF
          #00 : MoveCur ( C, 11, X, Y, NOp )       ;
          #27 : OpenWin := FALSE                   ;
          #13 : BEGIN
                  IF (X=7) THEN
                  BEGIN
                    WriteB (Y-1,'[A-D]:')          ;
                    UpDateHWWin (Y, C)             ;
                    GOTOXY (21, Y)                 ;
                  END                              ;
                  SubOp := S[Y-1][1]               ;
                  DoSubOp (C)                      ;
                  LogoTopBottom                    ;
                  XYWindow                         ;
                END                                ;
           ELSE BEGIN
                  n := FindTPos ( C, TChr, 11 )       ;
                  IF n<12 THEN
                  BEGIN
                    WriteTop ( T, TPos, TChr, 11, n ) ;
                    C := 'Q'                          ;
                  END                                 ;
                END                                   ;
        END                                           ;
  UNTIL (C='Q') OR (C=#27) OR Quit   ;
END                                  ;
{-------------------------------------------}
BEGIN                      { Main Menu }
 X := 3                          ;
 Y := 1                          ;
 Quit    := FALSE                ;
 OpenWin := FALSE                ;

 REPEAT
   LogoTopBottom                 ;
   Choice1 := UPCASE (READKEY)   ;

   CASE Choice1 OF
   #00 : MoveCur ( Choice2, 11, X, Y, 1) ; { Arrow L, R, U, D }
   #27 : Quit := TRUE                    ;
   #13 : BEGIN
            OpenWin := TRUE              ;
            REPEAT
              LogoTopBottom              ;
              SubOp  := SubWindow
             (SubT[X], NumOp[X], TPos[X], SizWinX[X], SizWinY[X] ) ;
            UNTIL NOT OpenWin OR Quit                              ;
          END                                                      ;
   ELSE BEGIN
          n := FindTPos ( Choice1, TChr, 11 )             ;
          IF n<12 THEN WriteTop ( T, TPos, TChr, 11, n )  ;
        END                                               ;
   END                                                    ;
 UNTIL Quit                                               ;
END                                                       ;
