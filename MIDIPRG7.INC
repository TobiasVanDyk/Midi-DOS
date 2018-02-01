{---------------------------}
{ INCLUDE FILE MIDIPRG7.INC }
{---------------------------}
{--------------------------------------}
PROCEDURE DoBottom (Option : BYTE ) ;
{--------------------------------------}
CONST
UD   = #24+#25                                       ;
Arow = #24+#25+#26+#27                               ;

S1=' Press key [A]-[Z] ([Esc]:Quit [0]:Clear all): ' ;
S2=' [ ]:Enter [DR:PATHNAME\]FILENAME.EXT : '        ;
M1=' [AnyKey]:Quit  [0]:Clear all '                  ;
M2=' [Key0]-[KeyF] : '  ;
M3=' [F1]-[F12] :  '    ;
K1=' [PgUD-End-Home'+UD+'] [ENTER]Edit [Q]uit [SPC] [D]el [F]ind [N][O][S]Buff ' ;
K2=' [ENTER][EndHome'+Arow+'][PRGKEYS][S]elect+[B][E][BS][INS-DEL]SHFT[C]opy[D]el[I]ns' ;
K3=' Select Buffer empty..'                                                              ;
L0=' [Note Number: 0-127] or [Note: C01-F11 (Flat=Xb)]: '     ;
L1=' ['+UD+']Select  [M]asterKey  [S]ubKey  [C]onfig  [D]ata  [P]rogKey  [Q]uit' ;
L2=' Enter MasterKey as'                                        ;
L3=' Enter Second Key as'                                       ;
L4=' Enter [DR:PATHNAME\]FILENAME.EXT : '                       ;

BEGIN
  IF (Option<>5) THEN DoWindow (LightGray, Blue , 1,25,80,25,1,0) ;
  IF (Option=5 ) THEN DoWindow (Green    , Black, 1,25,80,25,1,0) ;
  CASE Option OF
       0: WRITE (S1)            ;
       1: WRITE (S2)            ;
       2: WRITE (M1,M2)         ;
       3: WRITE (M1,M3)         ;
       4: WRITE (K1)            ;
       5: WRITE (K2)            ;
       6: WRITE (K3)            ;
       7: WRITE (L1)            ;
       8: WRITE (L2,L0)         ;
       9: WRITE (L3,L0)         ;
      10: WRITE (L4)            ;
  END                           ;
END                             ;
{------------------------------}
{ HexDump 16 x 16 = 256 bytes  }
{------------------------------}
PROCEDURE HexDump ( First, Last : WORD; Blank, ShowSelect: BOOLEAN ) ;

VAR
  AddrStr , ByteStr : Str10   ;
  AddrNum , ByteNum : WORD    ;
  Row, Col          : INTEGER ;
  DumpEnd           : BOOLEAN ;
  B                 : CHAR    ;
  A                 : WORD    ;
CONST
S1='³ EDITOR[HEX Dump]                                        [ASCII Dump]     ³'+NL;
{  '³ Addr  OO-O1 O2-O3 O4-O5 O6-O7  O8-O9 OA-OB OC-OD OE-OF  O123456789ABCDEF ³'}
S2='³                                                                          ³'+NL;
{  '³.HHHH..BB.BB.BB.BB.BB.BB.BB.BB..BB.BB.BB.BB.BB.BB.BB.BB..0123456789ABCDEF ³'}
{  '1234567890.........20........30........40........50........60........70....³'}
{  '...3.....10......................34........................60..............³'}
S4=  'Addr  OO-O1 O2-O3 O4-O5 O6-O7  O8-O9 OA-OB OC-OD OE-OF  O123456789ABCDEF' ;
{-------------------------------------------}
PROCEDURE SetSelect ( Option: BYTE )        ;
{-------------------------------------------}
VAR Select : BYTE                           ;

BEGIN
  A := AddrNum + Col                      ;
  ByteNum := XBuff.P^[ A ]                ;
  IF ( A > Last ) THEN DumpEnd := TRUE    ;
  Select  := BYTE( (SBuff.Size>0) AND (A>=SBuff.Start) AND (A<SBuff.Stop)) ;
  IF NOT ShowSelect THEN Select := 0      ;
  Select := Select + 2 * BYTE( (Search.CurCount>0) AND
                               (A=Search.Pos[Search.CurCount])) ;
  CASE Option OF
       0: BEGIN
            IF NOT DumpEnd THEN ByteStr := DecToHex ( ByteNum )
                           ELSE ByteStr := '..'                ;
            CASE Select OF
               0: BEGIN
                    TEXTATTR := BlueYellow                     ;
                    IF (ByteNum=Q.BlankChar) AND
                    (Blank) THEN TEXTATTR := BlueLightGray     ;
                   END                                         ;
               1: BEGIN
                      TEXTATTR := BlueLightRed                 ;
                      IF (ByteNum=Q.BlankChar) AND
                         (Blank) THEN TEXTATTR := BlueRed      ;
                  END                                          ;
               2: TEXTATTR := BlueWhiteBlink                   ;
               3: TEXTATTR := BlueLightRedBlink                ;
            END                                                ;
          END                                                  ;
       1: BEGIN
            IF NOT DumpEnd THEN
            BEGIN
              IF (ByteNum IN [ 07,08,10,13 ] ) THEN ByteStr := '.'
                                               ELSE ByteStr := CHR(ByteNum) ;
            END ELSE ByteStr := '-'                                         ;
            CASE Select OF
               0: TEXTATTR := BlueWhite                    ;
               1: TEXTATTR := BlueLightRed                 ;
               2: TEXTATTR := BlueWhiteBlink               ;
               3: TEXTATTR := BlueLightRedBlink            ;
            END                                            ;
          END                                              ;
  END                                   ;
END                                     ;
{------------- MAIN ----------------------}
BEGIN
    DumpEnd := FALSE                        ;
    DoWindow  (Cyan, Blue  , 1,2,80,24,1,0) ;
    DoWindow  (Blue, Yellow, 2,3,77,22,1,1) ;
    DoWindow  (Blue, Yellow, 2,3,78,22,0,0) ;

    WRITE (StrL(76,0),NL,S1)              ;
    FOR i := 1 TO 17 DO WRITE (S2)        ;
    WRITE (StrL(76,1))                    ;
    GotoColorStr (White,3,3,S4)           ;

    FOR Row := 0 TO 15 DO
    BEGIN
      TEXTATTR := BlueWhite              ;
      GOTOXY (3,Row+4)                   ;
      AddrNum := First + (Row)*16        ;
      AddrStr := DecToHex ( AddrNum )    ;
      WRITE (AddrStr:4, '  ' )           ;

      FOR Col := 0 TO 15 DO
      BEGIN
        SetSelect ( 0 )                  ;
        WRITE (ByteStr)                  ;
        IF (Col<15) THEN WRITE (' ')     ;
        IF (Col=7 ) THEN WRITE (' ')     ;
      END ;
      WRITE ('  ')          ;
      DumpEnd := FALSE      ;

      FOR Col := 0 TO 15 DO
      BEGIN
        SetSelect ( 1 )     ;
        WRITE (ByteStr)     ;
      END                   ;
    END                     ;  { Row Loop }

    DoBottom (4)         ;
