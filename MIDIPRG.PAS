
{$M 6000, 100, 300000 }  { StackSize : MOVE use > 2k stack              }
                         { FreeMin   : Leave space for dispose heaplist }
{$S-,R-,O-}              { Stack Check Off: will affect CALL WriteCode3 }
{--------------------------------------}
PROGRAM midiprogrammer                 ;
{--------------------------------------}
USES DOS, CRT,
     MIDIPRG1,  { GLOBAL VARIABLES and various FUNCTIONS, PROCEDURES }
     MIDIPRG2 ; { INT Handler, INT Routines, HELP Routines           }

     PROCEDURE SwopBuffers ; FORWARD ;

{-----------------------------------}
{ INCLUDE FILES :                   }
{ PATCH\Midiprg3.INC  Matrix Setup  }
{ PATCH\Midiprg4.INC  CheckFilters  }
{ PATCH\Midiprg5.INC  Filter Setup  }
{ PATCH\Midiprg6.INC  Options menu  }
{ PATCH\Midiprg7.INC  Editor P.Key  }  { In File MIDIPRG1.PAS }
{ PATCH\Midiprg8.INC  Main Menu     }
{ PATCH\Midiprg9.INC  WriteCode1,2  }
{ PATCH\MidiprgA.INC  Normal Nosplit}
{ PATCH\MidiprgB.INC  Do C0, D0     }
{ PATCH\MidiprgC.INC  KeySplit      }
{-----------------------------------}
{-----------------------------------------}
PROCEDURE DoSplit                         ;
{$I Patch\midiprgC.INC }    { INLINE PROC }
{-----------------------------------------}
PROCEDURE DoNoSplit                       ;
{$I Patch\midiprgA.INC }    { INLINE PROC }
{-----------------------------------------}
PROCEDURE DoC0D0                          ;
{$I Patch\midiprgB.INC }    { INLINE PROC }
{-----------------------------------------}
{ Convert Number to ASCII string          } { 25 bytes long }
{-----------------------------------------}
PROCEDURE NumToStr1                       ; INLINE
          ( $8B/$3E/WriteByte    { MOV DI, WriteByte    } { WDummy = 0 }
           /$D1/$E7              { SHL DI  2* = WORD    } { *2 }
           /$03/$3E/WriteByte    { ADD DI, WriteByte    } { *3 }
           /$81/$C7/NumArr       { ADD DI, NN=@NumArr   }
           /$8B/$05              { MOV AX, [DI]         } {NumArr+3*WriteByte}
           /$A3/NumStr+2         { MOV NumStr[2], AX    }
           /$8B/$45/$02          { MOV AX, [DI+$02]     } {NumArr+3*WriteByte}
           /$A2/NumStr+4 ) ;     { MOV NumStr[4], AL    }
{-------------------------------------------}
PROCEDURE DoWritePort ( CONST B:BYTE )      ;
{-------------------------------------------}
PROCEDURE WritePort (P:PortType )           ;
BEGIN
  REPEAT
    ReadByte := PORT[P.SERLineStat] AND $20 ; { Transmitter empty Bit 5 }
  UNTIL ( ReadByte > 0 )                    ;
  PORT [P.SERData] := B                     ;
END                                         ;
BEGIN
  IF PortOutA THEN WritePort (P1) ;
  IF PortOutB THEN WritePort (P2) ;
  IF PortOutC THEN WritePort (P3) ;
  IF PortOutD THEN WritePort (P4) ;
END                               ;
{-----------------------------------------}
PROCEDURE WriteBottom ( Option : BYTE )   ;
{-----------------------------------------}
CONST
ArUD = #24#25 ;
S1=' [Home][End]Start-End [PUp-PDwn]Prev-Next [E]DmpEnd [N][O][S]Buffer [Q]uit';
B1=' ACTIVATE-RUN: [Q][ENTER]Quit [ESC]Stat '+NL+
   ' [A-Z]FileKey [F1-F12]BuffKey ['+ArUD+']Channel [0-127][ENTER]Patch' ;
