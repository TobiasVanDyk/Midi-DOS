UNIT MIDIPRG2 ;

INTERFACE

USES CRT, DOS, MIDIPRG1                    ;

PROCEDURE Timer0CFG ( Option : BYTE )      ;
PROCEDURE Timer0INT                        ;
PROCEDURE INTHandler                       ;
PROCEDURE EnableINT                        ;
PROCEDURE DisableINT                       ;
PROCEDURE SetINTVector                     ;
PROCEDURE ResetINTVector                   ;
PROCEDURE DoHelp  (Option: CHAR)           ;

IMPLEMENTATION
{----------------------------------------------}
PROCEDURE Timer0CFG  ( Option : BYTE )         ;
{----------------------------------------------}
BEGIN
  IF (Option=1) THEN TimerDiv := Hz96
                ELSE TimerDiv := HzNormal;

  INLINE ( $FA )                         ; { CLI }
  PORT [TimerCtrl] := $36                ; { 0011 0110 : 00:Timer0          }
                                           { 11:LSB,MSB DIV Value           }
                                           { 011:Mode3  0:BIN Count         }
  INLINE ($EB/$00)                       ; { JMP 00 }
  PORT [Timer0] := LO(TimerDiv)          ;
  INLINE ($EB/$00)                       ; { JMP 00 }
  PORT [Timer0] := HI(TimerDiv)          ;
  INLINE ( $FB )                         ; { STI }