END                      ;
{--------------------------------------}
{ Display the Contents of Buffer [i]   }
{--------------------------------------}
PROCEDURE DisplayHexData               ;
{--------------------------------------}
TYPE  KeyArrType = RECORD
                     Start :WORD   ;
                     Stop  :WORD   ;
                     OnOff :BYTE   ;
                     UpDate:BOOLEAN;
                   END             ;

      KType = ARRAY [0..63] OF KeyArrType   ;
CONST
      S1 : Str3 = '[.]'                     ;
      S2 =  '[N][O][S]'                     ;

VAR KeyArr      : KType ;

CONST
    PosX: ARRAY [0..15] OF BYTE =
          (09,12,15,18,21,24,27,30, 34,37,40,43,46,49,52,55) ;
    PosY: ARRAY [0..15] OF BYTE =
          (04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19) ;
    PosA: ARRAY [0..15] OF BYTE =
          (59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74) ;
VAR
  DumpSize                   : INTEGER ;
  PrevBuff                   : BYTE    ;
  SelBuff                    : BYTE    ;
  MaxPos                     : WORD    ;
  SBStart                    : WORD    ;
  i                          : WORD    ;
  n, k                       : INTEGER ;
  PageNum                    : BYTE    ;
  Key1, Key2                 : CHAR    ;
  EditMode, Blank, UpDate    : BOOLEAN ;
  PosXY : ARRAY [0..256,0..1] OF BYTE  ; { 256 Hex Area Position XY }
  PosAY : ARRAY [0..256,0..1] OF BYTE  ; { ASCII Area Position XY   }
{---------------------------------------}
{ Check MidiStr (initialising) Keys     }
{---------------------------------------}
PROCEDURE DoMidiKeys ( Pos  : BYTE      ;
                       i    : WORD      ;
                       X,Y  : BYTE    ) ;
VAR W : WORD                            ;
BEGIN
  IF (Pos<16) THEN TEXTBACKGROUND (LightCyan) ELSE TEXTBACKGROUND (Magenta) ;

  IF (KeyArr[Pos].Start>KeyArr[Pos].Stop) AND (KeyArr[Pos].OnOff=0) THEN
  BEGIN
    KeyArr[Pos].Stop := KeyArr[Pos].Start ;
    KeyArr[Pos].OnOff:= 0                 ;
    KeyArr[Pos].UpDate := FALSE           ;
  END                                     ;

  IF KeyArr[Pos].OnOff=0 THEN
  BEGIN
    KeyArr[Pos].Start := i ;
    TEXTCOLOR (White)      ;
  END                      ;
  IF KeyArr[Pos].OnOff=1 THEN
  BEGIN
    KeyArr[Pos].Stop := i      ;
    TEXTCOLOR (LightGray)      ;
    KeyArr[Pos].UpDate := TRUE ;
  END                          ;

  IF ((Pos<16) AND
      (KeyArr[Pos].OnOff=1) AND
      (KeyArr[Pos].Stop-KeyArr[Pos].Start>18)) THEN
      BEGIN
        KeyArr[Pos].UpDate := FALSE ;
        EXIT                        ;
      END                           ;

   KeyArr[Pos].OnOff := KeyArr[Pos].OnOff XOR 1 ;

   GOTOXY (X,Y)                             ;
   W := XBuff.P^[i]                         ;
   WRITE ( DecToHex( W ) )                  ;
END                                         ;
{---------------------------------------}
{ Select Buffer is Empty                }
{---------------------------------------}
FUNCTION SBuffEmpty : BOOLEAN           ;
BEGIN
  SBuffEmpty := FALSE         ;
  IF (SBuff.Size=0) THEN
  BEGIN
    DoBottom (6)                        ;
    DELAY (1000)                        ;
    DoBottom (4)                        ;
    SBuffEmpty := TRUE                  ;
  END                                   ;
END                                     ;
{------------------------}
{ Def Select Buffer      }
{------------------------}
PROCEDURE SelectBuff ( A : WORD ; X, Y : BYTE ; KB : CHAR )          ;
VAR WStr       : Str10  ;
    W, i, n    : WORD   ;

BEGIN
  SBuff.Size := 0                                            ;

  IF (KB='B') THEN BEGIN
                     SBuff.Start := A                        ;
                     SBuff.Stop  := A                        ;
                   END                                       ;

  IF (KB='E') THEN BEGIN
                     IF A > SBuff.Start THEN SBuff.Stop:=A
                                        ELSE SBuff.Stop:=SBuff.Start ;
                   END                                               ;

  IF (SBuff.Size>=SBuff.Max) THEN SBuff.Stop:=SBuff.Max           ;

  GOTOXY (X,Y)                                                   ;
  W := XBuff.P^[A]                                               ;
  WStr := DecToHex(W)                                            ;
  TEXTBACKGROUND (Red)                                           ;
  TEXTCOLOR (LightGreen)                                         ;
  IF KB='B' THEN WRITE (WStr)                                    ;

  SBuff.Size := SBuff.Stop - SBuff.Start   ;
  SBuff.Pos  := SBuff.Stop                 ;

  n := 0                                   ;
  IF (SBuff.Size>0) THEN
  BEGIN
    SelBuff := CurrentBuff                 ;
    FOR i:=SBuff.Start TO SBuff.Stop-1 DO
    BEGIN
      SBuff.P^[n] := XBuff.P^[i]           ;
      INC (n)                              ;
    END                                    ;
  END                                      ;
END                                        ;
{-----------------------------}
{ Insert one space            }
{-----------------------------}
PROCEDURE DoInsert (A : WORD)                             ;
BEGIN
  MOVE ( XBuff.P^[A], XBuff.P^[A+1], XBuff.Max - (A-1) )  ;
  XBuff.P^[A] := 0                                        ;
  IF A<=MaxPos THEN INC (MaxPos)                          ;
  UpDate := TRUE                                          ;
END                                                       ;
{--------------------------------------------}
{ Copy Select Block : Overwrite              }
{--------------------------------------------}
PROCEDURE DoCopy ( A : WORD )                ;
BEGIN
  IF SBuffEmpty THEN EXIT                    ;
  UpDate := TRUE                             ;
  IF (SBuff.Size>0) AND
     (A+SBuff.Size<=XBuff.Max) THEN
     MOVE ( SBuff.P^[0], XBuff.P^[A], SBuff.Size )       ;
  IF (MaxPos<A+SBuff.Size) THEN MaxPos := A + SBuff.Size ;