B3=' STATUS: CHAN[  ] BUFF[   ] RSTAT[   ] FILT[   ] CODE[   ] INIT[   ] SYSX[   ]' ;
T1=' Polling Read: [ANY-KEY] Quit Read. '                        ;
U1= 'Enter code(s) to transmit. Enter Q to return to main menu.' ;
V=' ['+ArUD+']Move [Enter]Select ['+ArL+ArR+']Channel [C]lear Ch. [A]llClear [I]nvert [Q]uit';
VAR Y1, Y2, Color : INTEGER ;
BEGIN
   Y2 := 25            ;
   Color := LightGray  ;
   CASE Option OF
   6  : BEGIN Color:=Green; Y1:=23; Y2:=23 ; END ;
   2  : Y1 := 24                                 ;
   ELSE Y1 := 25                                 ; {0,1,3..5,7}
   END                                           ;
   DoWindow (Color ,Blue,1,Y1,80,Y2,1,0)           ;
   CASE Option OF
   0: WRITE (S1)                ;
   2: WRITE (B1)                ;
   4: WRITE (T1)                ;
   5: WRITE (U1)                ;
   6: WRITE (B3)                ;
   7: WRITE (V)                 ;
   END                          ;
END                             ;
{-----------------------------------------------}
FUNCTION GetRanges ( CONST Source:BYTE) : WORD  ;
{-----------------------------------------------}
CONST
  S0=NL+' Dump Start [O-'           ;
  S1=NL+' Buffer Position [O-'      ;
  S3=   '][ENTER=0][E=End] : '      ;
VAR
  Start : WORD    ;
  E     : INTEGER ;
  AStr  : Str10   ;

BEGIN
   REPEAT
       E := 0                                                        ;
       IF (Source=0) THEN WRITE (S0) ELSE WRITE (S1)                 ;
       WRITE (XBuff.Max, S3)                                         ;
       READLN (AStr)                                                 ;
       IF AStr = '' THEN AStr := '0'                                 ;
       IF UPCASE(AStr[1])='E' THEN
       BEGIN
         IF (Source=0) THEN Start := (XBuff.Pos DIV 256)*256
                       ELSE Start := XBuff.Pos+1                     ;
       END ELSE VAL (AStr, Start, E )                                ;
       IF (Start>XBuff.Max) THEN E := 1                              ;
   UNTIL (E=0)                                                       ;
   GetRanges := Start                                                ;
END                                                                  ;
{--------------------------------------------------------------------}
{$I PATCH\MidiprgG.INC }  { Buffer display, Code Sender + Receiver   }
{--------------------------------------------------------------------}
{---------------------------------------------}
{$I PATCH\Midiprg5.INC }  { Filter Setup      }
{---------------------------------------------}
{------------------------------------------------}
{$I PATCH\Midiprg4.INC} { CheckFilters           }
{$I PATCH\Midiprg9.INC} { WriteCode1, WriteCode2 }
{------------------------------------------------}
{-----------------------------------------}
{ Read and Write                          }
{-----------------------------------------}
PROCEDURE DoReadWrite ( Option : CHAR )   ;
{-----------------------}
TYPE
  MidiErrorType = RECORD
                    Pos     : LONGINT ;
                    MidiChr : BYTE    ;
                  END                 ;

VAR MError     : ARRAY [0..5] OF MidiErrorType ;
    ErrorCount : INTEGER                       ;
    NStr       : Str16                         ;

CONST
    B1 = NL+' Warning:[CodeDisplay=ON]:Slower response to received code(s)';
    B2 = NL+' Warning:[Buffer=ON]:F1-F12 PROG.KEYS could be overwritten.'  ;
    B4 = NL+' Sending Initialising Code(s): [  ].' + #08#08#08#08          ;

VAR MidiChannel, B, Y   : BYTE        ;
    j                   : INTEGER     ;
{-------------------------------------}
PROCEDURE DoEcho                      ;
{-------------------------------------}
BEGIN
  ;
END ;
{-------------------------------------}
PROCEDURE DoRemote (i:INTEGER)        ;
{ First press MasterKey Then SubKey   }
{-------------------------------------}
LABEL          L1,L2,L3               ;
VAR Size     : LONGINT                ;
    S1,S2,S3 : Str24                  ;

PROCEDURE WriteLoad ( SF:Str24 )                  ;
CONST S = NL+' Loaded: '                          ;
BEGIN
  IF (Size>0) THEN WRITE (S,SF,' Size:[',Size,']');
