{$S-,R-,O-}
{-------------------------}
{ GOBAL VARIABLES         }
{-------------------------}
UNIT Midiprg1             ; { MIDIPRG1.PAS }

{-----------------------------}
{ INCLUDE FILES: MIDIPRG7.INC } { Editor + Prgmble. Keys }
{                MIDIPRGD.INC } { ParamStr Check         }
{-----------------------------}

INTERFACE

USES CRT, DOS     ;

{$I PATCH\MIDIPRGF.INC} { GLOBALS }

      PROCEDURE ResetBuff ( Option : BYTE )                           ;
      FUNCTION  PressedKey : BOOLEAN                                  ;
      PROCEDURE GetKey                                                ;
      FUNCTION  GetMaxBuffSize : LONGINT                              ;
      PROCEDURE GotoColorStr (CONST Color,X,Y:INTEGER; CONST S:STRING);
      FUNCTION  SetXBuff   : BOOLEAN                ;
      FUNCTION  SeekManuf ( CONST Num:BYTE) : Str16 ;
      PROCEDURE GetBuffClear                        ;
      PROCEDURE DoWindow ( CONST BC,TC,X1,Y1,X2,Y2: INTEGER ;
                           CONST Option1, Option2: BYTE)    ;
      PROCEDURE GetBuff                                 ;
      PROCEDURE MakeCurrentBuff                         ;
      PROCEDURE GetInPort ( VAR P: WORD )               ;
      PROCEDURE GetOutPort ( VAR P: POutType )          ;
      PROCEDURE WritePortOut ( CONST P: POutType; CONST S : Str1 )  ;
      FUNCTION  PortOutOK ( CONST P:POutType) : BOOLEAN             ;
      FUNCTION  PortInOK  ( CONST P:WORD)     : BOOLEAN             ;
      PROCEDURE WriteSides ( CONST X1, X2, Y1, CountY: INTEGER)     ;
      PROCEDURE DoSubWindow ( CONST YSize: INTEGER )       ;
      FUNCTION  DecToHex (CONST Number : WORD ) : str10    ;
      FUNCTION  HexToDec ( CONST HexStr : str10)    : WORD ;
      PROCEDURE SysExCheckSum ( Option : BYTE )            ;
      FUNCTION  StrL ( CONST NumChr, TopBottom: BYTE) : Str80 ;
      FUNCTION  KeyRead ( Key : CHAR ) : CHAR         ;
      PROCEDURE ClearFilterLists                      ;
      PROCEDURE ClearBuffer ( VAR B:BuffType )        ;
      PROCEDURE ClearRemote                           ;
      PROCEDURE ClearFLinkList                        ;
      PROCEDURE ClearMidiStr                          ;
      PROCEDURE ClearFKeyList                         ;
      PROCEDURE ResetChannelMatrix                    ;
      PROCEDURE InitialiseHWConfig(CONST Option: BYTE);
      PROCEDURE WritePortCfg  ( Y: INTEGER)           ;
      PROCEDURE Initialise8250Baud                    ;
      PROCEDURE InitialiseCAddr                       ;
      PROCEDURE CheckParamStr                         ;
      FUNCTION  CheckFileSize ( NameStr : STRING ) : LONGINT    ;
      FUNCTION  GetNumberStr (CONST Source:BYTE)   : INTEGER    ;
      PROCEDURE DoFError ( FError: INTEGER ; Name : STRING; Size: LONGINT ) ;
      PROCEDURE DTALoad  ( NStr: STRING; VAR Size: LONGINT; Pos: WORD )     ;
      PROCEDURE DTASave  ( NStr: STRING; VAR Size: LONGINT; Pos: WORD )     ;
      PROCEDURE KEYLoad  ( Option: BYTE; NStr: Str24; VAR Size: LONGINT  ) ;
      PROCEDURE KEYSave  ( Option: BYTE; NStr: Str24; VAR Size: LONGINT  ) ;
      PROCEDURE CFGLoad  ( NStr: STRING; VAR Size: LONGINT )    ;
      PROCEDURE CfGSave  ( NStr: STRING; VAR Size: LONGINT )    ;
      PROCEDURE DisplayHexData                                  ;
      PROCEDURE HexDump ( First, Last : WORD; Blank, ShowSelect : BOOLEAN)  ;
      PROCEDURE Edit (Option : CHAR )                           ;

IMPLEMENTATION
{----------------------------------}
FUNCTION PressedKey : BOOLEAN      ;
{----------------------------------}
BEGIN
  KBrd.On := FALSE                 ;
  INLINE ( $B8/$00/$11                { MOV AX, $11    }
          /$CD/$16                    { INT 16H        }
          /$74/$05                    { JZ NoChar      }
          /$B0/$01                    { MOV AL, $01    }
          /$A2/KBrd                   { MOV KBrd.On, AL}
          /$90              )      ;  {                }
  PressedKey := KBrd.On            ;
END                                ;
{----------------------------------}
PROCEDURE GetKey                   ;
{----------------------------------}
VAR n : INTEGER                    ;