END                                                      ;
{------------------------------------------}
{ Delete Select Block                      }
{------------------------------------------}
PROCEDURE DoDelete ( A:WORD )              ;
VAR n, i : WORD                            ;
BEGIN
  IF SBuffEmpty THEN EXIT                      ;
  MOVE ( XBuff.P^[SBuff.Stop], XBuff.P^[SBuff.Start], XBuff.Max-SBuff.Stop+1 )          ;
  FOR i := XBuff.Max-(SBuff.Stop-SBuff.Start-1) TO XBuff.Max DO XBuff.P^[i]:=0  ;
  SBuff.Start  := 0                                ;
  SBuff.Stop   := 0                                ;
  SBuff.Size   := 0                                ;
  SBuff.Pos    := 0                                ;
  IF A<=MaxPos THEN MaxPos := (A-SBuff.Size)+(MaxPos-SBuff.Start) ;
  UpDate := TRUE                                                  ;
END                                                ;
{----------------------------------------------------}
{ Delete One Char                                    }
{----------------------------------------------------}
PROCEDURE Do1Delete ( A:WORD )                       ;
BEGIN
  IF (A=XBuff.Max) THEN EXIT                         ;
  MOVE ( XBuff.P^[A+1], XBuff.P^[A], XBuff.Max-A )   ;
  XBuff.P^[XBuff.Max] :=0                            ;
  IF A<=MaxPos THEN MaxPos := MaxPos-1               ;
  UpDate := TRUE                                     ;
END                                                  ;
{-----------------------------------------------------}
{ Inside Editor                                       }
{-----------------------------------------------------}
PROCEDURE DumpEdit (Start : WORD; Blank : BOOLEAN ) ;

VAR
  KeyA, KeyB                 : CHAR      ;
  A                          : WORD      ;
  c, m                       : INTEGER   ;
  HexStr                     : Str10     ;
  Number                     : BYTE      ;
  K, xy                      : BYTE      ;