END                                               ;
BEGIN
  DisableINT                            ;
  S1 := Remote[i].CFG                   ;
  S2 := Remote[i].PROG                  ;
  S3 := Remote[i].DTA                   ;

  IF (S1=Sp) THEN GOTO L1               ;
      CFGLoad ( S1, Size )              ;
      WriteLoad (S1)                    ;
  L1:
  IF (S2=Sp) THEN GOTO L2               ;
      KEYLoad ( 3, S2, Size )           ;
      WriteLoad (S2)                    ;
  L2:
  IF (S3=Sp) THEN GOTO L3               ;
      DTALoad ( S3, Size, 0 )           ;
      WriteLoad (S3)                    ;
  L3:
  EnableINT                             ;
END                                     ;
{-------------------------------------------------------------------}
 PROCEDURE UpDateStat (Option:BYTE)                ;
{--------------------------------------------------}
    BEGIN
      DoWindow (Green, Black, 1,23,80,23,0,0)        ;
      CASE Option OF
      0: BEGIN
           GOTOXY (15,1) ; WRITE (MidiChannel+1:2)        ;
           GOTOXY (24,1) ; WRITE (OnOff[Q.BuffOption])    ;
           GOTOXY (35,1) ; WRITE (OnOff[Q.RStatEnable])   ;
           GOTOXY (45,1) ; WRITE (OnOff[Q.FilterEnable])  ;
           GOTOXY (55,1) ; WRITE (OnOff[Q.CodeOption])    ;
           GOTOXY (65,1) ; WRITE (OnOff[Q.InitStrEnable]) ;
           GOTOXY (75,1) ; WRITE (OnOff[Q.SysExOption])   ;
         END                                            ;
      1: BEGIN
           GOTOXY (15,1) ; WRITE (MidiChannel+1:2)      ;
         END                                            ;
      END                                               ;
      DoWindow (LightGray, Blue, 11,5,74,19,0,0)        ;
    END                                                 ;
{----------------------------------------}
PROCEDURE MidiError                      ;
{----------------------------------------}
LABEL L1                                 ;
BEGIN
  IF (ErrorCount>5) THEN GOTO L1         ;
  MError[ErrorCount].Pos    :=WriteCount ;
  MError[ErrorCount].MidiChr:=WriteByte  ;
  L1:
  INC (ErrorCount)                       ;
  IF (Q.CodeOption=0) THEN WRITE (' *E') ;
END                                      ;
{----------------------------}
{ Check Key Pressed          }
{----------------------------}
PROCEDURE CheckKey           ;

VAR K2   : CHAR              ;
{----------------------------------------------}
PROCEDURE DoPatchKeys ( K1: CHAR)              ;
{----------------------------------------------}
LABEL L1                     ;
VAR E          : INTEGER     ;
    S          : Str10       ;
BEGIN
  DisableINT                 ;
  S := ' PRG: '              ;
  S[6] := K1                 ;
  WRITE (S)                  ;
  READLN (S)                 ;
  EnableINT                  ;

  S := K1 + S                ;
  VAL ( S, NewB[2], E )      ;

  IF (E>0)         THEN GOTO L1  ;
  IF (NewB[2]>127) THEN GOTO L1  ;

  NewB[1] := $C0 OR MidiChannel  ;
  n       := 3                   ;
  INLINE ( $FF/$16/Write3Addr)   ; { CALL WriteCode3   }
  L1:
END                              ;
{--------------------------------------------}
PROCEDURE DoFKeys (VAR Start, Stop: ArrW12 ) ;
{--------------------------------------------}
LABEL L1                                ;
VAR Size, i : WORD                      ;
    S       : Str10                     ;
BEGIN
  S := NL+' F:'                               ;
  Size := (Stop[KBrd.FKey]-Start[KBrd.FKey])  ; { Number of bytes to send }
  IF (Size=0) THEN GOTO L1                    ;

  S := S + CHAR(KBrd.FKey+48)                 ;
  WRITE (S)                                   ;
  FOR i := 0 TO Size-1 DO
  BEGIN
    WriteByte := XBuff.P^[Start[KBrd.FKey]+i] ;
    WriteCode2                                ;
  END                                         ;
  L1:
