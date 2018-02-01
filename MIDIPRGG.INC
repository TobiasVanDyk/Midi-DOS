{-----------------------------------------}
{ Display Buffer Dump Info                }
{ Select Buffer                           }
{-----------------------------------------}
PROCEDURE DoBufferDump (SubOp: CHAR)      ;
CONST
S1=   ' Number +OOO OO1 OO2 OO3 OO4 OO5 OO6 OO7 OO8 OO9 O1O O11 O12 O13 O14 O15';
S2=   ' Buffer [NOS]:? '+ BS      ;
S3=NL+' No Buffer(s) selected...' ;
S7=   '[N][O][S]'                 ;
S8 : Str3 = '[.]'                 ;
BS9=  BS+BS+BS+BS+BS+BS+BS+BS+BS  ;

VAR
  i, n, p                  : WORD    ;
  X, Y                     : CHAR    ;
  Imax,JMax                : WORD    ;
  Start                    : WORD    ;
  Color                    : INTEGER ;
  TimeCnt                  : INTEGER ;
{-----------------------------------------------------}
PROCEDURE UpDateBuff (BOn:BOOLEAN;B:BuffType;CB:BYTE) ;
{-----------------------------------------------------}
BEGIN
  IF BOn THEN
  BEGIN
    CurrentBuff := CB                           ;
    XBuff := B                                  ;
    p     := 0                                  ;
    S8[2] := BuffChr1[CB]                       ;
    DoWindow (LightGray, Blue, 1,25,80,25,0,0)  ;
    GotoColorStr (Blue    ,53,1,S7)             ;
    GotoColorStr (LightRed,53+(CB-1)*3,1,S8)    ;
    DoWindow (LightGray, Black, 3,4,76,20, 0,0) ;
  END ELSE p := p - 256                         ;