(* KBrd:RECORD  0: Ascii : BYTE    ;
                   Scan  : BYTE    ;
                   Class : BYTE    ;
                   FKey  : BYTE    ;
                   UpCh  : CHAR    ;
                1: Val   : WORD    ;
                2: Ch    : CHAR    ;
                3: On    : BOOLEAN ; *)

CONST F1: ARRAY [1..75] OF BYTE =
    ( 129,120,121,122,123,124,125,126,127,128,            { A 0 - 9  }
      30 ,48 ,46 ,32 ,18 ,33,                             { A A - F  }
      59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,67 ,68 ,133,134,    { N F1-F12 }
      84 ,85 ,86 ,87 ,88 ,89 ,90 ,91 ,92 ,93 ,135,136,    { S F1-F12 }
      94 ,95 ,96 ,97 ,98 ,99 ,100,101,102,103,137,138,    { C F1-F12 }
      104,105,106,107,108,109,110,111,112,113,139,140,    { A F1-F12 }
      71 ,79 ,81 ,73,                                     { Home,End,PD,PU  }
      75 ,77 ,72 ,80,                                     { ArL,ArR,ArU,ArD }
      82 ,83 ,15                                     ) ;  { Ins, Del, BS    }


     F2: ARRAY [1..75] OF BYTE =
   ( 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
     1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8,9,10,11,12,
     1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8,9,10,11,12,
     0,1,2,3,
     4,5,6,7,
     8,9,10 ) ;

     F3: ARRAY [1..75] OF BYTE =
   ( 14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
     10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,
     12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,
     2,2,2,2,
     1,1,1,1,
     3,3,3 ) ;
BEGIN
     INLINE ( $B8/$00/$10                { MOV AX, $00    }
             /$CD/$16                    { INT 16H        }
             /$A3/KBrd                   { MOV KeyBrd, AX }
             /$90              ) ;       {                }

    IF KBrd.Ch IN ['a'..'z'] THEN KBrd.UpCh := CHAR(KBrd.Ascii-32) ELSE
    KBrd.UpCh := KBrd.Ch                                              ;

    n          := 1                      ;
    KBrd.Class := 0                      ;

    IF NOT (Kbrd.On) THEN
    BEGIN
      WHILE (KBrd.Scan<>F1[n]) AND (n<76) DO INC (n) ;
      IF (n<76) THEN BEGIN
                       KBrd.Pos   := n-1   ;
                       KBrd.Class := F3[n] ;
                       KBrd.FKey  := F2[n] ;
                     END                   ;
    END                                    ;
END                                        ;
PROCEDURE GotoColorStr (CONST Color, X, Y: INTEGER; CONST S:STRING) ;
{-------------------------------------------------------------------}
BEGIN
  TEXTATTR := (TEXTATTR AND $F0) + Color ;  { 1111 0000 }
  GOTOXY (X,Y)                           ;
  WRITE  (S)                             ;
END                                      ;
{--------------------------------------------}
FUNCTION SeekManuf (CONST Num:BYTE) : Str16  ;
{--------------------------------------------}
BEGIN
  SeekManuf := 'UNKNOWN'               ;
  FOR i := 0 TO 14 DO
  IF Num = ManufacturerNumber[i] THEN
     SeekManuf := ManufacturerName[i]  ;
END                                    ;
{------------------------------------------------------}
FUNCTION StrL ( CONST NumChr, TopBottom: BYTE) : Str80 ;
{------------------------------------------------------}
VAR   LStr       : Str80      ;
CONST TopLeft    : CHAR = 'Ú' ;
      TopRight   : CHAR = '¿' ;
      BottomLeft : CHAR = 'À' ;
      BottomRight: CHAR = 'Ù' ;
      LChr       : CHAR = 'Ä' ;
BEGIN
  FOR i := 1 TO 80 DO LStr[i] := LChr         ;
  LStr[0] := CHR(NumChr-2)                    ;
  CASE TopBottom OF
  0 : LStr := TopLeft + LStr + TopRight       ;
  1 : LStr := BottomLeft + LStr + BottomRight ;
  END                                         ;
  StrL := LStr                     ;
END                                ;
{-----------------------------------}
PROCEDURE GetInPort ( VAR P: WORD ) ;
{-----------------------------------}
VAR Y : CHAR                             ;
BEGIN
  Y := KeyRead(Y)                        ;
  IF (Y='A') AND PortA THEN P := P XOR 1 ;
  IF (Y='B') AND PortB THEN P := P XOR 2 ;
END                                      ;
{------------------------------------}
PROCEDURE GetOutPort (VAR P:POutType);
{------------------------------------}
VAR Y : CHAR                         ;
BEGIN
  Y := KeyRead (Y)                   ;
  CASE Y OF
  'A': P.A := P.A XOR BYTE(PortA)    ;
  'B': P.B := P.B XOR BYTE(PortB)    ;
  'C': P.C := P.C XOR BYTE(PortC)    ;
  'D': P.D := P.D XOR BYTE(PortD)    ;
  END                                ;