END                                           ;
{----------------------------------------}
PROCEDURE DoFileKeys (Z:INTEGER)         ;
{----------------------------------------}
LABEL L1                                 ;
VAR NStr, S1   : Str24                   ;
    F          : FILE                    ;
    FError     : INTEGER                 ;
    i, j, r, s : INTEGER                 ;
    Size       : LONGINT                 ;
    WBuff      : ARRAY [0..255] OF BYTE  ;

BEGIN
  S1 := NL+' FILE:'             ;
  NStr := FLink[Z]              ;
  IF NStr = '' THEN GOTO L1     ;

  S1   := S1 + NStr             ;
  Size := FSizeArr[Z]           ;
  r    := Size DIV 256          ;
  s    := Size MOD 256          ;

  ASSIGN (F, NStr)              ;
{$I-}
  RESET (F, 1 )                 ;
  FError := IORESULT            ;
{$I+}
  IF (FError>0) THEN GOTO L1    ;

  IF r>0 THEN FOR i := 1 TO r DO
  BEGIN
    BLOCKREAD (F, WBuff, 256 )               ;
    FOR j := 0 TO 255 DO
    BEGIN
      WriteByte := WBuff[j]                  ;
      WriteCode2                             ;
    END                                      ;
  END                                        ;
  IF s>0 THEN WHILE NOT (EOF(F)) DO
   BEGIN
     BLOCKREAD (F, WriteByte, 1)              ;
     WriteCode2                               ;
   END                                        ;
   WRITE (S1,' [', Size,']', NL)              ;
   CLOSE (F)                                  ;
  L1:
END                                           ;
{-------------------------------------------}
(* KBrd:RECORD  0: Ascii : BYTE    ;
                   Scan  : BYTE    ;
                   Class : BYTE    ;
                   FKey  : BYTE    ;
                   UpChr : CHAR    ;
                1: Val   : WORD    ;
                2: Chr   : CHAR    ;
                3: On    : BOOLEAN ; *)
BEGIN
  CASE KBrd.Class OF
  0 : CASE KBrd.UpCh OF
      #13      :                                ;
      #27      :                                ;
      #65..#70 : DoFileKeys (ORD(KBrd.UpCh)-64) ; { A..Z }
      #48..#57 : DoPatchKeys (KBrd.Ch)          ; { 0..9 }
      END                                       ;
  10: DoFKeys ( FKey.N.Start,FKey.N.Stop );
  11: DoFKeys ( FKey.S.Start,FKey.S.Stop );
  12: DoFKeys ( FKey.C.Start,FKey.C.Stop );
  13: DoFKeys ( FKey.A.Start,FKey.A.Stop );
   1: BEGIN
        CASE KBrd.Scan OF
        72: INC (MidiChannel)      ;
        80: DEC (MidiChannel)      ;
        ELSE EXIT                  ;
        END                        ;
        IF MidiChannel=16 THEN MidiChannel:=0  ;
        IF MidiChannel>16 THEN MidiChannel:=15 ;
        WRITE (NL,' MC:',MidiChannel+1)        ;
        UpDateStat (1)                         ;
        GOTOXY (1,Y+2)                         ;
      END                                      ;
  END                                          ;