BEGIN
    xy := 0                ;

    REPEAT
      UpDate := FALSE                                                      ;
      DoBottom (5)                                                         ;
      DoWindow  (Blue, Yellow, 2,3,78,23,0,0) ;
      TEXTATTR := GreenRed                                                 ;
      A := Start + xy                                                      ;
      GOTOXY ( PosXY [xy,0], PosXY [xy,1] )                                ;

      GetKey                                                               ;
      HexStr[1] := KBrd.UpCh                                               ;
      IF KBrd.Ch='D' THEN HexStr[1]:='R'                                   ;
      IF KBrd.Ch='C' THEN HexStr[1]:='T'                                   ;

      CASE KBrd.Class OF
      0: CASE HexStr[1] OF
         #48..#57,
         #65..#70: BEGIN
                      WRITE (HexStr[1])                        ;
                      REPEAT HexStr[2] := UPCASE (READKEY)     ;
                      UNTIL (HexStr[2] IN [#48..#57,#65..#70]) ;
                      WRITE (HexStr[2])                        ;
                      HexStr[0] := CHR (2)                     ;
                      Number := HexToDec (HexStr)              ;
                      IF (Blank) AND (Number=Q.BlankChar) THEN
                                 WRITE (#8#8,'..')             ;
                      GOTOXY ( PosAY [xy,0] , PosAY [xy,1] )   ;
                      IF ( Number IN [ 07,08,10,13 ] ) THEN WRITE ('.')
                           ELSE WRITE ( CHR(Number) )            ;
                      XBuff.P^[A] := Number                      ;
                      IF (MaxPos<A) THEN MaxPos := A             ;
                      IF (xy<255) THEN INC (xy)                  ;
                   END                                           ;
      #27,#13,'Q': BEGIN
                     TEXTATTR := BlueYellow  ;
                     EXIT                    ;
                   END                       ;
             #08 : BEGIN
                     IF A>0 THEN
                     BEGIN
                       Do1Delete (A-1)         ;
                       DEC (xy)                ;
                     END                       ;
                   END                         ;
              'S': BEGIN   { Select Buffer Definition }
                     KeyB := UPCASE (READKEY)                       ;
                     IF KeyB IN ['B','E'] THEN
                     SelectBuff (A, PosXY[xy,0], PosXY[xy,1], KeyB) ;
                     IF KeyB = 'E' THEN UpDate := TRUE              ;
                   END                                              ;
              'I': BEGIN    { Insert one space }
                     DoInsert ( A )                          ;
                   END                                       ;
              'T': BEGIN    { Transfer - Copy Select Buffer  }
                     IF (SBuff.Size=0) THEN
                     BEGIN
                       DoBottom (6)                          ;
                       DELAY (1000)                          ;
                       DoBottom (5)                          ;
                     END
                     ELSE DoCopy (A)                         ;
                   END                                       ;
              'R': BEGIN    { Remove - Delete Select Buffer  }
                     DoDelete (A)                            ;
                   END                                       ;
          END    ;         { Case HexStr[1]                  }
   1..3: CASE KBrd.Scan OF
           71 : { Home        } xy := 0              ;
           79 : { End         } xy := 255            ;
           72 : { Up Arrow    } xy := xy - 16        ;
           80 : { Down Arrow  } xy := xy + 16        ;
           75 : { Left Arrow  } DEC (xy)             ;
           77 : { Right Arrow } INC (xy)             ;
           82 : { Insert      } DoInsert  (A)        ;
           83 : { Delete      } Do1Delete (A)        ;
         END                                              ;
 10..14: DoMidiKeys (KBrd.Pos,A,PosXY[xy,0],PosXY[xy,1])  ;
 END                                                      ;

  IF UpDate THEN HexDump (Start,XBuff.Max,Blank,(SelBuff=CurrentBuff));

  UNTIL HexStr[1] = #27 ;
 END                    ;
{-----------------------------------------}
PROCEDURE UpDateMidiKeys                  ;
{-----------------------------------------}
VAR i, j, n : INTEGER                     ;

PROCEDURE DoFKey ( VAR Start, Stop: ArrW12; n : INTEGER ) ;
VAR i : INTEGER                                           ;
BEGIN
  FOR i := 1 TO 12 DO
  BEGIN
    IF (KeyArr[n].UpDate) AND (KeyArr[n].Start<=KeyArr[n].Stop) THEN
    BEGIN
      Start[i] := KeyArr[n].Start ;
      Stop[i]  := KeyArr[n].Stop  ;
      IF (Stop[i]=Start[i]) THEN
      BEGIN
        Stop[i]  := 0 ;
        Start[i] := 0 ;
      END             ;
    END                                  ;
    INC (n)                              ;
  END                                    ;
END                                      ;
BEGIN
  {-------------------------- Update Initialising Keys }
FOR i := 0 TO 15 DO
BEGIN
  IF (KeyArr[i].UpDate) AND (KeyArr[i].Start<=KeyArr[i].Stop) THEN
  BEGIN
    n := 0                                             ;
    C[i].MidiStrX := KeyArr[i].Stop - KeyArr[i].Start  ; { Length }
    FOR j := KeyArr[i].Start TO KeyArr[i].Stop DO
    BEGIN
      C[i].MidiStr[n] := XBuff.P^[j] ;
      INC (n)                       ;
    END                             ;
  END                               ;
END                                 ;
  {------------Update Buffer (F1-F12) Keys }
  DoFKey ( FKey.N.Start, FKey.N.Stop, 16 ) ;
  DoFKey ( FKey.S.Start, FKey.S.Stop, 28 ) ;
  DoFKey ( FKey.C.Start, FKey.C.Stop, 40 ) ;
  DoFKey ( FKey.A.Start, FKey.A.Stop, 52 ) ;
END                                        ;
{---------------------------------------------------------}
PROCEDURE UpDateBuff (BOn:BOOLEAN;VAR B:BuffType;CB:BYTE) ;
{---------------------------------------------------------}
BEGIN
  IF BOn THEN
  BEGIN
    IF CB=3 THEN BEGIN
                   SBStart := B.Start ;
                   B.Start := 0       ;
                 END                  ;
    XBuff.Pos := MaxPos               ;
    CASE PrevBuff OF
    1: NBuff:=XBuff ;
    2: OBuff:=XBuff ;
    3: BEGIN
         SBuff:=XBuff           ;
         SBuff.Start := SBStart ;
       END                      ;
    END             ;
    CurrentBuff := CB               ;
    XBuff := B                      ;
    n := 0                          ;
    i := XBuff.Start                ;
    PageNum := 0                    ;
    PrevBuff := CurrentBuff         ;
    MaxPos   := XBuff.Pos           ;
  END                               ;
END                                 ;
{ -----------------------------MAIN ----------------- Display HexData }
BEGIN { Display the results }

FOR i := 0 TO 63 DO
  BEGIN
    KeyArr[i].Start  := 0       ;
    KeyArr[i].Stop   := 0       ;
    KeyArr[i].OnOff  := 0       ;
    KeyArr[i].UpDate := FALSE   ;
  END                           ;

   EditMode := FALSE  ;
   Blank    := TRUE   ;

   DumpSize := 256  ;

   FOR i := 0 TO 15 DO
       FOR n := 0 TO 15 DO
       BEGIN
         PosAY [i*16+n,0] := PosA [n] ;
         PosAY [i*16+n,1] := PosY [i] ;
         PosXY [i*16+n,0] := PosX [n] ;
         PosXY [i*16+n,1] := PosY [i] ;
       END                            ;

  IF NOT SetXBuff THEN EXIT           ; {Set all XBuff Parameters }
  n := 0                  ;
  i := XBuff.Start        ;
  PageNum := 0            ;
  PrevBuff := CurrentBuff ;
  MaxPos   := XBuff.Pos   ;
  Search.CurCount := 0    ;

  REPEAT
    HexDump   ( i, XBuff.Max, Blank, (SelBuff=CurrentBuff))       ;
    DoWindow (LightGray, Black, 1, 25, 80, 25, 0 ,0)              ;
    GOTOXY (73, 1)                                                ;
    WRITE ( PageNum+1:3, ':',(XBuff.Max DIV DumpSize):3 )         ;
    S1[2] := BuffChr1[CurrentBuff]                                ;
    GotoColorStr (Blue    ,58,1,S2)                               ;
    GotoColorStr (LightRed,58+(CurrentBuff-1)*3,1,S1)             ;
    {--------------------------------------}
    { Check for special keys, escape and E }
    {--------------------------------------}
    Key1 := UPCASE (READKEY) ;
    IF Key1 IN ['N','O','S'] THEN Search.CurCount := 0 ;
    CASE Key1 OF
              #27: { Escape } ;
              #32: { SPCbar } Blank := Blank XOR TRUE  ;
              #00: BEGIN
                     Key2 := READKEY ;     { 2 code for special keys }
                     CASE Key2 OF
                                #73 : { PageUp    } i := i-DumpSize           ;
                                #81 : { Page Down } i := i+DumpSize           ;
                                #71 : { Home      } i := XBuff.Start          ;
                                #79 : { End       } i := XBuff.Max-DumpSize+1 ;
                                #72 : { Up Arrow  } DEC (i)                   ;
                                #80 : { Down Arrow} INC (i)                   ;
                     END          ; { Case Key2                         }
                     IF i <  XBuff.Start THEN i := XBuff.Start          ;
                     IF i >= XBuff.Max   THEN i := XBuff.Max-DumpSize+1 ;
                   END   ; { Case Key1 = #0 }
              #13: BEGIN
                       EditMode := EditMode XOR TRUE  ;
                       IF EditMode THEN
                       BEGIN
                         DumpEdit (i, Blank)            ;
                         EditMode := FALSE              ;
                       END                              ;
                   END                                  ;
              'N': UpDateBuff (BuffN,NBuff,1)           ;
              'O': UpDateBuff (BuffO,OBuff,2)           ;
              'S': UpDateBuff (BuffS,SBuff,3)           ;
              'F': BEGIN
                     IF (CurrentBuff=Search.Buff) AND (Search.Num>0) THEN
                     BEGIN
                       IF (Search.CurCount<Search.Count) THEN
                           INC (Search.CurCount) ELSE Search.CurCount:=1 ;
                       i := Search.Pos [Search.CurCount]                 ;
                     END                                                 ;
                   END                                                   ;
              'D': BEGIN    { Remove - Delete Select Buffer  }
                     DoDelete ( i )                          ;
                   END                                       ;
          END    ;         { Case Key1      }
    PageNum := (i DIV DumpSize)  ;
  UNTIL (Key1=#27) OR (Key1='Q') ; { ESC }

  XBuff.Pos := MaxPos                ;
  IF CurrentBuff=1 THEN NBuff:=XBuff ;
  IF CurrentBuff=2 THEN OBuff:=XBuff ;
  IF CurrentBuff=3 THEN SBuff:=XBuff ;

  UpDateMidiKeys   ;
END                ;
{-----------------------------------}
{ Display F Keys                    }
{-----------------------------------}
PROCEDURE DisplayFKeys              ;

CONST
S0 ='³                                                               ³' + NL ;
S2 ='³ Programmed F Keys [F1] - [F10] ([F11],[F12] optional) :       ³' + NL +
    '³ NORMAL        SHIFT           CONTROL         ALT             ³' + NL +
    '³ KEY Range     KEY   Range     KEY   Range     KEY   Range     ³' + NL ;

S20='³ Define Ranges inside Editor by pressing selected F Key twice: ³' + NL +
    '³ 1: Start position. 2: End position.                           ³' + NL ;

VAR
  i            : INTEGER ;
  Y            : CHAR    ;

PROCEDURE WriteS ;
VAR N : INTEGER ;
    A : BYTE    ;
CONST B=
'³ F1            S-F1            C-F1            A-F1            ³';
{³ KEY Range     KEY   Range     KEY   Range     KEY   Range     ³ }
BEGIN
  FOR n := 1 TO 12 DO
  BEGIN
     GotoColorStr (Blue,1,4+n,B) ;
     TEXTCOLOR (Red)      ;
     GOTOXY (3 , 4+n)     ; WRITE  ( 'F'  ,n )   ;
     GOTOXY (17, 4+n)     ; WRITE  ( 'S-F',n )   ;
     GOTOXY (33, 4+n)     ; WRITE  ( 'C-F',n )   ;
     GOTOXY (49, 4+n)     ; WRITE  ( 'A-F',n )   ;
  END                     ;
  TEXTCOLOR (Blue)        ;
END                       ;

PROCEDURE WriteKey (XPos,YPos: INTEGER; VAR Start, Stop: WORD ) ;

VAR FStr1, FStr2 : Str10 ;
    S            : WORD  ;

BEGIN
    TEXTCOLOR (Black)                               ;
    GOTOXY (XPos,YPos)                              ;
    IF (Stop>Start) THEN
    BEGIN
      FStr1 := DecToHex(Start)                      ;
      S := Stop-1                                   ;
      FStr2 := DecToHex(S)                          ;
      WRITE (FStr1:4,'-',FStr2)
    END ELSE BEGIN
               TEXTCOLOR (LightGray) ;
               WRITE (' Not Def')    ;
             END                     ;
END                                  ;

BEGIN
  DoWindow (Cyan     , Blue, 1, 2, 80, 24, 1 ,0) ;
  DoWindow (LightGray, Blue, 4, 3, 70, 21, 1 ,1) ;
  DoBottom (3)                                 ;
  REPEAT
    DoWindow (LightGray, Blue, 4, 3, 71, 21, 0,0 ) ;
    WRITE ( StrL(65,0),NL,S2 )                     ;
    WriteS                                         ;
    WRITE ( NL,S20,StrL(65,1))                     ;
    FOR i := 0 TO 11 DO
    BEGIN
      WriteKey (7 ,i+5, FKey.N.Start[i+1], FKey.N.Stop[i+1] ) ;
      WriteKey (23,i+5, FKey.S.Start[i+1], FKey.S.Stop[i+1] ) ;
      WriteKey (39,i+5, FKey.C.Start[i+1], FKey.C.Stop[i+1] ) ;
      WriteKey (55,i+5, FKey.A.Start[i+1], FKey.A.Stop[i+1] ) ;
    END                                                       ;
    DoBottom (3)                                              ;
    Y := KeyRead ( Y )                                        ;
    IF Y='0' THEN ClearFKeyList                               ;
  UNTIL (Y<>'0')                                              ;
END                                                           ;
{-----------------------------------}
{ Display I Keys                    }
{-----------------------------------}
PROCEDURE DisplayIKeys              ;

CONST
S2 ='³ Programmed Startup Keys: [0]-[9][A]-[F] [Max length:18]         ³' + NL +
    '³ KEY  DEFINITION                                            SIZE ³' + NL ;
     { [0]  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17      }
S20='³ Define Ranges from Editor: Press [ALT+Key] twice [Begin-End]    ³' + NL ;

VAR
  i            : INTEGER ;
  Y            : CHAR    ;

PROCEDURE WriteS ;
VAR N : INTEGER ;
    A : BYTE    ;
CONST B=
{³ KEY  DEFINITION                                                 ³ }
'³ [0]  00                                                         ³';
BEGIN
  A := 48                ; { Chr '0' }
  FOR n := 0 TO 15 DO
  BEGIN
     GotoColorStr (Blue,1,4+n,B) ;
     TEXTCOLOR (Red)      ;
     GOTOXY (3,  4+n)     ; WRITE  ( '[', CHR(A), ']' )   ;
     INC (A)              ;
     IF A=58 THEN A:=65   ;
  END                     ;
  TEXTCOLOR (Blue)        ;
END                       ;

PROCEDURE WriteKey (XPos,YPos: INTEGER; S: ArrB18; VAR Size : WORD ) ;

VAR HStr : Str10   ;
    H    : WORD    ;
    k, i : INTEGER ;

BEGIN
    IF Size>18 THEN Size:=18                        ;
    IF (Size>0) THEN TEXTCOLOR (Black)
                ELSE TEXTCOLOR (LightGray)          ;
    k := 0                                          ;
    FOR i := 0 TO Size-1 DO
    BEGIN
      GOTOXY (XPos+k,YPos)                          ;
      H    := S[i]                                  ;
      HStr := DecToHex(H)                           ;
      WRITE (HStr)                                  ;
      k := k+3                                      ;
    END                                             ;
    TEXTCOLOR (Red)                                 ;
    GOTOXY (62,YPos)                                ;
    WRITE ('[',Size:2,']')                          ;
END                                                 ;
{------------------- MAIN --------------------------}
BEGIN
  DoWindow (Cyan     , Blue, 1, 2, 80, 24, 1 ,0) ;
  DoWindow (LightGray, Blue, 4, 3, 74, 23, 1 ,1) ;
  DoBottom (2)                                 ;
  REPEAT
    DoWindow (LightGray, Blue, 4, 3, 75, 23, 0, 0 ) ;
    WRITE (StrL(67,0),NL, S2 )     ;
    WriteS                         ;
    WRITE (NL,S20,StrL(67,1))      ;
    FOR i := 0 TO 15 DO
    BEGIN
      WriteKey (8,i+4, C[i].MidiStr, C[i].MidiStrX )     ;
    END                                                  ;
    DoBottom (2)                                         ;
    Y := KeyRead ( Y )                   ;
    IF Y = '0' THEN ClearMidiStr         ;
  UNTIL (Y<>'0')                         ;

END                                      ;

{----------------------------------------}
{ Define File ink programmable Keys      }
{----------------------------------------}
PROCEDURE DefFileKeys                    ;
CONST
S0='³                                                                        ³'+NL  ;
S2='³ Programmed Disk File Linked Keys [A] - [Z] :                           ³'+NL  +
   '³ KEY FILE-NAME               SIZE    KEY FILE-NAME               SIZE   ³'+NL  ;

S6='³ Links Disk File: (Drive:)(Path\)Filename to Keys [A] - [Z].            ³'+NL  +
   '³ ( Maximum number of characters: Pathname:[8], Filename:[8.3] )         ³'+NL  ;

VAR  Y        : CHAR   ;
     NStr     : Str24  ;
     i        : INTEGER ;

PROCEDURE WriteS ;
VAR N : INTEGER ;
    A : BYTE    ;
CONST S=
'³ [ ]                         [     ] [ ]                         [     ]³';
{³ [A] a:keysdef \filename.ext [12345] [N] a:keysdef \filename.ext [12345]³ }
BEGIN
  A := 65                ; { Chr 'A' }
  FOR n := 0 TO 12 DO
  BEGIN
     GotoColorStr (Blue,1,4+n,S) ;
     TEXTCOLOR (Red)     ;
     GOTOXY (4,  4+n)    ; WRITE  ( CHR(A+n) )   ;
     GOTOXY (40, 4+n)    ; WRITE  ( CHR(A+13+n)) ;
  END                    ;
  TEXTCOLOR (Blue)       ;
END                      ;
{----------------------------}
PROCEDURE WriteFLink         ;
CONST NF = ' No F'           ;
VAR i : INTEGER              ;
BEGIN
  FOR i := 1 TO 13 DO
  BEGIN
    TEXTCOLOR (Black)                               ;
    GOTOXY (7, 3+i)    ; WRITE (FLink[i])           ;
    GOTOXY (43, 3+i)   ; WRITE (FLink[i+13])        ;
    GOTOXY (32, 3+i)   ;
    IF (FSizeArr[i]>0)    THEN WRITE (FSizeArr[i]:5)    ELSE WRITE (NF) ;
    GOTOXY (68, 3+i)   ;
    IF (FSizeArr[i+13]>0) THEN WRITE (FSizeArr[i+13]:5) ELSE WRITE (NF) ;
  END                          ;
END                            ;

BEGIN
  DoWindow (Cyan     , Blue, 1, 2, 80, 24, 1, 0 ) ;
  DoWindow (LightGray, Blue, 2, 3, 76, 22, 1, 1 ) ;
  REPEAT
    DoBottom (0)                                    ;
    DoWindow (LightGray, Blue, 2, 3, 77, 22, 0, 0 ) ;
    WRITE ( StrL(74,0),NL,S2 )        ;
    WriteS                            ;
    WRITE ( NL,S0,S6,StrL(74,1))      ;
    FOR i := 1 TO 26 DO FSizeArr[i] := CheckFileSize (FLink[i]) ;
    WriteFLink                                                 ;
    DoBottom (0)                ;
    Y := KeyRead (Y)            ;
    IF Y IN ['A'..'Z'] THEN
    BEGIN
      DoBottom (1)              ;
      GotoColorStr (Black,3,1,Y);
      GOTOXY (43,1)             ;
      READLN (NStr)             ;
      IF (LENGTH(NStr)<26) THEN
      BEGIN
        FLink [ORD(Y)-64] := NStr        ;
      END                                ;
    END                                  ;
    IF Y = '0' THEN ClearFLinkList       ;
  UNTIL (Y=#27)                          ;
END                                      ;
{-----------------------------------}
{ Binary Data Editor                }
{-----------------------------------}
PROCEDURE Edit (Option: CHAR)       ;
{-----------------------------------}
VAR
  i                   : INTEGER ;
  j                   : BYTE    ;
  k                   : WORD    ;
  M, X                : WORD    ;
  N                   : LONGINT ;
  NStr, MStr          : Str4    ;
  NumberStr           : Str24   ;
  BFrom, BTo, BSize   : WORD    ;
  CChar               : BYTE    ;
  Select              : CHAR    ;
  Y                   : CHAR    ;
  NumOK               : BOOLEAN ;
{-------------------------------}
PROCEDURE ClearL (Y : WORD)     ;
{-------------------------------}
CONST CL = Sp6+Sp6+'     '  ;
BEGIN
  GOTOXY (2,Y)  ; WRITE (CL) ; GOTOXY (3,Y) ;
END             ;
{--------------------------------------------}
{ Get numbers from String                    }
{--------------------------------------------}
FUNCTION GetNum ( IStr: STRING; VAR N1, N2: WORD) : BOOLEAN ;
VAR
  i, n, E    : INTEGER             ;
  Pos        : INTEGER             ;

FUNCTION GetNext ( VAR Pos : INTEGER ; S : STRING ) : WORD  ;
VAR Num      : WORD                                         ;
    NumStr   : Str24                                        ;
    i, n     : INTEGER                                      ;
    Len      : INTEGER                                      ;

BEGIN
  i := Pos                                     ;
  Len := LENGTH (S)                            ;
  WHILE (S[i]=' ') AND (i<=Len) DO INC(i)      ;
  n := 1                                       ;
  WHILE (S[i]<>' ') AND (i<=Len) DO
  BEGIN
    IF S[i] IN ['0'..'9'] THEN
    BEGIN
      NumStr[n] := S[i]   ;
      INC (n)             ;
    END ELSE i:= Len      ;
    INC (i)               ;
  END                     ;
  NumStr[0] := CHR(n-1)      ;
  Pos := i                   ;
  VAL ( NumStr, Num, E )     ;
  IF E=0 THEN GetNext := Num ;
  IF E>0 THEN GetNext := 0   ;
END                          ;
{------- MAIN ----------------------------}
BEGIN
  GetNum := FALSE                         ;
  Pos := 1                                ;
  N1 := GetNext (Pos, IStr)               ;
  IF E=0 THEN N2 := GetNext (Pos, IStr)   ;
  IF E=0 THEN GetNum := TRUE              ;
END                                       ;

{-----------------------------------------}
{ Get Fill and Blank Characters Number    }
{-----------------------------------------}
FUNCTION GetChar ( X : BYTE ) : BYTE      ;
CONST
    S2A = '³ BLANK CHAR:     ³' + NL ;
    S2B = '³ FILL CHAR:      ³' + NL ;
    S2C = '³ CLEAR Chr[ :   ]³' + NL ;
    S2D = '³ CHECKSUM:       ³' + NL ;
    S3A = '³ New Num [0-255]:³' ;
    S3B = '³ Enter [FROM TO]:³' ;
    S4  = '³                 ³' ;

VAR NStr : Str10   ;
    N    : INTEGER ;
    E    : INTEGER ;

BEGIN
  N := 0                             ;
  TEXTATTR := LightGrayBlue          ;
  GOTOXY (1,12)                      ;
  IF X=0 THEN WRITE ( S2A,S3A )   ;
  IF X=1 THEN WRITE ( S2B,S3A )   ;
  IF X=2 THEN WRITE ( S2C,S3B )   ;
  IF X=3 THEN WRITE ( S2D,S3B )   ;
  WRITE ( NL,S4,NL,S4,NL,S4 )        ;
  TEXTCOLOR (Red)                    ;
  IF (X<2) THEN
  REPEAT
    ClearL (14)                  ;
    READLN (NStr)                ;
    VAL (NStr, N, E)             ;
  UNTIL (E=0)AND(N<256)AND(N>=0) ;
  GetChar := N                   ;
END                              ;
{--------------------------------}
PROCEDURE DisplayRemoteKeys      ;
{--------------------------------}
CONST
S0='³                                                                            ³';
S2='³Programmed Keyboard Remote Keys : MASTER KEY [   :    ]                     ³'+
   '³KEY        CONFIG-FILE        PROG-KEY-FILE      DATA-FILE          Size    ³';

S6='³ Links Disk File: (Drive:)(Path\)Filename to up to 12 Keyboard Notes.       ³'+
   '³ ( Maximum number of characters: Pathname:[8], Filename:[8.3] )             ³';
XLate : ARRAY [0..12] OF Str2 =
       (' C','Db',' D','Eb',' E',' F','Gb',' G','Ab',' A','Bb',' B','C') ;

VAR  X, Y     : CHAR    ;
     NStr     : Str24   ;
     N, i     : INTEGER ;
     Line     : INTEGER ;

PROCEDURE WriteS ;
VAR n    : INTEGER ;
    A    : BYTE    ;
    Size : LONGINT ;
    x,y  : INTEGER ;
CONST S=
'³[   :    ]                                                          [      ]³';
{³KEY        CONFIG-FILE        PROG-KEY-FILE      DATA-FILE          Size    ³}
{³[127:Cb12] PATH1\12345678.EXT PATH1\12345678.EXT PATH1\12345678.EXT [100000]³}
BEGIN
  TEXTCOLOR (Black)                                           ;
  x := (RemoteMaster) DIV 12                                  ;
  y := (RemoteMaster) MOD 12                                  ;
  GOTOXY (48, 2) ; WRITE  ( RemoteMaster:3, ':', XLate[y],x ) ;

  FOR n := 1 TO 12 DO
  BEGIN
     GotoColorStr (Blue,1,3+n,S) ;
     TEXTCOLOR (Red)     ;
     IF (n+3=Line) THEN TEXTCOLOR (White)                 ;
     GOTOXY (3,  3+n)    ; WRITE  ( Remote[n].KEY:3 )     ;
     x := (Remote[n].KEY) DIV 12                          ;
     y := (Remote[n].KEY) MOD 12                          ;
     GOTOXY (7,  3+n)    ; WRITE  ( XLate[y], x )         ;
     GOTOXY (13, 3+n)    ; WRITE  ( Remote[n].CFG  )      ;
     GOTOXY (32, 3+n)    ; WRITE  ( Remote[n].PROG )      ;
     GOTOXY (50, 3+n)    ; WRITE  ( Remote[n].DTA  )      ;
     Size := CheckFileSize (Remote[n].DTA)                ;
     GOTOXY (72, 3+n)    ; WRITE  ( Size:5  )             ;
  END                    ;
  TEXTCOLOR (Blue)       ;
END                      ;
{---------------------------------------}
FUNCTION ExtractNum (S:Str10) : INTEGER ;
VAR T, U            : Str10   ;
    Len, E, N, M, i : INTEGER ;

BEGIN
  T := '  '                                ;
  E := 0                                   ;
  Len := LENGTH(S)                         ;
  T[1] := UPCASE (S[1])                    ;
  T[2] := UPCASE (S[2])                    ;

  IF T[1] IN ['0'..'9'] THEN VAL (S, N, E) ;

  IF T[1] IN ['A'..'G'] THEN
  BEGIN
    M := 0                                         ;
    IF T[2] = 'B' THEN
    BEGIN
        T[2]:='b'                                  ;
        FOR i:= 1 TO 12 DO IF T=XLate[i] THEN M:=i ;
        U := COPY(S, 3, Len)                       ;
    END                                            ;
    IF T[2] IN ['0'..'9'] THEN
    BEGIN
        FOR i:= 1 TO 12 DO IF T[1]=XLate[i][2] THEN M:=i ;
        U := COPY(S, 2, Len)                             ;
    END                                                  ;
    IF M=0 THEN E:=10                                    ;
    VAL (U, N, E)                                        ;
    IF E=0 THEN N := (12*(N-1))+M                        ;
 END                                                     ;

 IF E=0 THEN ExtractNum := N ELSE ExtractNum := 255      ;
END                                                      ;
{--------------------------------------------------------}
BEGIN
  Line := 4                                      ;
  DoWindow (Cyan     , Blue, 1, 2, 80, 24, 1,0 )   ;

  REPEAT
    DoWindow (LightGray, Blue, 2, 3, 79, 22, 0,0 ) ;
    WRITE ( StrL(78,0),S2 )                        ;
    WriteS                                         ;
    WRITE ( NL,S0,S6,StrL(78,1) )                  ;
    DoBottom (7)                                   ;
    Y := UPCASE (READKEY)                          ;
    CASE Y OF
    #00: BEGIN
           X := READKEY                            ;
           CASE X OF #72: { Up Arrow  } DEC (Line) ;
                     #80: { Down Arrow} INC (Line) ; END ;
           IF Line>16 THEN Line:=4                 ;
           IF Line<4  THEN Line:=15                ;
         END                                       ;
    'M','S': BEGIN  { Masterkey + Subkey           }
               REPEAT
                 IF Y='M' THEN DoBottom (8) ELSE DoBottom(9)            ;
                 READLN (NStr)                                          ;
                 N := ExtractNum (NStr)                                 ;
               UNTIL (N<128) AND (N>0)                                  ;
               IF Y='M' THEN RemoteMaster:=N ELSE Remote[Line-3].KEY:=N ;
             END                                                        ;
    'C','P','D': BEGIN
                   DoBottom (10)             ;
                   WRITE ('[',Y,']:')        ;
                   READLN (NStr)             ;
                   IF (LENGTH(NStr)<25) THEN
                   BEGIN
                     IF Y='C' THEN Remote[Line-3].CFG  := NStr ;
                     IF Y='P' THEN Remote[Line-3].PROG := NStr ;
                     IF Y='D' THEN Remote[Line-3].DTA  := NStr ;
                   END                                         ;
                 END                         ;
  END                                        ;
  UNTIL (Y=#27) OR (Y='Q')                   ;
END                                          ;
{-----------------------------------------------}
PROCEDURE DoSearchReplace (CONST Option: BYTE)  ;
{-----------------------------------------------}
CONST
S0 = NL+' End of Search'             ;
S1 =    ' and Replace'               ;
S2 =    ' (No Matches)'              ;
S3 =    ' Matches: '                 ;

VAR
    Count, B, n            : INTEGER                   ;
    A, Diff, Src, Dst, i   : LONGINT                   ;
    Auto, Found            : BOOLEAN                   ;

PROCEDURE FindStr                            ;
VAR B : INTEGER                              ;
BEGIN
    Found := FALSE                                      ;
    REPEAT
      IF (SearchN[1]=XBuff.P^[A]) THEN
      FOR B:=1 TO Count DO Found:=(SearchN[B]=XBuff.P^[A+B-1]) ;
      INC (A)                                                  ;
    UNTIL (A>XBuff.Max) OR Found                               ;

    IF Found THEN BEGIN
                     WRITE ( ' [', A-1:5,']' )                         ;
                     IF (A<XBuff.Max) AND NOT Auto THEN WRITE ('?',BS) ;
                     IF (Search.Count<14) THEN INC(Search.Count)       ;
                     INC (Search.Num)                                  ;
                     Search.Pos [Search.Count] := A-1                  ;
                  END                                                  ;
END                                                                    ;
{----------------------------------------------------------------------}
BEGIN
  DoSubWindow (16)                           ;
  Choice := ' '                              ;

  Count := GetNumberStr ( Option )           ;

  Search.Count := 0                          ;
  Search.Buff  := CurrentBuff                ;
  Search.Num   := 0                          ;

  IF (Count=0) THEN EXIT                     ;

  A    := 0                                  ;
  Auto := (Option=2)                         ;

  IF (Option=0) THEN
  BEGIN
    REPEAT
      FindStr                                        ;
      IF NOT Auto THEN Choice := KeyRead (Choice)    ;
      Auto := (Choice='A')                           ;
    UNTIL (Choice IN ['Q',#27]) OR (A>XBuff.Max)     ;
  END                                                ;

  IF (Option=2) THEN
  BEGIN
    IF (ReplaceN[0]=0)   THEN EXIT                        ;
    IF (ReplaceN[0]=255) THEN ReplaceN[0] := 0            ;
    Diff := ReplaceN[0]-Count                             ;
    REPEAT
      FindStr                                             ;
      IF Found THEN
      BEGIN
        Src := A + Count - 1                                ;
        Dst := Src + Diff                                   ;
        IF (Diff<>0) THEN
        BEGIN
          MOVE ( XBuff.P^[Src], XBuff.P^[Dst], XBuff.Max-Dst+Diff+1)  ;
          i := XBuff.Max + Diff + 1                                   ;
          WHILE (i<=XBuff.Max) DO BEGIN XBuff.P^[i]:=0; INC(i); END   ;
        END                                                           ;
        IF (ReplaceN[0]>0) THEN
        FOR n := 1 TO ReplaceN[0] DO XBuff.P^[A-2+n] := ReplaceN[n]   ;
      END                                                             ;
    UNTIL (A>XBuff.Max)                           ;
  END                                             ;

  WRITE (S0)                                      ;
  IF (Option=2) THEN WRITE (S1)                   ;
  IF (Search.Num=0) THEN WRITE (S2)
                    ELSE WRITE (S3,Search.Num)    ;
  Choice := KeyRead (Choice)                      ;
END                                               ;
{--------------------------------------------}
PROCEDURE DoReplace                          ;
{--------------------------------------------}
BEGIN
    ;
END ;
{----------------------------------------MAIN---------------}
BEGIN
  CASE Option OF
       'S','R'  : DoSearchReplace (0 + 2*BYTE(Option='R'));
       'B'      : BEGIN
                    Q.BlankChar := GetChar (0)  ;
                    TEXTBACKGROUND (Cyan)     ;
                    GOTOXY (11,6) ; WRITE ( CHR(Q.BlankChar) ) ;
                    GOTOXY (14,6) ; WRITE ( Q.BlankChar:3    ) ;
                  END                         ;
       'F'      : BEGIN
                    Q.FillChar := GetChar (1)   ;
                    TEXTBACKGROUND (Cyan)     ;
                    GOTOXY (11,7) ; WRITE ( CHR(Q.FillChar)  ) ;
                    GOTOXY (14,7) ; WRITE ( Q.FillChar:3     ) ;
                  END                         ;
       '2'      : BEGIN
                    DisplayFKeys              ;
                  END                         ;
       '1'      : BEGIN
                    DefFileKeys               ;
                  END                         ;
       '3'      : BEGIN
                    DisplayIKeys              ;
                  END                         ;
       '4'      : BEGIN
                    DisplayRemoteKeys         ;
                  END                         ;
       'E'      : BEGIN
                    DisplayHexData  ;
                  END               ;
       'X'      : BEGIN
                    DoSubWindow (16)             ;
                    WRITE (NL, ' Busy...', BS8 ) ;
                    SysExCheckSum (0)            ;
                  END                            ;
       'C'      : BEGIN
                    REPEAT
                      j := GetChar (2)                                     ;
                      GOTOXY (13,12) ; WRITE (CHR(Q.FillChar))               ;
                      GOTOXY (15,12) ; WRITE (Q.FillChar)                    ;
                      ClearL (14)                                          ;
                      READLN (NumberStr)                                   ;
                      NumOK := GetNum ( NumberStr, BFrom, BTo )            ;
                    UNTIL NumOK                                            ;
                    FOR n:= BFrom TO BTo DO XBuff.P^[n] := Q.FillChar         ;
                  END ;
       'D'      : BEGIN
                    REPEAT
                      j := GetChar (3)                                   ;
                      ClearL (14)                                        ;
                      READLN (NumberStr)                                 ;
                      NumOK := GetNum ( NumberStr, BFrom, BTo )          ;
                    UNTIL NumOK                                          ;
                    N := 0 ; MStr := '0000' ; M := 0                     ;
                    FOR k := BFrom TO BTo DO
                                            N := N + XBuff.P^[k]              ;
                    IF N > $FFFF THEN BEGIN M := N DIV 65536             ;
                                            X := N MOD 65536             ;
                                            MStr := DecToHex (M)         ;
                                            NStr := DecToHex (X) ; END
                                 ELSE BEGIN X := N                       ;
                                            NStr := DecToHex (X) ; END   ;
                    ClearL (14)                               ;
                    WRITE ('Checksum:' ,NL)                   ;
                    ClearL (15)                               ;
                    WRITE ('HEX: ', MStr, Sp, NStr, NL)       ;
                    ClearL (16)                               ;
                    WRITE ('DEC: ', N)                        ;
                  END                                         ;
   END                                                        ;
   IF Option IN ['B','F','D','X'] THEN Y := KeyRead (Y)       ;
 END                                                          ;