END                                  ;
{-------------------------------------------------------------}
PROCEDURE WritePortOut ( CONST P: POutType; CONST S : Str1 )  ;
{-------------------------------------------------------------}
VAR L : Str10                                                 ;
BEGIN
  L := ''                    ;
  TEXTCOLOR (LightRed)       ;
  IF P.A>0 THEN L :=   'A'+S ;
  IF P.B>0 THEN L := L+'B'+S ;
  IF P.C>0 THEN L := L+'C'+S ;
  IF P.D>0 THEN L := L+'D'+S ;
  WRITE (L)                  ;
  TEXTCOLOR (Black)          ;
END                                                  ;
{------------------------------------------------------}
FUNCTION PortInOK ( CONST P:WORD) : BOOLEAN            ;
{------------------------------------------------------}
BEGIN
  PortInA  := (P AND 1>0) AND PortA         ;
  PortInB  := (P AND 2>0) AND PortB         ;
  PortInOK := (PortInA OR PortInB)          ;
END                                         ;
{------------------------------------------------------}
FUNCTION PortOutOK (CONST P:POutType) : BOOLEAN        ;
{------------------------------------------------------}
BEGIN
   PortOutA  := ((P.A>0) AND PortA) ;
   PortOutB  := ((P.B>0) AND PortB) ;
   PortOutC  := ((P.B>0) AND PortC) ;
   PortOutD  := ((P.B>0) AND PortD) ;
   PortOutOK := ( PortOutA OR PortOutB OR PortOutC OR PortOutD ) ;
END                                                              ;
{-------------------------------------------------------------------------}
PROCEDURE DoWindow (CONST BC, TC, X1, Y1, X2, Y2: INTEGER ;
                    CONST Option1, Option2: BYTE )        ;
{-------------------------------------------------------------------------}
BEGIN
  IF Option2>0 THEN
  DoWindow (Black,Black,X1+1,Y1+(2-Option2),X2+1,Y2+1,1,0);
  TEXTATTR := TC + 16*BC                                  ;
  WINDOW (X1,Y1,X2,Y2)                                    ;
  IF Option1=1 THEN CLRSCR                                ;
END                                                       ;
{----------------------------------------------------------}
PROCEDURE WriteSides (CONST X1, X2, Y1, CountY : INTEGER ) ;
{----------------------------------------------------------}
VAR i : INTEGER                                            ;
BEGIN
  FOR i := 0 TO CountY-1 DO
  BEGIN
    GOTOXY (X1,Y1+i) ; WRITE ('³') ;
    GOTOXY (X2,Y1+i) ; WRITE ('³') ;
  END                              ;
END                                ;
{-----------------------------------------------}
PROCEDURE DoSubWindow ( CONST YSize: INTEGER)   ;
{-----------------------------------------------}
BEGIN
  DoWindow (Cyan     , Blue , 1 ,2,80,24      ,1,0) ;
  DoWindow (LightGray, Black, 9 ,3,76,4+YSize ,1,1) ;
  DoWindow (LightGray, Black, 9 ,3,77,4+YSize ,0,0) ;
  WRITE (StrL(68,0),NL)                             ;
  WriteSides (1, 68, 2, YSize )                     ;
  WRITE (NL, StrL(68,1))                            ;
  DoWindow (LightGray, Blue, 11,4,74,3+YSize,1,0)   ;
END                                                 ;
{-----------------------------------}
{ Convert Number to Hex String      }
{-----------------------------------}
FUNCTION DecToHex (CONST Number: WORD ) : str10 ;
VAR DH : Str10                                  ;
BEGIN
  DH[1] :=  HexDigit [ HI (Number) DIV 16 ]     ;
  DH[2] :=  HexDigit [ HI (Number) MOD 16 ]     ;
  DH[3] :=  HexDigit [ LO (Number) DIV 16 ]     ;
  DH[4] :=  HexDigit [ LO (Number) MOD 16 ]     ;
  DH[0] :=  CHR (4)                             ; { Set Length = 4 }
  IF (Number<256) THEN DH := COPY (DH,3,2)      ; { Set Length = 2 }
  DecToHex := DH                                ;
END                                             ;
{-----------------------------------}
{ Convert Hex String to Number      }
{-----------------------------------}
FUNCTION HexToDec ( CONST HexStr : str10) : WORD ;
VAR
  Number                  : WORD    ;
  E                       : INTEGER ;
BEGIN
  VAL ('$'+ HexStr, Number, E)      ;
  IF E>0 THEN Number := 0           ;
  HexToDec := Number                ;
END                                 ;
{--------------------------------------------}
{ Clear Buffer                               }
{--------------------------------------------}
PROCEDURE ClearBuffer ( VAR B: BuffType )    ;
VAR i : WORD                                 ;
BEGIN
  FOR i := 0 TO B.Max DO B.P^[i] := 0        ;
  B.Start := 0                               ;
  B.Size  := 0                               ;
  B.Stop  := 0                               ;
  B.Pos   := 0                               ;
END                                          ;
{------------------------------------}
PROCEDURE GetBuffClear               ;
{------------------------------------}
VAR Y : CHAR                         ;
BEGIN
  Y := KeyRead (Y)                   ;
  CASE Y OF
  'N': ClearBuffer (NBuff)           ;
  'O': ClearBuffer (OBuff)           ;
  'S': ClearBuffer (SBuff)           ;
  'A': BEGIN
         ClearBuffer (NBuff)         ;
         ClearBuffer (OBuff)         ;
         ClearBuffer (SBuff)         ;
       END                           ;
  END                                ;