END                                            ;
{------------ MAIN --------------------------}
BEGIN
 OverFlow    := 0   ;
 KBrd.UpCh   := ' ' ;
 ErrorCount  := 0   ;
 SysBegin    := 0   ;
 SysEnd      := 0   ;
 SysSize     := 0   ;
 SysExBusy   := 0   ;
 WriteCount  := 0   ;
 ReadCount   := 0   ;
 MidiChannel := 0   ;
 RemoteFlag  := 0   ;

 CBuff[0]  := 0                       ;
 FOR i:= 0 TO CBuffMax DO CBuff[i]:=0 ;
 CBuffPut  := 0                       ; { WORD Size but use as BYTE! }
 CBuffGet  := 0                       ;

 Filter     := 0                                       ;
 MCh        := 0                                       ;
 MChDummy   := 0                                       ; { MCh WORD=0 }
 Count      := 0                                       ;
 NewB[0]    := 0                                       ;

 ComSize   := 0 ;
 ComCode   := 0 ;
 ComCount  := 0 ;
 ComSysEx  := 0 ;
 ComClass  := 0 ;
 ComStatus := 0 ;

 FEFilter := C[0].FList[9]              ;
 F8Filter := C[0].FList[5]              ;

 DoSubWindow (17)                        ;
 WriteBottom (2)                         ;
 WriteBottom (6)                         ;
 UpDateStat  (0)                         ;

 IF NOT SetXBuff   THEN Q.BuffOption:=0  ; {Set all XBuff Parameters }
 { Buffer Position }
 IF (Q.AutoClr=1) THEN ClearBuffer (XBuff)                         ;
 IF Option='B' THEN XBuff.Pos := GetRanges (1) ELSE XBuff.Pos := 0 ;
 IF Q.CodeOption=1 THEN WRITE (B1)                                 ;
 IF Q.BuffOption=1 THEN WRITE (B2)                                 ;

 StartBuff   := CurrentBuff              ;
 {-----------------------------------------------------}
 { Send any initialising strings first                 }
 {-----------------------------------------------------}
 IF (Q.InitStrEnable=1) THEN
 BEGIN
   B := 0                             ;
   FOR i := 0 TO 15 DO
   BEGIN
     IF C[i].MidiStrX > 0 THEN
     BEGIN
       INC (B)                        ;
       FOR n := 0 TO C[i].MidiStrX - 1 DO
       BEGIN
         WriteByte := C[i].MidiStr[n] ;
         WriteCode2                   ;
       END                            ; {n}
     END                              ; {MidiStrX}
  END                                 ; {i}
  WRITE ( B4, B )                     ;