END                                             ;
{------------- MAIN ----------------------------}
BEGIN
  IF SubOp IN ['E','S','C'] THEN
  BEGIN
    TEXTATTR := CyanGreen                         ;
    i := 4 + WORD(SubOp='S') + 2*WORD(SubOp='C')  ;
    GotoColorStr ( Black, 1, i, S2 )              ;
    IF SubOp='E' THEN GetBuff                     ;
    IF SubOp='S' THEN MakeCurrentBuff             ;
    IF SubOp='C' THEN BEGIN
                        WRITE ('[A]')             ;
                        GetBuffClear              ;
                      END                         ;
    EXIT                                          ;
  END                                             ;

  DoWindow (Cyan, Yellow, 1,2,80,24,1,0)          ;
  WriteBottom  (0)                                ;
  IF NOT SetXBuff THEN EXIT                       ; {Set all XBuff Parameters }
  UpDateBuff (TRUE,XBuff,CurrentBuff)             ;
  DoWindow (LightGray, Black, 2,3,78,21, 1,1)     ;

  IF SubOp='R' THEN Start := GetRanges (0) ELSE Start := 0  ;

  CLRSCR                                      ;
  WRITE (StrL(76,0))                          ;
  WriteSides (1,76,2,17)                      ;
  WRITE (NL,StrL(76,1))                       ;
  DoWindow (LightGray, Black, 3,4,76,20, 0,0) ;
  TimeCnt := 0                                ;

  REPEAT
    CLRSCR                         ;
    GotoColorStr (DarkGray,1,1,S1) ;

    FOR i := 0 TO 15 DO
    BEGIN
       TEXTCOLOR (DarkGray)                                      ;
       IMax := (i*16) + Start                                    ;
       IF (IMax+p)<=XBuff.Max THEN WRITE ( NL, (IMax+p):6, '  ') ;

       FOR n := 0 TO 15 DO
       BEGIN
         JMax := IMax + n + p                                  ;
         IF (JMax-p)<=XBuff.Max THEN WriteByte:=XBuff.P^ [JMax];

         IF (TimeCnt=0) THEN
         CASE WriteByte OF $00     : Color := LightBlue       ;
                           $01..$7F: Color := Blue            ;
                           $80..$9F: Color := Black           ;
                           $A0..$EF: Color := Magenta         ;
                  $F1..$F6,$F8..$FE: Color := Red             ;
                           $F0, $F7: Color := Yellow          ;
                           $FF     : IF (Q.TimeEnable=1) THEN
                                     BEGIN
                                       TimeCnt := 2           ;
                                       Color   := DarkGray    ;
                                     END ELSE Color := Red    ;
                      END
         ELSE DEC (TimeCnt)                                   ;

         TEXTCOLOR (Color)                                    ;
         NumToStr1                                            ;
         IF (JMax<=XBuff.Max) THEN WRITE ( NumStr )           ;
       END                                                    ; { n Loop }
    END                                                       ; { i Loop }

    X := UPCASE (READKEY)                                         ;
    p := p + 256                                                  ;

    CASE X OF
    'E': IF XBuff.Max>256 THEN p:=(XBuff.Pos DIV 256)*256 ELSE p:=0  ;
    'N': UpDateBuff (BuffN,NBuff,1) ;
    'O': UpDateBuff (BuffO,OBuff,2) ;
    'S': UpDateBuff (BuffS,SBuff,3) ;
    #00: BEGIN
           Y := KeyRead(Y)                                 ;
           IF Y = #71 THEN p := 0                          ;
           IF Y = #79 THEN p := (XBuff.Max DIV 256)*256    ;
           IF Y = #81 THEN p := p                          ;
           IF Y = #73 THEN IF (p>511) THEN p := p - 512
                                      ELSE p := 0          ;
           X := ' '                                        ;
         END                                               ;
     ELSE p := p - 256                                     ;
     END                                                   ;

     IF (p>=XBuff.Max) THEN p := p-256                     ;

  UNTIL (X='Q') OR (X=#27)                                 ;
END                                                        ;
{--------------------------------------}
{ Read Single Patch Midi Data          }
{--------------------------------------}
PROCEDURE Do8250Read   ;
CONST
  S1 =    ' Polling Read : Press any key when ready :  ' ;
  S2 = NL+' All HW Input Ports disabled.'                ;
  S3 = NL+' No midi codes received.'                     ;
  S4 = NL+' Press any key to return to main menu..'      ;
  S5 = NL+' Total Dump Size:['                           ;
  S6 = NL+' SysEx DUMPSIZE :['                           ;
  BS5 = BS+BS+BS+BS+BS                                   ;

VAR
   n, i                : WORD        ;
   Choice              : CHAR        ;
   A                   : BYTE        ;
   SysManuf            : BYTE        ;
   SysDetect           : BOOLEAN     ;
   SysSize             : LONGINT     ;
   SERLineStat, SERData: INTEGER     ;

PROCEDURE SetUpPort (PIn: BOOLEAN; P:PortType) ;
BEGIN
  IF PIn THEN BEGIN
                SERLineStat := P.SERLineStat ;
                SERData     := P.SERData     ;
              END                            ;
END                                          ;
BEGIN
   {---------------------------------------------}
   { Initialise variables and storage            }
   {---------------------------------------------}
   SetUpPort (PortInB, P2)                       ;
   SetUpPort (PortInA, P1)                       ;
   WriteBottom (4)                               ;
   DoSubWindow (18)                              ;
   WRITE (S1)                                    ;
   Choice := KeyRead (Choice)                    ;

   IF NOT (PortInOK (C[1].PortIn)) THEN
   BEGIN
     WRITE (S2)     ;
     DELAY (1000)   ;
     EXIT           ;
   END              ;

   IF NOT SetXBuff THEN EXIT  ; { Set all XBuff Parameters }
   IF (Q.AutoClr=1) THEN ClearBuffer (XBuff)               ;
   StartBuff := CurrentBuff   ;

   ReadCount := 0        ;
   SysSize   := 0        ;
   SysManuf  := 0        ;
   SysDetect := FALSE    ;
   Choice    := #00      ;
   TEXTCOLOR (Red)       ;
   IF (Q.CodeOption=0) THEN
   BEGIN
     GOTOXY ( 50, 1 )          ;
     WRITE  ('[COUNT:      ]') ;
     GOTOXY ( 58, 1 )          ;
   END                         ;
   {-------------------------------------------------------}
   { Main Read Loop                                        }
   {-------------------------------------------------------}
   REPEAT
     IF KEYPRESSED THEN Choice := READKEY ;
     A := (PORT[SERLineStat]) AND 1       ;
     IF (A=1) THEN
     BEGIN
       WriteByte := PORT [SERData]             ;

       IF (Q.BuffOption=1) THEN
       BEGIN
          IF (XBuff.Pos>XBuff.Max) THEN
          BEGIN
             ReadCount := ReadCount+XBuff.Max+1 ;
             INLINE ( $FF/$16/SwopBuffersAddr)  ;  { CALL SwopBuffers    }
          END                                   ;
          XBuff.P^[XBuff.Pos] := WriteByte      ;
       END                                      ;

       IF (WriteByte=$F0)AND(Q.SysExOption=1) THEN SysDetect:=TRUE ;
       IF SysDetect THEN
       BEGIN
         IF SysSize=1 THEN SysManuf := WriteByte ;
         INC (SysSize)                           ;
         IF WriteByte=$F7 THEN SysDetect := FALSE;
       END                                       ;

       NumToStr1                                 ; {INLINE ReadByte->NumStr}

       IF (Q.CodeOption=1)   THEN WRITE (NumStr)
       ELSE IF NOT SysDetect THEN WRITE (XBuff.Pos:5, BS5)   ;

       INC (XBuff.Pos)                         ;
     END                                       ; { A = 1 }
  UNTIL (Choice<>#00)                          ;

  TEXTCOLOR (Black)                            ;
  ReadCount := ReadCount+XBuff.Pos+1           ;

  IF (ReadCount>0) THEN
  BEGIN
    WRITE (S5, ReadCount, ']' )          ;

    IF Q.BuffOption=1 THEN ResetBuff (1) ;

    IF Q.SysExOption=1 THEN
    BEGIN
      WRITE ( S6, SysSize,']',' Make:[',SeekManuf(SysManuf),']' ) ;
      IF Q.BuffOption=1 THEN SysExCheckSum (1)                    ;
    END                                                           ;
  END ELSE WRITE (S3)                                             ;

  WRITE ( S4 )                ;
  Choice := KeyRead (Choice)  ;
END                           ;  { DoRead }
{-----------------------}
{ Send code in buffer   }
{-----------------------}
PROCEDURE SendBuffer    ;
CONST
S1 = NL+'Note: All HW Output Ports disabled.'    ;
S2 = NL+'Press any key to return to Main Menu..' ;
VAR
  AStr        : Str10   ;
  E           : INTEGER ;
  Start, Stop : WORD    ;
  Count       : WORD    ;
  Choice      : CHAR    ;
BEGIN
  DoSubWindow (15)                                         ;
  IF NOT (PortOutOK (C[1].SysPortOut)) THEN WRITE (S1)     ;
  IF NOT  SetXBuff THEN EXIT       ; { Set XBuff Parameters}
  E := 0                                                   ;
  REPEAT
    WRITE  (NL+'Start [0-',XBuff.Max,'][ENTER=0] : ')      ;
    READLN (AStr)                                          ;
    IF AStr = '' THEN AStr := '0'                          ;
    VAL (AStr, Start, E )                                  ;
    IF Start>XBuff.Max THEN E := 1                         ;
  UNTIL (E=0)                                              ;
  REPEAT
    WRITE  (NL+'End   [1-',XBuff.Max,'][ENTER=ALL] : ')    ;
    READLN (AStr)                                          ;
    IF AStr = '' THEN STR( XBuff.Max, AStr )               ;
    VAL (AStr, Stop, E )                                   ;
    IF Stop>XBuff.Max THEN E := 1                          ;
  UNTIL (E=0)                                              ;
  Count := Start  ;
  Choice:= ' '    ;
  REPEAT
    DoWritePort (XBuff.P^[Count])                 ;
    IF KEYPRESSED THEN Choice := KeyRead (Choice) ;
    INC (Count)                                   ;
  UNTIL (Count>Stop) OR (Choice='Q')              ;

  WRITE (NL+'Transmitted : [ ', Count, ' ] bytes.')    ;
  WRITE (S2)                                           ;
  Choice := KeyRead (Choice)                           ;
END                                                    ;
{-----------------------}
{ 8250 Sender           }
{-----------------------}
PROCEDURE Do8250        ;
CONST
   S1 =   NL+' Note: All HW Output Ports disabled.'                   ;
VAR
    Count    : INTEGER                                 ;
    B        : BYTE                                    ;
BEGIN
  Count := 1                                           ;
  StartBuff := CurrentBuff                             ;
  DoSubWindow (15)                                     ;
  IF NOT (PortOutOK (C[1].SysPortOut)) THEN WRITE (S1) ;
  IF (Q.BuffOption=1) THEN ClearBuffer (XBuff)         ; { XBuff.Pos = 0 }

  WHILE (Count>0) DO
  BEGIN
    Count := GetNumberStr ( 1 )    ;
    FOR i := 1 TO Count DO
    BEGIN
      B := LO(SearchN[i])          ;
      DoWritePort ( B )            ;
      IF (Q.BuffOption=1) THEN
      BEGIN
        XBuff.P^[XBuff.Pos] := B                       ;
        IF (XBuff.Pos<=XBuff.Max) THEN INC (XBuff.Pos) ;
      END                          ;
    END                            ;
  END                              ;
  ResetBuff (1)                    ;
END                                ;