END                                  ;
{------------------------------------}
PROCEDURE GetBuff                    ;
{------------------------------------}
VAR Y : CHAR                         ;
BEGIN
  Y := KeyRead (Y)                   ;
  CASE Y OF
  'N': BuffN := BuffN XOR TRUE       ;
  'O': BuffO := BuffO XOR TRUE       ;
  'S': BuffS := BuffS XOR TRUE       ;
  END                                ;
END                                  ;
{------------------------------------}
PROCEDURE MakeCurrentBuff            ;
{------------------------------------}
VAR Y : CHAR                         ;
BEGIN
  Y := KeyRead (Y)                   ;
  CASE Y OF
  'N': IF BuffN THEN XBuff := NBuff  ;
  'O': IF BuffO THEN XBuff := OBuff  ;
  'S': IF BuffS THEN XBuff := SBuff  ;
  END                                ;
  CurrentBuff := XBuff.Num           ;
END                                  ;

{-----------------------------------}
PROCEDURE ResetBuff ( Option: BYTE) ;
{-----------------------------------}
BEGIN
  CASE  (CurrentBuff=StartBuff) OF
  TRUE : CASE StartBuff OF 1:NBuff.Pos := XBuff.Pos ;
                           2:OBuff.Pos := XBuff.Pos ;
                           3:SBuff.Pos := XBuff.Pos ;
         END                                        ;
  FALSE: CASE StartBuff OF 1:BEGIN
                               IF (Option=1) THEN
                               BEGIN
                                  NBuff.Pos := NBuff.Max                    ;
                                  OBuff.Pos := OBuff.Max                    ;
                               END                                          ;
                               IF CurrentBuff=2 THEN OBuff.Pos := XBuff.Pos ;
                               IF CurrentBuff=3 THEN SBuff.Pos := XBuff.Pos ;
                               XBuff := NBuff                               ;
                             END                                            ;
                           2:BEGIN
                               IF (Option=1) THEN
                               BEGIN
                                  OBuff.Pos := OBuff.Max ;
                                  SBuff.Pos := XBuff.Pos ;
                               END                       ;
                               XBuff     := OBuff        ;
                             END                         ;
         END                                             ;
  END                                                    ;
  CurrentBuff := StartBuff                               ;
END                                                      ;
{-------------------------------------------------------}
FUNCTION SetXBuff : BOOLEAN                             ;
{-------------------------------------------------------}
CONST S = NL+' Warning:[NOS=OFF]:No Buffer(s) selected.';
VAR BuffOK : BOOLEAN                                    ;
PROCEDURE DoBuff (VAR B:BuffType; BOn:BOOLEAN; N:BYTE ) ;
BEGIN
  B.Num       := N        ;
  BuffSeq[N] := BYTE(BOn) ;
END                       ;
BEGIN
   DoBuff (SBuff, BuffS, 3)              ;
   DoBuff (OBuff, BuffO, 2)              ;
   DoBuff (NBuff, BuffN, 1)              ;
   BuffOK := (BuffN OR BuffO OR BuffS)   ;
   SetXBuff := BuffOK                    ;
   IF BuffOK THEN EXIT                   ;
   WRITE (S)                             ;
   DELAY (1000)                          ;
END                                      ;
{-----------------------------------------}
FUNCTION NextBuff : BOOLEAN               ;
{-----------------------------------------}
VAR BuffAvail : BOOLEAN                   ;
BEGIN
  CASE CurrentBuff OF
  3: BuffAvail := FALSE                    ;
  1: BEGIN
       IF BuffSeq[3]=1 THEN XBuff := SBuff ;
       IF BuffSeq[2]=1 THEN XBuff := OBuff ;
       BuffAvail := (BuffO OR BuffS)       ;
     END                                   ;
  2: BEGIN
       IF BuffSeq[3]=1 THEN XBuff := SBuff ;
       BuffAvail := BuffS                  ;
     END                                   ;
  END                                      ;
  IF BuffAvail THEN INC (CurrentBuff)      ;
  NextBuff := BuffAvail                    ;
END                                        ;
{-------------------------------------------------------}
FUNCTION GetMaxBuffSize : LONGINT                       ;
{-------------------------------------------------------}
BEGIN
  MaxBuffSize := BYTE(BuffN)*NBuff.Max +
                 BYTE(BuffO)*OBuff.Max +
                 BYTE(BuffS)*SBuff.Max                  ;
  GetMaxBuffSize := MaxBuffSize                         ;
END                                                     ;
{-----------------------------------------}
PROCEDURE SysExCheckSum ( Option : BYTE ) ;
{-----------------------------------------}
CONST S = NL+' NO  Start    End   Size  CheckSum  Buff';
{              12  50000  50000  50000  10000000     X }
TYPE CSType = RECORD
                Start     : WORD    ;
                Stop      : WORD    ;
                Size      : LONGINT ;
                CSum      : LONGINT ;
                BuffStart : BYTE    ;
                BuffStop  : BYTE    ;
              END                   ;

VAR SysCS         : ARRAY [1..12] OF CSType ;
    i, n, PrevMax : WORD                    ;
    SysExDetect   : BOOLEAN                 ;
    Total         : LONGINT                 ;