END                                   ; { InitialStrEnable }
{-----------------------------------}
WRITE (NL)                          ;
TEXTCOLOR (Black)                   ;
TimeOn        := FALSE              ;
SERError      := 0                  ;
SERErrorCount := 0                  ;
SERReadByte   := 0                  ;
SERErrorPos   := 0                  ;
EnableINT                           ;

 REPEAT
   IF (CBuffGetLo=CBuffPutLo) THEN        { Nothing to write .... }
   BEGIN
    {--------------------------------------------}
    IF PressedKey THEN BEGIN
                         GetKey                    ;
                         Y := WHEREY               ;
                         TEXTCOLOR (Red)           ;
                         CheckKey                  ;
                         TEXTCOLOR (Black)         ;
                       END                         ;
   {---------------------------------------------}
   END
   ELSE BEGIN  { CBuffGet<>CBuffPut } { Something to write....}
   {-------------------------------------------------------------------}
   INLINE (  $BB/CBuff                   { MOV BX, NN=@CBuff           }
            (* Hi byte of CBuffGet always = 0 : DI= word move !! ******)
            /$8B/$3E/CBuffGet            { MOV DI, CBuffGet            }

            /$8A/$01                     { MOV AL, [BX+DI]             }
            /$A2/WriteByte               { MOV WriteByte, AL           }
            /$FE/$06/CBuffGet            { INCB CBuffGet               }

            /$3C/$F0                     { CMP AL, $F0  SysEX Start    }
            /$75/$05                     { JNE L1                      }
            /$C6/$06/SysExBusy/$01       { MOVB SysExBusy, $01         }
     {L1:}  /$3C/$F7                     { CMP AL, $F7  SysEX End      }
            /$75/$05                     { JNE Exit                    }
            /$C6/$06/SysExBusy/$00 ) ;   { MOVB SysExBusy, $00         }
     {-----------------------------------------------------------------}
     {---------------------------}
     { Check if filter is active }
     {---------------------------}
     CheckFilters                ; { INLINE PROC:Execute if FilterEnable=1 }
     {-----------------------------}
     { Set Flag if Midi Cmd        }
     {-----------------------------}
     INLINE ( $A0/WriteByte          { MOV AL, WriteByte     }
             /$3C/$80                { CMP AL, $80           }
             /$72/$47                { JB Exit               }
             /$3C/$EF                { CMP AL, $EF           }
             /$77/$43                { JA Exit               }
             /$50                    { PUSH AX               }
             /$24/$F0                { AND AL, $F0           }
             /$A2/CmdB               { MOV CmdB, AL          }
             /$58                    { POP AX                }
             /$24/$0F                { AND AL, $0F           }
             /$A2/MCh                { MOV MCh, AL           }

             /$B4/$00                { MOV AH, $00           }
             /$D1/$E0                { SHL AX                }{*2WORDlength }
             /$BB/CAddr              { MOV BX, NN=@CAddr     }
             /$01/$C3                { ADD BX, AX            }
             /$8B/$3F                { MOV DI, [BX]          }
             /$89/$3E/CurCAddr       { MOV CurCAddr, DI      }

             /$80/$3D/$00            { CMP [DI], $00         }{ DI=C[MCh].ChX }
             /$75/$1E                { JNE L1:               }

             /$81/$C7/$8C/$00        { ADD DI, $8C = 140     }{ DI = ChMessX }
             /$80/$3D/$00            { CMP [DI+(140)], $00   }
             /$75/$15                { JNE L1:               }

             /$B1/$04                { MOV CL, $04            }
             /$A0/CmdB               { MOV AL, CmdB           }
             /$D2/$E8                { SHR AL, #[CL]          }
             /$BB/X80E0Filter        { MOV BX, NN=@X80E0Filter}
             /$D7                    { XLAT                   }
             /$A2/Filter             { MOV Filter, AL         }
             /$C6/$06/Count/$00      { MOVB Count, $00        }
             /$EB/$05                { JMP Exit               }

       {L1:} /$C6/$06/Count/$01  ) ; { MOVB Count, $01        }

     IF Filter=0 THEN              { Not a filter byte active }
     BEGIN
       (******************************************************)
       CASE Count OF
       {---------------------------------------------}
       2: BEGIN
            { IF (WriteByte<$80) THEN Data1:=WriteByte             ; }
            INLINE  ( $A0/WriteByte          { MOV AL, WriteByte     }
                     /$3C/$7F                { CMP AL, $7F           }
                     /$77/$03                { JA Exit               }
                     /$A2/Data1  ) ;         { MOV Data1, AL         }
            DoC0D0                           ;
          END                                ; { CASE Count=2 }
       {-------------------------------------}
       3: BEGIN
       {---------------------------------------------------------------------}
       INLINE  ( $A0/WriteByte          { MOV AL, WriteByte     }
                /$3C/$7F                { CMP AL, $7F           }
                /$77/$03                { JA Exit               }
                /$A2/Data2  ) ;         { MOV Data2, AL         }
                { IF (WriteByte<$80) THEN Data2:=WriteByte ;    }
          {----------------------------------------------------------------}
          CASE CmdB OF
          {----------------------------------------------------------------}
          $90,$80:
          BEGIN
            {---------------------------------------------}
            { Check Remote Key                            }
            {---------------------------------------------}
            IF (RemoteFlag=1) THEN
            BEGIN
              IF (Data2>0) THEN FOR i:=1 TO 12 DO
                  IF Data1=Remote[i].KEY THEN DoRemote(i)       ;
              INC (RemoteFlag)                                  ;
            END                                                 ;
            {----------------------------------------------------}
            IF (RemoteFlag=0) THEN
            BEGIN
          INLINE ( $8B/$3E/CurCAddr     { MOV DI, CurCAddr C[MCh]}
                  /$8B/$05              { MOV AX, [DI]           }
                  /$A3/Cx               { MOV Cx, AX             }
                  /$8B/$45/$02          { MOV AX, [DI+$02]       }
                  /$A3/Px               { MOV Px, AX             }
                  /$8B/$45/$04          { MOV AX, [DI+$04]       }
                  /$A3/Vx               { MOV Vx, AX             }
                  /$C7/$06/n/$01/$00 ); { MOVW n, $0001          }
                  (**********************************************
                   n := 1              ;
                   Cx := C[MCh].ChX    ;
                   Px := C[MCh].PitchX ;
                   Vx := C[MCh].VelX   ;
                  ***********************************************)
                {------------------------------------------------}
                { Split Keyboard or Normal                       }
                {------------------------------------------------}
                IF (C[MCh].SplitX>0) THEN DoSplit ELSE DoNoSplit ;
                IF (Q.TimeEnable=1) THEN
                BEGIN
                  IF TimeOn THEN BEGIN
                                   TimeOn   := FALSE                   ;
                                   EventCnt := WORD(TimerCnt-Time)     ;
                                   XBuff.P^[XBuff.Pos] := $FF          ;
                                   INC (XBuff.Pos)                     ;
                                   XBuff.P^[XBuff.Pos] := LO(EventCnt) ;
                                   INC (XBuff.Pos)                     ;
                                   XBuff.P^[XBuff.Pos] := HI(EventCnt) ;
                                   INC (XBuff.Pos)                     ;
                                   TimeOn := TRUE                      ;
                                   Time   := TimerCnt                  ;
                                 END
                            ELSE BEGIN
                                   TimeOn := TRUE                      ;
                                   Time   := TimerCnt                  ;
                                 END                                   ;
                END                                                    ;
                {-------------------------------------------------}
                INLINE ( $82/$3E/n/$01         { CMPB n, $01      }
                        /$74/$04               { JE L7:           }
                        /$FF/$16/Write3Addr    { CALL WriteCode3  }
                {L7:}   /$C6/$06/Count/$00  ); { MOVB Count, $00  }
                {-------------------------------------------------}
                END                            { RemoteFlag = 0   }
                ELSE BEGIN                     { RemoteFlag = 1,2 }
                       IF RemoteFlag=2 THEN RemoteFlag := 0       ;
                       Count := 0            ;
                     END                     ;
          END                                ; { CASE CmdB 80,90 }
          {------------------------------------------------------}
          ELSE  { CmdByte>80,90 }
          BEGIN
                n := 1            ;
                FOR i := 0 TO 15 DO
                BEGIN
                  B := 0          ;
                  IF (CmdB=$A0) AND (C[MCh].ChMess.A0[i]=1) THEN B := 1 ;
                  IF (CmdB=$B0) AND (C[MCh].ChMess.B0[i]=1) THEN B := 2 ;
                  IF (CmdB=$E0) AND (C[MCh].ChMess.E0[i]=1) THEN B := 3 ;

                  IF (B=2) THEN
                  BEGIN
                    IF (C[MCh].ChMessB0.COut1=i) THEN
                    BEGIN
                       B := 0                                                    ;
                       FOR j := 0 TO 9 DO
                       IF C[MCh].ChMessB0.ContrList1[j] = Data1 THEN B := 4   ;
                    END                                                       ;
                    IF (C[MCh].ChMessB0.COut2=i) THEN
                    BEGIN
                      IF B<>4 THEN B := 0                                     ;
                      FOR j := 0 TO 9 DO
                      IF C[MCh].ChMessB0.ContrList2[j] = Data1 THEN
                         IF B = 4 THEN B := 6 ELSE B := 5                     ;
                    END                                                       ;
                  END            { B=2 }                                      ;
                  IF B>0 THEN
                  BEGIN
                    IF  B<4 THEN NewB[n]        :=CmdB + i                    ;
                    IF (B=4)OR(B=6) THEN NewB[n]:=CmdB + C[MCh].ChMessB0.COut1;
                    IF (B=5)OR(B=6) THEN NewB[n]:=CmdB + C[MCh].ChMessB0.COut2;
                    NewB[n+1] := Data1       ;
                    NewB[n+2] := Data2       ;
                    n := n+3                 ;
                  END                        ;
                END                          ; { i Loop            }
                IF n>1 THEN
                BEGIN
                  INLINE ( $FF/$16/Write3Addr) ; { CALL WriteCode3   }
                END                            ;
                Count := 0                   ;
          END              { CmdB > 90 }     ;
          {----------------------------------}
          END              { CASE CmdB }     ;
       {-------------------------------------}
       END              { CASE Count=3 }     ;
       {----------------------------------------------------}
       0: BEGIN
            IF (Q.CodeOption=1) THEN
            BEGIN
              NumToStr1        ;
              WRITE (NumStr)   ;
            END                ;
            IF (SysExBusy=0)AND(WriteByte<$80) THEN MidiError
                                               ELSE WriteCode2          ;
          END                                               ; { CASE Count=0 }
       {-----------------------------------------------------}
       END  { CASE COUNT }                                   ;
       {-----------------------------------------------------}
       { IF (Count>0)   THEN INC (Count)                     }
       INLINE ( $80/$3E/Count/$00      { CMPB Count, $00     }{ Count = 0 }
               /$74/$04                { JE Exit             }
               /$FE/$06/Count  );      { INCB Count          }
     {-------------------------------------------------------}
     END                                                     ; { Filter = 0 }
    {--------------------------------------------------------}
    { IF (Filter IN [1..254]) THEN DEC (Filter )             }
    INLINE  ( $80/$3E/Filter/$00  { CMPB Filter, $00         }{ Filter = 0   }
             /$74/$0B             { JE Exit                  }
             /$80/$3E/Filter/$FF  { CMPB Filter, $255        }{ Filter = 255 }
             /$74/$04             { JE Exit                  }
             /$FE/$0E/Filter  );  { DECB Filter              }
    {--------------------------------------------------------}
    END                                          ; { CbuffPut<>CBuffGet}
 {-----------------------------------------------}
 UNTIL (KBrd.UpCh='Q')                           ;
 {-----------------------------------------------}
 DisableINT                                 ;

 IF (Q.BuffOption=1) THEN ResetBuff (1)     ;

 IF (Q.SysExOption=1) THEN
 BEGIN
   CLRSCR                                   ;
   SysSize := SysEnd-SysBegin               ;
   WRITE (NL,' SysExStart:', SysBegin-1,' ',
             ' SysExEnd  :', SysEnd-1  ,' ',
             ' SysExSize :', SysSize+1 )    ;
   IF Q.BuffOption=1 THEN SysExCheckSum (1) ;
 END                                        ;

 IF ErrorCount>0 THEN
 BEGIN
   WRITE (NL, ' [',ErrorCount,'] MidiError(s) detected.')  ;
   FOR i := 0 TO ErrorCount-1 DO
   BEGIN
     WRITE (NL, ' ', ErrorCount, ': Pos:', MError[i].Pos ) ;
     WRITE (' Chr:', MError[i].MidiChr )                   ;
   END                                                     ;
 END                                                       ;

 IF SERErrorCount>1 THEN
 WRITE (NL, ' Serial Error:[', SERError, '] Count:[',SERErrorCount-1,
            '] Pos:[',SERErrorPos,']') ;

 WRITE (NL,' CODES: Transmitted:[',WriteCount,'] Received:[',ReadCount ,']') ;
 WRITE (NL,' BUFF : N:[',NBuff.Pos,'] O:[',OBuff.Pos,'] S:[',SBuff.Pos, ']') ;
 IF (OverFlow>0) THEN WRITE (NL, ' (Buffer overflow: Max exceeded.)')        ;
 Choice := KeyRead (Choice)       ;
END                               ;
{---------------------------------}
{ INCLUDE FILES :                 }
{---------------------------------}

{$I PATCH\Midiprg3.INC }  { Matrix Setup }
{$I PATCH\Midiprg6.INC }  { Options Menu }
{$I PATCH\Midiprg8.INC }  { Main Menu    }
{-----------------------------------------}
{ Main                                    }
{-----------------------------------------}
BEGIN
   P := @WriteCode3                  ;
   Write3Addr := OFS(P^) + 3         ;
   P := @SwopBuffers                 ;
   SwopBuffersAddr := OFS(P^) + 3    ;

   WDummy   := 0                          ;
   RDummy   := 0                          ;

   Timer0CFG (1)                          ;
   TimerCnt := 0                          ;
   EventCnt := 0                          ;

   CFG := 1                               ;
   KEY := 1                               ;
   DTA := 1                               ;

   CheckParamStr                          ; { Can reset CFG, KEY, DTA }

   InitialiseHWConfig (0)                 ;
   Initialise8250Baud                     ;
   WritePortCfg (9)                      ;
   WRITE (NL,NL,' PORT TYPE:[',P1.SERUartType,'] Press Enter...')  ;
   Choice := KeyRead (Choice)             ;

   InitialiseCAddr                        ;

   IF KEY=1 THEN
   BEGIN
     ClearFLinkList                       ;
     ClearFKeyList                        ;
     ClearRemote                          ;
   END                                    ;

   SetINTVector                           ;

   IF CFG=1 THEN BEGIN
                   ResetChannelMatrix     ;
                   ClearFilterLists       ;
                 END                      ;

   OK := PortInOK (C[1].PortIn)           ; { Initialise PortInA,B }
   OK := PortOutOK(C[1].SysPortOut)       ; { Init. PortOutA,B,C,D }

   FEFilter := C[0].FList[9]              ;
   F8Filter := C[0].FList[5]              ;

   {--------------------------------------}

   MainMenu                               ;

   Timer0CFG (0)                          ;
   ResetINTVector                         ;
END                                       .

