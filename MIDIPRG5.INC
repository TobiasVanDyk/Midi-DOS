{-----------------------}
{ Configure Filter List }
{-----------------------}
PROCEDURE DoFilter      ;

CONST
S1 = ' [FO] Sys Excl Start [FB] Timing Cont.   [8x] Note Off'         +NL+
     ' [F2] Song Position  [FC] Timing Stop    [9x] Note On'          +NL+
     ' [F3] Song Select    [FE] Active Sense   [Ax] Key Aftertouch'   +NL;
S4 = ' [F6] Tune Request   [FF] System Reset   [Bx] Control Changes'  +NL+
     ' [F7] Sys Excl End  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  [Cx] Patch Change'     +NL;
S6 = ' [F8] Timing Clock  ³INPUT CHANNEL:   ³  [Dx] Channel Pressure' +NL+
     ' [FA] Timing Start  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  [Ex] Pitch Bend'       +NL;

M1 = '             [ MIDI COMMAND CODES FILTERLIST ]' +NL+NL           ;
M2 = ' (Codes [FO]..[FF] System-wide : Select on Channel 1 only. )' +NL+
     ' (Codes [FO],[F2],[F3],[8O] - [EF] : Data bytes included.  )'    ;


PosArr : ARRAY [0..17,0..2] OF BYTE = (
( 3,4,$F0),( 3,5,$F2),( 3,6,$F3),( 3,7,$F6),( 3,8,$F7),( 3,9,$F8),( 3,10,$FA),
(23,4,$FB),(23,5,$FC),(23,6,$FE),(23,7,$FF),
(43,4,$80),(43,5,$90),(43,6,$A0),(43,7,$B0),(43,8,$C0),(43,9,$D0),(43,10,$E0));

VAR
    xy, n  : INTEGER     ;
    I      : WORD        ;
    A1, A2 : CHAR        ;
    MCh    : INTEGER     ;
    HStr   : Str10       ;
{------------------------}
PROCEDURE UpdatePanel    ;
{------------------------}
VAR n : INTEGER          ;

BEGIN
  FOR n := 0 TO 17 DO
  BEGIN
    GOTOXY ( PosArr[n,0], PosArr[n,1] ) ;
    IF C[MCh].FList[n]>0 THEN TEXTCOLOR (White)     ;
    IF C[MCh].FList[n]=0 THEN IF n<>4 THEN TEXTCOLOR (Black)
                                      ELSE TEXTCOLOR (Blue) ;  { Sys Ex End }
    I := PosArr[n,2]             ;
    HStr := DecToHex (I)         ;
    WRITE (HStr,#08,#08)         ;
  END                            ;
END ;
{------------------------------}
BEGIN
  WriteBottom (7)           ;
  DoSubWindow (14)          ;
  WRITE (NL,M1,S1,S4,S6)    ;
  TEXTCOLOR (Black)         ;
  WRITE (NL,M2)             ;

  MCh := 0                     ;
  xy  := 0                     ;
  UpdatePanel                  ; { For Midichannel 0 }

  REPEAT
    GOTOXY (36,9) ; TEXTCOLOR (LightRed) ; WRITE (MCh+1:2) ;
    UpdatePanel                                            ;
    GOTOXY ( PosArr[xy,0], PosArr[xy,1] )                  ;

    A1 := UPCASE (READKEY) ;

    CASE A1 OF #00: BEGIN
                     A2 := READKEY   ;     { 2 code for special keys }
                     CASE A2 OF #72 : { Up Arrow    } xy := xy  - 1  ;
                                #80 : { Down Arrow  } xy := xy  + 1  ;
                                #75 : { Left Arrow  } MCh:= MCh - 1  ;
                                #77 : { Right Arrow } MCh:= MCh + 1  ;
                      END           ; { Case A2                      }
                      IF  xy <  0 THEN xy  := 17 ;
                      IF  xy > 17 THEN xy  := 0  ;
                      IF MCh < 0  THEN MCh := 15 ;
                      IF MCh > 15 THEN MCh := 0  ;
                   END   ; { Case A1 = #0        }

              #13: BEGIN   { Return : Select Filter Code              }
                     IF xy<>4 THEN C[MCh].FList[xy] :=
                              C[MCh].FList[xy] XOR (PosArr[xy,2]+MCh) ;
                     IF (MCh>0) AND (xy<8) THEN C[MCh].FList[xy] := 0 ;
                     IF (xy=0) AND (MCh=0) THEN C[MCh].FList[4] :=
                     C[MCh].FList[4] XOR (PosArr[4,2])                ;
                   END                      ;
              'I': BEGIN   { Invert current Midichannel }
                     FOR n := 0 TO 17 DO C[MCh].FList[n] :=
                                         C[MCh].FList[n] XOR PosArr[n,2] ;
                   END                      ;
              'C': BEGIN   { Clear current Midichannel }
                     FOR n := 0 TO 17 DO C[MCh].FList[n] := 0 ;
                   END                      ;
              'A': BEGIN   { Clear all Midichannels 1-16 }
                     ClearFilterLists                    ;
                   END                      ;
              END                           ; { CASE A1 }
  UNTIL (A1='Q') OR (A1=#27)                ;
  FEFilter := C[0].FList[9]                 ;
  F8Filter := C[0].FList[5]                 ;
END                                         ;