PROCEDURE DoCount                           ;
BEGIN
  REPEAT
   i := 0       ;
   REPEAT
    IF (XBuff.P^[i]=$F7) AND SysExDetect THEN
    BEGIN
      SysCS[n].Stop      := i                                             ;
      IF CurrentBuff=SysCS[n].BuffStart THEN
           SysCS[n].Size := SysCS[n].Stop - SysCS[n].Start + 1
      ELSE SysCS[n].Size := SysCS[n].Stop + (PrevMax-SysCS[n].Start) + 2  ;
      SysCS[n].BuffStop  := CurrentBuff                                   ;
      SysExDetect        := FALSE                                         ;
      Total := Total + SysCS[n].CSum                                      ;
      INC (n)                                                             ;
    END                                                                   ;
    IF SysExDetect AND (XBuff.P^[i]<$80) THEN
       SysCS[n].CSum := SysCS[n].CSum + XBuff.P^[i]                       ;
    IF XBuff.P^[i] = $F0 THEN
    BEGIN
      SysCS[n].Start     := i           ;
      SysCS[n].CSum      := 0           ;
      SysCS[n].BuffStart := CurrentBuff ;
      SysExDetect        := TRUE        ;
      PrevMax            := XBuff.Max   ;
    END                         ;
    INC (i)                     ;
  UNTIL (n>12) OR (i>XBuff.Max) ;
  i := 0                        ;
 UNTIL (Option=1) OR NOT NextBuff OR (n>12)   ;
END                             ;
{-------------------------------}
BEGIN
  IF NOT SetXBuff THEN EXIT                                ;
  IF (Option=0) THEN StartBuff := CurrentBuff              ;
  IF (Option=1) AND (CurrentBuff>StartBuff) THEN Option:=0 ;
  n     := 1                                               ;
  Total := 0                                               ;
  SysExDetect := FALSE                                     ;
  DoCount                                                  ;

  IF (n>1) THEN
  BEGIN
    WRITE (S)                                                       ;
    TEXTCOLOR (Blue)                                                ;
    FOR i := 1 TO n-1 DO
    BEGIN
       WRITE ( NL, i:3, SysCS[i].Start:7 )                          ;
       IF (i=n) AND SysExDetect THEN WRITE ('     ?')
       ELSE BEGIN
              WRITE ( SysCS[i].Stop:7 )                             ;
              WRITE ( SysCS[i].Size:7, '  [',SysCS[i].CSum:7,']'  ) ;
              WRITE ( ' ', BuffChr1 [ SysCS[i].BuffStart ]      )   ;
              IF (SysCS[i].BuffStart<>SysCS[i].BuffStop) THEN
              WRITE ( ':',BuffChr1 [ SysCS[i].BuffStop ] )          ;
            END                                                     ;
    END                                                             ;
    WRITE (NL+'  Total SysEx CheckSum :  [', Total:7, ']')          ;
    TEXTCOLOR (Black)                                               ;
  END ELSE WRITE (NL,' No SYSEX Found..' )                          ;
  ResetBuff (0)                                                     ;