END                                      ;
{------------------------------------}
{ TIMER INT Handler                  }
{------------------------------------}
PROCEDURE Timer0INT      ; ASSEMBLER ;
   ASM
      PUSH AX
      PUSH DS
      MOV  AX, SEG @DATA
      MOV  DS, AX
      ADD  WORD PTR TimerCnt  , $01 ;
      ADC  WORD PTR TimerCnt+2, $00 ;
      POP  DS
      POP  AX
      STI
      IRET
   END                     ;
{----------------------------------------------------------------------}
(*
       ComXLate : ARRAY [0..255] OF BYTE =
       (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
{00-7F} 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
{80-9F} 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
{A0-BF} 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
{C0-DF} 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
{E0-EF} 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
{F0-FF} 4,1,3,2,1,1,1,5,6,7,7,7,7,1,8,1) ;
{       F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF }
*)
{--------------------------------------------}
{ INT Handler (Initialise DS:= Global Data   }
{--------------------------------------------}
PROCEDURE INTHandler                         ;

LABEL L0,L1,L2,L3,L4,L5,L6,L7,L8,LL0,LL1,LL2,LL3 ;
BEGIN
   ASM
      PUSH AX
      PUSH DI
      PUSH DX
      PUSH DS
      MOV  AX, SEG @DATA
      MOV  DS, AX
   END                 ;

   ComStatus := PORT[P1.SERLineStat]        ;
   ComStatus := ComStatus AND $0F           ;
   IF ComStatus = 1 THEN GOTO LL2           ;
   INC (SERErrorCount)                      ;
   SERError := ComStatus                    ;
   SERErrorPos := ReadCount                 ;
   SERReadByte := PORT [P1.SERData]         ;
   GOTO LL1                                 ;

   LL2:
   INC (ReadCount)                          ;

   ReadByte := PORT [P1.SERData]            ;
   ComClass := ComXlate [ReadByte]          ;

   IF ComClass = 0 THEN GOTO L0 ;
   IF ComClass = 1 THEN GOTO L1 ;
   IF ComClass = 2 THEN GOTO L2 ;
   IF ComClass = 3 THEN GOTO L3 ;
   IF ComClass = 4 THEN GOTO L4 ;
   IF ComClass = 5 THEN GOTO L5 ;
   IF ComClass = 6 THEN GOTO L6 ;
   IF ComClass = 7 THEN GOTO L7 ;
   IF ComClass = 8 THEN GOTO L8 ;

   L0: IF (ComSysEx>0) THEN GOTO LL0       ;
       IF (ComCount=0) THEN GOTO LL3       ;
       DEC (ComCount)                      ;
       GOTO LL0                            ;
   LL3:
       CBuff[CBuffPutLo]:=ComCode          ;
       INC (CBuffPutLo)                    ;
       ComCount := ComSize                 ;
       DEC (ComCount)                      ;
       GOTO LL0                            ;
   L1: GOTO LL0                            ;
   L2: ComCount := 1                       ;
       ComSize  := 1                       ;
       ComCode  := ReadByte                ;
       GOTO LL0                            ;
   L3: ComCount := 2                       ;
       ComSize  := 2                       ;
       ComCode  := ReadByte                ;
       GOTO LL0                            ;
   L4: ComSysEx := 1                       ;
       GOTO LL0                            ;
   L5: ComSysEx := 0                       ;
       GOTO LL0                            ;
   L6: IF (F8Filter>0) THEN GOTO LL1       ;
       IF (ComCount>0) THEN GOTO LL1       ;
       GOTO LL0                            ;
   L7: IF (ComCount>0) THEN GOTO LL1       ;
       GOTO LL0                            ;
   L8: IF (FEFilter>0) THEN GOTO LL1       ;
       IF (ComCount>0) THEN GOTO LL1       ;
       IF (ComSysEx>0) THEN GOTO LL1       ;

   LL0:CBuff[CBuffPutLo] := ReadByte       ;
       INC (CBuffPutLo)                    ;

   LL1:
   ASM
      MOV AL, $20
      OUT $20, AL
      POP DS
      POP DX
      POP DI
      POP AX
      MOV SP, BP
      POP BP
      STI
      IRET
   END         ;
END                            ;
{------------------------------}
{ Enable 8250 Interrupts       }
{------------------------------}
PROCEDURE EnableINT            ;
VAR A : BYTE                   ;
    i : INTEGER                ;
BEGIN
{ PORT [P1.SERIntE]:=$01       ;}{ Enable INT (if D7 Port 3xB (DLAB bit)=0) }
  PORT [P1.SERModemCtrl]:=$08  ; { Set OUT2=1 : Master INT Enable           }
  FOR i := 0 TO 7 DO
      A := PORT [P1.SERData+i] ; { Clear Ports by reading                   }
  INLINE ( $FB )               ; { STI Enable 80x86 INT                     }
END ;
{------------------------------}
{ Disable 8250 Interrupts      }
{------------------------------}
PROCEDURE DisableINT           ;
BEGIN
{ PORT [P1.SERIntE]:=$00       ;}{ Disable INT (if D7 Port 3xB (DLAB bit) = 0)}
  PORT [P1.SERModemCtrl]:=$00  ; { Set OUT2=0 : Master INT Disable            }
END ;
{------------------------}
{ Set INT vector to User }
{------------------------}
PROCEDURE SetINTVector                ;

VAR A, B : BYTE                       ;
BEGIN
  {-----------------------------------}
  { Install new SER INT Vector        }
  {-----------------------------------}
  GETINTVEC (P1.INTNumber, OldINTVector);
  GETINTVEC ($1C         , OldTimerINT );
  INLINE ( $FA )                        ; { CLI Disable 80x86 INT             }
  SETINTVEC (P1.INTNumber, @INTHandler) ; { @: W1=OFS(INTHandler), W2=CSEG    }
  SETINTVEC ($1C         , @Timer0INT ) ; { @: W1=OFS(TimerINT)  , W2=CSEG    }
  INLINE ( $FB )                        ; { STI Enable 80x86 INT              }
  {-----------------------------------}
  { Convert IRQ=3,4,5,7 TO PICMask    }
  {-----------------------------------}
  A := $FF                            ;

  CASE P1.IRQ OF $02 : A := $FB          ; { 1111 1011 } { IRQ02 used on AT ! }
                 $03 : A := $F7          ; { 1111 0111 }
                 $04 : A := $EF          ; { 1110 1111 }
                 $05 : A := $DF          ; { 1101 1111 }
                 $06 : A := $BF          ; { 1011 1111 }
                 $07 : A := $7F          ; { 0111 1111 }
  END                                 ;
  {-----------------------------------}
  { Enable 8259 INT controller        }
  {-----------------------------------}
  B := PORT [PICMask]                 ; { $021: Get current INT status  }
  B := B AND A                        ; { 1100 0011 : Enable serial INT }
  PORT [PICMask] := B                 ; { D4,D3=0   : Enable IRQ4, IRQ3 }
END                                   ;
{----------------------------}
{ Reset INT vector to System }
{----------------------------}
PROCEDURE ResetINTVector     ;
BEGIN
  INLINE ( $FA )                         ; { CLI Disable 80x86 INT }
  SETINTVEC (P1.INTNumber, OldINTVector) ;
  SETINTVEC ($1C         , OldTimerINT ) ; { @: W1=OFS(TimerINT)  , W2=CSEG    }
  INLINE ( $FB )                         ; { STI Enable 80x86 INT  }
END                                      ;
{----------------------------}
{ Help Pages                 }
{--------------------------------}
PROCEDURE DoHelp (Option : CHAR) ;
{--------------------------------}
TYPE FType    = ARRAY [0..76] OF CHAR  ;
     FArrType = ARRAY [1..23] OF FType ;

VAR
  OptionNum : INTEGER ;
  n         : INTEGER ;
  FArr      : FArrType;
  TopStr    : Str80   ;
{-------------------------------}
PROCEDURE ReadPage ( N: INTEGER);
{-------------------------------}
VAR
  F         : FILE    ;
  FError    : INTEGER ;
BEGIN
   ASSIGN  (F, 'MIDIPRG.HLP' )  ;
  {$I-}
   RESET   (F, SIZEOF(FArr) )   ;
   FError := IORESULT           ;
  {$I+}
   IF (FError<>0) THEN EXIT     ;
   SEEK (F, N-1)                ;
  {$I-}
   BLOCKREAD (F, FArr, 1)       ;
   FError := IORESULT           ;
  {$I+}
   IF (FError<>0) THEN BEGIN CLOSE (F) ; EXIT ; END ;
   CLOSE (F)                    ;
END                             ;
{----------------------------------------------------}
PROCEDURE WritePage ( Page : FArrType; N : INTEGER ) ;
{----------------------------------------------------}
CONST AnyKey = '[ANY-KEY]' ;
{-------------------------------------------------------------}
{ Each Page = 77 chars * 23 lines = 1771 bytes                }
{ Pagenum = number of pages in each option                    }
{-------------------------------------------------------------}
PageNum: ArrInt16 = (3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1)          ;
PagePos: ArrInt16 = (1,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18) ;

VAR i, M   : INTEGER     ;

BEGIN
  DoWindow (Cyan, Blue, 1,1,80,25,1,0) ;
  WRITE ('  ', StrL(75,0),NL)          ;
  M := PagePos[N]                      ;

   REPEAT
      TEXTCOLOR (Blue)                ;
      ReadPage ( M )                  ;
      TopStr := COPY (FArr[1], 2, 70) ;

      FOR i := 1 TO 23 DO
      BEGIN
        GOTOXY (1,i+1)                         ;
        WRITE ('  ',FArr[i])                   ;
        IF i=1 THEN BEGIN
                      GotoColorStr (White,4,2,TopStr) ;
                      TEXTCOLOR (Blue)                ;
                      END                             ;
      END                                             ;

      WRITE ('  ', StrL(75,1))          ;
      INC (M)                           ;
      GotoColorStr (Black,65,24,AnyKey) ;
      Option := KeyRead ( Option )      ;
   UNTIL (M>PageNum[N])                 ;
END                                     ;

{------------ MAIN -------------------------}
BEGIN                      { Help Main Menu }
   DoWindow (Cyan, Blue, 1,1,80,25,1,0) ;
   OptionNum := ORD(Option) - 48        ;
   WritePage ( FArr, OptionNum )        ;
END                                     ;

BEGIN
END .