END                                                                 ;
{--------------------------------------}
{ Clear #00 from Key Read              }
{--------------------------------------}
FUNCTION KeyRead ( Key : CHAR ) : CHAR ;
BEGIN
  Key := UPCASE (READKEY)              ;
  IF Key=#00 THEN BEGIN
                    Key := READKEY     ;
                    Key := ' '         ;
                   END                 ;
  KeyRead := Key                       ;
END                                    ;
{--------------------------------------}
PROCEDURE InitOptions2                 ;
{--------------------------------------}
VAR i : INTEGER                  ;
BEGIN
   CIn  := C[2].PortIn           ;
   COut := C[3].PortIn           ;
   FOR i := 4 TO 13 DO
   Q.ClearQ[i-4] := C[i].PortIn  ;
END                              ;
{--------------------------------------}
PROCEDURE InitOptions3                 ;
{--------------------------------------}
VAR i : INTEGER                 ;
BEGIN
   C[2].PortIn  := CIn          ;
   C[3].PortIn  := COut         ;
   FOR i := 4 TO 13 DO
   C[i].PortIn := Q.ClearQ[i-4] ;
END                             ;
{-------------------------------------------------}
{ Clear all Filter Lists                          }
{-------------------------------------------------}
PROCEDURE ClearFilterLists                        ;
VAR i : INTEGER                                   ;
BEGIN
  FOR i:=0 TO 15 DO FILLCHAR (C[i].FList[0],18,0) ;
END                                               ;
{----------------------------------}
{ Clear Remote File link Lists     }
{----------------------------------}
PROCEDURE ClearRemote              ;
VAR i : INTEGER                    ;
BEGIN
  RemoteMaster := 01      ; { 2nd lowest key }
  FOR i := 1 TO 12 DO
  BEGIN
    Remote[i].KEY  := i   ;
    Remote[i].CFG  := Sp  ;
    Remote[i].DTA  := Sp  ;
    Remote[i].PROG := Sp  ;
  END                     ;
END                       ;
{------------------------------------}
{ Clear all File link Key Lists      }
{------------------------------------}
PROCEDURE ClearFLinkList             ;
VAR i : INTEGER                      ;
BEGIN
  FILLCHAR (FSizeArr[1],26,0)        ;
  FOR i := 1 TO 26 DO FLink[i]:=Sp23 ;
END                                  ;
{----------------------------------}
{ Clear all MidiStr Lists          }
{----------------------------------}
PROCEDURE ClearMidiStr             ;
VAR i : INTEGER                    ;
BEGIN
  FOR i := 0 TO 15 DO
  BEGIN
    C[i].MidiStrX := 0             ;
    FILLCHAR (C[i].MidiStr[0],18,0);
  END                              ;
END                                ;
{----------------------------------}
{ Clear all F Key Buffer Lists     }
{----------------------------------}
PROCEDURE ClearFKeyList            ;
BEGIN
  FILLCHAR (FKey.N.Start[1],12,0)  ;
  FILLCHAR (FKey.N.Stop[1] ,12,0)  ;
  FKey.C := FKey.N                 ;
  FKey.S := FKey.N                 ;
  FKey.A := FKey.N                 ;
END                                ;
{----------------------------------}
{ Reset Channel Records to Default }
{----------------------------------}
PROCEDURE ResetChannelMatrix       ;
VAR i, j : INTEGER                 ;
BEGIN
  FOR i := 0 TO 15 DO
  BEGIN
    FILLCHAR (C[i].ClearCFGRec[0],488,0) ;
    C[i].ChX    := 1               ;
    C[i].Ch[i]  := 1               ;
    C[i].ChMessX:= 5               ;
    C[i].ChMessB0.COut1 := 255     ;
    C[i].ChMessB0.COut2 := 255     ;
    C[i].SysPortOut.A   := 1       ;
    C[i].PortIn         := 1       ;
    C[i].PatchCOut[0] := 1         ;
    C[i].ChMess.A0[i] := 1         ;
    C[i].ChMess.B0[i] := 1         ;
    C[i].ChMess.C0[i] := 1         ;
    C[i].ChMess.D0[i] := 1         ;
    C[i].ChMess.E0[i] := 1         ;
    FOR j := 0 TO 127 DO C[i].PatchXlat[j] := j ;
    FOR j := 0 TO 15  DO C[i].PortOut[j].A := 1 ;
  END                        ; { i Loop }
  ClearMidiStr               ;
END                          ;
{-------------------------------------------------------------}
{ Do Hardware Configuration                                   }
{-------------------------------------------------------------}
PROCEDURE  InitialiseHWConfig  (CONST Option: BYTE )          ;
PROCEDURE  Do4Ports ( VAR P:PortType; N:BYTE )                ;
VAR i : INTEGER                                               ;
BEGIN
  IF Option=1 THEN       { Only if problem with HW Config File }
  BEGIN
    { Initialise P1=COM1:IRQ3, P2=COM2:IRQ4, P3=COM3:IRQ5, P4=COM4:IRQ6}
    P.INTNumber := INTArr[N+2] ; { INT $0B = hardware IRQ03 (COM2)     }
                                 { INT $0C = hardware IRQ04 (COM1)     }
    P.IRQ       := N+2         ; { IRQ 3,4,5,7 in here                 }
    Xtal        := $02         ; { 10 MHz Xtzl default                 }
    P.SERPort   := SERArr[N-1] ; { COM1-COM4 $3F8,$2F8,$3E8,$2E8       }
    P.SEREnable := TRUE        ;
    P.SERNum    := N           ;
  END                          ;

  FOR i := 0 TO 7 DO
  P.ClearPort[i] := P.SERPort + i ;
END                               ;
BEGIN
  Do4Ports(P1,1) ; Do4Ports(P2,2) ;
  Do4Ports(P3,3) ; Do4Ports(P4,4) ;
END                               ;
{---------------------------------------------------------}
PROCEDURE WritePortCfg (Y : INTEGER)                      ;
{---------------------------------------------------------}
CONST
    S0 = '   COM IOAddr IRQ Ena Inst '+NL ;
{    S2 = '³A:[ ] [   ]  [ ]         ³'+NL ; }
VAR AStr, NStr : Str10                    ;
    Num        : WORD                     ;
PROCEDURE WriteX (P:PortType; PortX:BOOLEAN; S:CHAR) ;
BEGIN
  GoToColorStr (Blue, 2, Y+ORD(S)-63, S)             ;
  WRITE (':[',P.SERNum,'] ')                         ;
  TEXTCOLOR (Red)                                    ;
  Num  := P.SERPort                                  ;
  NStr := '[' + COPY (DecToHex(Num), 2, 3) + ']  ['  ;
  AStr := '] ' + OnOff2[BYTE(P.SEREnable)]           ;
  AStr := AStr + ' ' + OnOff2[BYTE(P.SERDetect)]     ;
  WRITE ( NStr, P.IRQ, AStr)                         ;
END                                                  ;
BEGIN
  GOTOXY (1,Y)                                    ;
  TEXTATTR := LightGrayBlack                      ;
  WRITE (S0,StrL(27,0))       ;
  WriteSides (1, 27, Y+2, 4 ) ;
  WRITE (NL,StrL(27,1))       ;

  WriteX (P1, PortA,'A')                      ;
  WriteX (P2, PortB,'B')                      ;
  WriteX (P3, PortC,'C')                      ;
  WriteX (P4, PortD,'D')                      ;
END                                           ;
{---------------------------------------------------------}
{ Initialise 8250 to MidiBaudRate                         }
{---------------------------------------------------------}
PROCEDURE Initialise8250Baud                              ;
VAR A, B, C, n : BYTE                                     ;
{---------------------------------------------------------}
PROCEDURE Do4Ports (VAR P: PortType; VAR PortX: BOOLEAN ) ;
{---------------------------------------------------------}
VAR i : INTEGER                       ;
BEGIN
  {-----------------------------------}
  { Check if Port physically present  }
  {-----------------------------------}
  n := 0                              ;
  C := $00                            ;
  P.SERUartType := 0                  ;
  REPEAT
    A := PORT[P.SERIntE]              ;
    INC (n)                           ;
    PORT[P.SERIntE] := $0F            ;
    DELAY (1)                         ;
    B := PORT[P.SERIntE]              ;
    PORT[P.SERIntE] := A              ;
    PORT [P.SERTempPad] := $AA        ; { 1010 1010 }
    DELAY (1)                         ;
    C := PORT [P.SERTempPad]          ;
    IF C=$AA THEN INC (P.SERUartType) ;
  UNTIL (B=$0F) OR (n=10)             ;

  IF (n=10) THEN P.SERDetect := FALSE
  ELSE INC (PortNum)                  ;
  {-----------------------------------}
  { Configure Ports                   }
  {-----------------------------------}
  IF P.SEREnable THEN
  BEGIN
    PORT [P.SERLineCtrl] := $80       ; { Set DLAB = 1: Access Baud Reg   }
    PORT [P.SERBaudLo]   := Xtal      ; { 01=5MHz 02=10MHz Xtal           }
    PORT [P.SERBaudHi]   := $00       ;
    PORT [P.SERLineCtrl] := $03 {07}  ; { Set DLAB = 0: Access normal Reg }
                                        { Bit0,1:11=8bit length           }
                                        { Bit2  :1=2stop bits, 0=1stop bit}
    PORT [P.SERModemCtrl]:= $00       ; { Set OUT2=0 : Master INT Disable }
                                      ; { $3xC: 0000 1000: DTR,RTS,OUT2 Lo}
                                        { (OUT1=D2)        D0  D1  D3     }
    PORT [P.SERIntE]     := $01       ; { Bit0=1: INT via receive data    }
    FOR i := 0 TO 7 DO
        A := PORT [P.SERData+i]       ; { Reset all Ports by reading      }
  END                                 ;
  PortX := (P.SEREnable AND P.SERDetect)             ;
END                                                  ;
BEGIN
  PortNum := 0        ;
  Do4Ports (P1,PortA) ;
  Do4Ports (P2,PortB) ;
  Do4Ports (P3,PortC) ;
  Do4Ports (P4,PortD) ;
END                   ;
{-------------------------------}
{ Fill CAddr with C[x]addresses.}
{-------------------------------}
PROCEDURE InitialiseCAddr       ;
VAR i : BYTE                    ;
BEGIN
  FOR i  := 0 TO 15 DO CAddr[i] := OFS(C[i]) ;
END                                          ;

{----------------------------------------------------}
{ Parse Numberstring                                 }
{----------------------------------------------------}
FUNCTION GetNumberStr (CONST Source:BYTE): INTEGER   ;

CONST
   DNum : SET OF CHAR =  ['0'..'9']                                   ;
   HNum : SET OF CHAR =  ['0'..'9','A'..'F','a'..'f','$','.','H','h'] ;
   Sep  : SET OF CHAR =  [' ' ,  ',' , ';' , '/' , '\' , ':']         ;
   Qt   : SET OF CHAR =  ['Q','q']                                     ;
   S0   = NL+' Search: Max 12 Numbers (HNum=Hex) [N]ext [A]uto [Q]uit.'+
          NL+' STRING: '                                               ;
   S1   = NL+' Code(s):  '                                             ;
   S2A  = NL+' Search : Max 12 Numbers (HNum=Hex).'+
          NL+' STRING : '                                               ;
   S2B  = NL+' Replace: Max 12 Numbers (HNum=Hex) [ENTER]=nothing.'+
          NL+' STRING : '                                               ;
   E1   = NL+' Non Numeric characters entered..'           ;
   E2   = NL+' Number(s) between 0-255 (HEX:00-FF) only.'  ;
   E3   = NL+' Returning to Main Menu..'                   ;
   E4   = NL+' Line too long...'                           ;

VAR
    IStr     : STRING [60]              ;
    NStr     : Str4                     ;
    TempN    : ArrI16                   ;
    NArr     : ARRAY [0..15] OF Str4    ;
    E, F     : INTEGER                  ;
    i, j, k  : INTEGER                  ;
    Len      : INTEGER                  ;

BEGIN
REPEAT
  E := 0                                ;
  F := 0                                ;

  IF (Source=0) THEN WRITE ( S0  )      ;
  IF (Source=1) THEN WRITE ( S1  )      ;
  IF (Source=2) THEN WRITE ( S2A )      ;
  IF (Source=3) THEN WRITE ( S2B )      ;

  READLN   ( IStr )                     ;
  Len := LENGTH(IStr)                   ;

  IF (Len>50)        THEN E := 4                    ;
  IF (Len=0 )        THEN E := 3 + 3*BYTE(Source=3) ;
  IF (IStr[1] IN Qt) THEN E := 3                    ;
  {-------------------------------------------------}
  { Parse Str into numbers                          }
  {-------------------------------------------------}
  i := 1 ;
  k := 1 ;
  IF (E=0) THEN
  BEGIN
    REPEAT
      j    := 1      ;
      NStr := '    ' ;
      IF (IStr[i] IN Qt) THEN E := 5                ;
      IF NOT (IStr[i] IN HNum+Sep+Qt) THEN E := 1   ;

      WHILE (IStr[i] IN HNum) AND (i<=Len) AND (E=0) DO
             BEGIN
                  IF (IStr[i] IN ['.','H','h']) THEN IStr[i] := '$' ;
                  NStr[j] := IStr[i]                                ;
                  INC (i)                                           ;
                  IF j<5 THEN INC (j)                               ;
                  NStr[0] := CHR(j-1)                               ;
             END                                                    ;

      IF (j>1) AND (E=0) THEN  { Number found        }
      BEGIN
        NArr[k] := NStr                    ;
        IF k<12 THEN INC (k)               ;
      END                                  ;

      WHILE (IStr[i] IN Sep)  AND (i<=Len) DO INC(i)   ;

    UNTIL (E>0) OR (i>Len)               ;
    SearchN[0] := k-1                    ; { # Numbers in NArr }
  END                                    ; { E=0 }

  i := 1                                 ;
  IF (E=0) OR (E=5) THEN
  REPEAT
    VAL     ( NArr[i], SearchN[i], F )              ;
    IF  NOT ( SearchN[i] IN [0..255] ) THEN E := 2  ;
    INC (i)                                         ;
  UNTIL (F>0) OR (E=2) OR (i>SearchN[0])            ;

  IF ((E=0) OR (E=5)) AND (F=0) THEN
  BEGIN
    TEXTCOLOR (Red)                                      ;
    FOR i := 1 TO SearchN[0] DO WRITE (' ',SearchN[i] )  ;
    TEXTCOLOR (Blue)                                     ;
    WRITELN                                              ;
  END                                                    ;

  IF (E>0) OR (F>0) THEN
      BEGIN
        IF (F>0) OR (E=1) THEN WRITE (E1)            ;
        IF          (E=2) THEN WRITE (E2)            ;
        IF (E=3) OR (E=5) THEN WRITE (E3)            ;
        IF          (E=4) THEN WRITE (E4)            ;
        DELAY (1000)                                 ;
      END                                            ;
  UNTIL (E IN [0,3,5,6]) AND (F=0)                   ;

  IF (E>0) THEN SearchN[0] := 0                      ;
  IF (E=6) THEN SearchN[0] := 255                    ;

  IF (Source=2) THEN
  BEGIN
    TempN    := SearchN             ;
    Len      := GetNumberStr (3)    ;
    ReplaceN := SearchN             ;
    SearchN  := TempN               ;
  END                               ;
  GetNumberStr := SearchN[0]        ;
END                                 ;
{---------------------------------------}
{$I PATCH\MIDIPRGE.INC } { FILE SAVE-LOAD }
{$I PATCH\MIDIPRGD.INC } { PARAMSTR CHK }
{$I PATCH\MIDIPRG7.INC } { EDITOR       }
{---------------------------------------}
PROCEDURE DoMemory                      ;
{---------------------------------------}
BEGIN
   WRITE ('MEMORY:[',MEMAVAIL,']bytes.')              ;
   Memory := ((MAXAVAIL-BufferMax-256) DIV 256)*256   ;
   IF (Memory>130560) THEN Memory := 130559           ;
   NBuff.Max  := BufferMax                            ;
   OBuff.Max  := ((Memory DIV 2) DIV 256)*256         ;
   SBuff.Max  := OBuff.Max                            ;
   WRITE ('BUFFERS:NORM[',BufferMax,']')              ;
   WRITE ('OVFLOW[',OBuff.Max,']SEL[',SBuff.Max,']')  ;
END                                                   ;
{-----------------------------------------}
{ Main                                    }
{-----------------------------------------}
BEGIN
   CLRSCR                          ;
   DoSubWindow (17)                ;
   DoMemory                        ;

   GETMEM ( XBuff.P, 10)           ;
   GETMEM ( NBuff.P, NBuff.Max+10) ;
   GETMEM ( OBuff.P, OBuff.Max+10) ;
   GETMEM ( SBuff.P, SBuff.Max+10) ;

   BuffN := TRUE                  ;
   BuffO := TRUE                  ;
   BuffS := FALSE                 ;

   ClearBuffer (NBuff)            ;
   ClearBuffer (OBuff)            ;
   ClearBuffer (SBuff)            ;

   IF SetXBuff THEN XBuff:=NBuff  ;
   CurrentBuff := 1               ;
END .
