
{---------------------------------}
{ Channel Matrix                  }
{---------------------------------}
PROCEDURE ChannelMatrix (X: CHAR) ;
CONST
L2 = #10#08#08 ; { LF + 2 BS }
L1 = #10#08    ;

VAR
    Update         : BOOLEAN ;
    E              : INTEGER ;
    MidiF          : FILE    ;
    FStr           : Str20   ;
    FError         : INTEGER ;
    FSize          : LONGINT ;
{--------------------------------------------}
FUNCTION PortLetter (P: POutType) : CHAR     ;
{--------------------------------------------}
VAR K : CHAR           ;
BEGIN
  K := ' '             ;
  IF P.D>0 THEN K:='D' ;
  IF P.C>0 THEN K:='C' ;
  IF P.B>0 THEN K:='B' ;
  IF P.A>0 THEN K:='A' ;
  PortLetter := K      ;
END                    ;
{-----------------------------------}
PROCEDURE WriteBottom (Option: BYTE);
{-----------------------------------}
CONST
Arr  = #26#27          ;
AR   = #24+#25+#26+#27 ;
ArUD = #24#25          ;

M1 = '[Esc][Q]uit ['+AR+'][Home-End] [RET]Detail [C]hannel [P]itch [V]elocity [O]ther'+NL+
     '(PortOut:[N]ormal-[S]ystem  PortIn:[I])+[A][B][C][D] [K]eybrdSplit [A]ctivate'  ;
O1 = ' [A]PolyKey [B]Contr. [C]Patch [D]Aftertouch [E]Bend [Y]Contr.Detail [Q]uit'+NL+
     ' [I]input [O]utput Channel+['+ArUD+'] [X]:Clear all [Max 10 Contr. per list]'    ;
S1 = ' ['+Arr+']:Move  [B][SHIFT-B]:Begin [E][SHIFT-E]:End  (C-B)or(Db-Bb)   [ESC]:Quit'+NL+
     ' [1]-[4]:SplitNumber [I]nput+['+ArUD+'] [O]utput Channel+[n] [P]:Pitch [V]:Velocity' ;
T2 = ' [I]nputChannel+['+ArUD+'] [O]utputChannel ['+ArUD+']Move [Num]NewPatchNum [R]eset [Q]uit'           ;
X1 = ' ['+Arr+']Move [ENTER]Select-Deselect Output Channel [Q]uit' ;
VAR X: INTEGER                                             ;
BEGIN
  IF (Option<4) THEN X:=24 ELSE X:=25   ;
  DoWindow (LightGray,Blue,1,X,80,25,1,0) ;
  CASE Option OF
       0: WRITE (M1)         ;
       1: WRITE (O1)         ;
       2: WRITE (S1)         ;
       3: WRITE (Sp)         ;
       4: WRITE (T2)         ;
       5: WRITE (X1)         ;
   END                       ;
END                                      ;
{-----------------------------------------}
{ Convert Pitch and Velocity into String  }
{-----------------------------------------}
FUNCTION GetPitchVel (I:INTEGER) : Str6   ; { Returns a str=6, R: or C: }
VAR X, N     : Str6          ;
    BHi, BLo : BYTE          ;
BEGIN
  BHi := HI(I)               ;
  BLo := LO(I)               ;
  IF BHi=1 THEN X:= 'R:-'    ;
  IF BHi=2 THEN X:= 'C: '    ;
  IF BHi=0 THEN X:= 'R:+'    ;
  STR ( BLo:3, N )           ;
  GetPitchVel := X + N       ;
END                          ;
{--------------------------------------------------------------------}
PROCEDURE GetCOut (VAR OutCh: ArrB16; S: Str24)                      ;
{--------------------------------------------------------------------}
CONST
 A1='³Select Channel Out :                                        ³'+NL+
    'ÃÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄ´'+NL+
    '³CHANNEL OUT:³O1³O2³O3³O4³O5³O6³O7³O8³O9³1O³11³12³13³14³15³16³'+NL+
    'ÀÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÙ' ;
 NumArr:ARRAY [0..15] OF Str2 =('O1','O2','O3','O4','O5','O6','O7','O8',
                                'O9','1O','11','12','13','14','15','16' ) ;
 PosArr:ARRAY [0..15] OF BYTE =
      ( 15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60 ) ;

VAR XY, i : INTEGER ;
    X, Y  : CHAR    ;

BEGIN
  DoWindow (Cyan, Yellow, 1,24,80,25,1,0)      ;
  WriteBottom (5)                              ;
  DoWindow ( Cyan, Blue, 12,12,73,16, 1,0)     ;
  DoWindow ( Cyan, Blue, 12,12,74,16, 0,0)     ;
  WRITE (StrL(62,0),NL,A1)                     ;
  GotoColorStr (Black,24,2,S)                  ;
  XY := 0                                      ;
  REPEAT
    FOR i := 0 TO 15 DO
    BEGIN
      GOTOXY (PosArr[i], 4)                                               ;
      IF OutCh[i]>0 THEN TEXTATTR := CyanWhite ELSE TEXTATTR := CyanBlack ;
      WRITE (NumArr[i])                                                   ;
    END                                                                   ;
    GOTOXY (PosArr[XY], 4)                      ;
    Y := UPCASE (READKEY)                       ;
    CASE Y OF #00: BEGIN
                     X := KeyRead(X)            ;
                     IF X=ArR THEN INC (XY)     ;
                     IF X=ArL THEN DEC (XY)     ;
                   END                          ;
              #13: BEGIN
                     OutCh[XY] := OutCh[XY] XOR 1 ;
                     INC (XY)                     ;
                   END                            ;
    END                                           ;
    IF XY>15 THEN XY:= 0       ;
    IF XY<0  THEN XY:=15       ;
  UNTIL (Y='Q') OR (Y=#27)     ;
  COut := XY                   ;
END                            ;
{------------------------------------}
PROCEDURE DoCInCout (VAR I: INTEGER) ;
{------------------------------------}
VAR X,Y : CHAR                       ;
BEGIN
  X := UPCASE (READKEY)   ;
  IF X=#00 THEN
  BEGIN
    Y := KeyRead(Y)         ;
    IF Y=ArU THEN INC (I)   ;
    IF Y=ArD THEN DEC (I)   ;
    IF I > 15  THEN I:=0    ;
    IF I < 0   THEN I:=15   ;
  END                       ;
END                         ;
{---------------------------------------------------------------}
 FUNCTION GetNumber (Option : BYTE ; X,Y : INTEGER ) : INTEGER  ;
{---------------------------------------------------------------}
VAR NStr    : Str20   ;
    N, Z    : INTEGER ;
    Constant: BYTE    ;
    S, T    : Str10   ;
    BHi,BLo : BYTE    ;

CONST ClrStr  = '......' + #8#8#8#8#8#8       ;

BEGIN
  GetNumber := 0                              ;
  E         := 0                              ;

  REPEAT
    IF (Option>0) THEN CLRSCR                 ;
    Constant := 0                             ;

    IF Option IN [1,2] THEN
    BEGIN
      IF Option=1 THEN BEGIN
                         T := 'Pitch'            ;
                         Z := C[Cin].Pitch[COut] ;
                      END                        ;
      IF Option=2 THEN BEGIN
                         T := 'Velocity'         ;
                         Z := C[Cin].Vel[COut]   ;
                      END                        ;
      S := GetPitchVel ( Z )                               ;
      WRITE ('Current ', T, ' : [', S, '] ' )              ;
      WRITE ('New ', T, ' [0:clear][Cnumber:constant] : ') ;
    END                                                    ;

    GOTOXY (X,Y) ; Write (ClrStr) ;

    READLN (NStr)                                   ;
    NStr [1] := UPCASE (NStr[1])                    ;

    IF NStr[1]='C' THEN BEGIN
                          NStr[1]  := ' ' ;
                          Constant := 1   ;
                        END               ;

    IF (NStr[1]) = 'Q' THEN E := 10
    ELSE BEGIN
           VAL ( NStr, N, E )                        ;
           IF E=0 THEN
           BEGIN                                     ;
             IF N<0  THEN IF ABS(N)>127 THEN E := 1  ;
             IF N>=0 THEN IF      N>127 THEN E := 2  ;
             IF Constant=1 THEN N := ABS(N)          ;
           END                                       ;
         END                                         ;
  UNTIL (E=0) OR (E=10)                              ;

  IF (N<0) THEN BEGIN
                  BLo := LO((-1)*N)           ;
                  BHi := 1                    ;
                END
           ELSE BEGIN
                  BLo := LO(N)                ;
                  BHi := 0                    ;
                  IF Constant=1 THEN BHi := 2 ;
                END                           ;

  N   := (BHi*256) + BLo ;
  GetNumber := N         ;
 END                     ;
{----------------------------------------------------}
PROCEDURE WriteController ( S : Str1 )               ;
{----------------------------------------------------}
BEGIN
  IF (C[CIn].ChMess.A0[COut]>0) THEN WRITE ('A',S) ;
  IF (C[CIn].ChMess.B0[COut]>0) THEN WRITE ('B',S) ;
  IF (C[CIn].ChMess.C0[COut]>0) THEN WRITE ('C',S) ;
  IF (C[CIn].ChMess.D0[COut]>0) THEN WRITE ('D',S) ;
  IF (C[CIn].ChMess.E0[COut]>0) THEN WRITE ('E',S) ;
END                                                ;
{--------------------------------------------------}
{ Controller data                                  }
{--------------------------------------------------}
PROCEDURE DoA0ToEF ;

LABEL EXIT1        ;

VAR i,n,k,m : INTEGER ;
    X, Y    : CHAR    ;

CONST
T2 ='³  Code:               ³[Bn]:Data Byte1 : Classification               ³'+NL+
    '³  [An]:PolyKey Press. ³[OOO-O31]: Cont. Controllers 1-32 (MSB)        ³'+NL+
    '³  [Bn]:Control Change ³[O34-O63]: Cont. Controllers 1-32 (LSB)        ³'+NL;
T5 ='³  [Cn]:Patch Change   ³[O64-O95]: Switches 1-32                       ³'+NL+
    '³  [Dn]:Ch. Aftertouch ³[122-127]: Channel Mode Messages               ³'+NL+
    '³  [En]:Pitch Bend     ³[O96-121]: Undefined                           ³'+NL;
T8 ='ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´'+NL+
    '³   MSB  LSB  Continuous Controller ³Number  Channel Mode Messages:    ³'+NL+
    '³  [OO1][O32] 1: Modulation Wheel   ³[122]: Local Control On-Off       ³'+NL;
T11='³  [OO2][O33] 2: Breath Controller  ³[123]: All Notes Off              ³'+NL+
    '³  [OO4][O35] 4: Foot Control       ³[124]: OMNI Mode Off              ³'+NL+
    '³  [OO5][O36] 5: Portamento Time    ³[125]: OMNI Mode On               ³'+NL;
T14='³  [OO6][O37] 6: Data Entry Knob    ³[126]: MONO Mode (POLY Off)       ³'+NL+
    '³  [OO7][O38] 7: Main Volume        ³[127]: POLY Mode (MONO Off)       ³'+NL+
    'ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´'+NL;
T17='³  Status: Ch.Out[  ] Ch.In[  ]  Messages [  ]  List [           ] MAX ³'+NL+
    '³  CList1: Ch.Out[  ] [                                          ] 1O  ³'+NL+
    '³  CList2: Ch.Out[  ] [                                          ]     ³'+NL;

O4 =   ' Select List (1 or 2) : '                                       ;
O5 =   ' Select Output Channel (1-16) for List '                        ;
O6 =   ' Enter Controller Number [1-127] (Q:quit) '                     ;
O7 =   'A'+L1+'B'+L1+'C'+L1+'D'+L1+'E'                                  ;

{-------------------------}
PROCEDURE WriteStatus     ;
{-------------------------}
VAR i : INTEGER           ;
{-------------------------------------------------------------------}
PROCEDURE WriteC ( X: BYTE; S: Str6 )                               ;
BEGIN IF (X<>255) THEN WRITE (X+1:2, S ) ELSE WRITE ('**', S ); END ;
{-------------------------------------------------------------------}
{------------------------------------------------}
PROCEDURE WriteD ( Y: INTEGER; X: ArrB9 )        ;
VAR i : INTEGER                                  ;
BEGIN
  GOTOXY (25,Y)                                  ;
  FOR i := 0 TO 9 DO WRITE (X[i]:3, ' ')         ;
END                                              ;
{-------------------------------------------------------------------}
BEGIN
   {--------------------------------------------------------------}
   { Write Current Status                                         }
   {--------------------------------------------------------------}
   GotoColorStr (Black,5,3,O7)                                    ;
   GOTOXY (19,17) ; WRITE (COut+1:2, L2 )                         ;

   WriteC ( C[Cin].ChMessB0.COut1, L2 )   ;
   WriteC ( C[Cin].ChMessB0.COut2, '' )   ;

   GOTOXY (29,17) ; WRITE (CIn+1:2)                     ;
   GOTOXY (44,17) ; WRITE (C[CIn].ChMessX:2)            ;

   GOTOXY (56,17) ; TEXTCOLOR (Red)                     ;
   WriteController ( ' ' )                              ;

   WriteD ( 18, C[Cin].ChMessB0.ContrList1 ) ;
   WriteD ( 19, C[Cin].ChMessB0.ContrList2 ) ;

   GOTOXY (68,19)                                       ;
END                                                     ;
{-------------------------------------------------}
PROCEDURE INCM ( VAR M: INTEGER; X : ArrB16 )     ;
VAR i : INTEGER                                   ;
BEGIN FOR i := 0 TO 15 DO IF (X[i]>0) THEN INC(M) ;
END                                               ;
{--------------------- MAIN ---------------------------------}
 BEGIN
   DoWindow (Cyan     , Blue, 1, 2, 80, 23 , 1,0)            ;
   DoWindow (LightGray, Blue, 4, 3, 75, 22 , 1,1)            ;
   REPEAT
     WriteBottom (1)                                         ;
     DoWindow (LightGray, Blue, 4, 3, 76, 22 , 0,0)          ;

     WRITE ( StrL(72,0), NL, T2, T5, T8, T11,T14,T17,StrL(72,1) )  ;
     WriteStatus                                                   ;
     {---------------------------------}
     { Get Selection                   }
     {---------------------------------}
     Y := KeyRead ( Y )                ;
     Update := TRUE                    ;
     i := 0                            ;

     CASE Y OF
     'Q',#27: EXIT   ;
         'X': FOR n := 0 TO 15 DO
              BEGIN
                C[CIn].ChMess.A0[n] := 0 ;
                C[CIn].ChMess.B0[n] := 0 ;
                C[CIn].ChMess.C0[n] := 0 ;
                C[CIn].ChMess.D0[n] := 0 ;
                C[CIn].ChMess.E0[n] := 0 ;
                IF (C[Cin].ChMessB0.COut1=COut) AND (n<10) THEN
                BEGIN
                  C[CIn].ChMessB0.COut1 := 255         ;
                  C[CIn].ChMessB0.ContrList1[n] := 0   ;
                END                                    ;
                IF (C[Cin].ChMessB0.COut2=COut) AND (n<10) THEN
                BEGIN
                  C[CIn].ChMessB0.COut2 := 255         ;
                  C[CIn].ChMessB0.ContrList2[n] := 0   ;
                END                                    ;
              END                                      ;
         'A': GetCOut ( C[CIn].ChMess.A0, '[An]:PolyKey Pressure'   ) ;
         'B': GetCOut ( C[Cin].ChMess.B0, '[Bn]:Control Change'     ) ;
         'C': GetCOut ( C[Cin].ChMess.C0, '[Cn]:Patch Change'       ) ;
         'D': GetCOut ( C[Cin].ChMess.D0, '[Dn]:Channel Aftertouch' ) ;
         'E': GetCOut ( C[Cin].ChMess.E0, '[En]:Pitch Bend'         ) ;
         'I': DoCInCout ( CIn)  ;
         'O': DoCInCout ( COut) ;
         'Y': BEGIN
                REPEAT
                    DoWindow (LightGray, Blue, 1,24,80,25,1,0) ;
                    WRITE (O4)                         ;
                    i := GetNumber (0,25,1)            ;   { List1, List2   }
                    IF E=10 THEN GOTO EXIT1            ;
                    IF NOT(i IN [1,2]) THEN E:= 1      ;
                    IF E=0 THEN
                    BEGIN
                      WRITE (O5, '[',i:1, '] : ')      ;
                      n := GetNumber (0,46,2)          ;   { OutPut Channel }
                      IF E=10 THEN GOTO EXIT1          ;
                      IF NOT(n IN [1..16]) THEN E := 2 ;
                    END                                ;
                UNTIL (E=0)                            ;
                IF E<>10 THEN
                BEGIN
                  IF i=1 THEN C[Cin].ChMessB0.COut1 := n-1 ;
                  IF i=2 THEN C[Cin].ChMessB0.COut2 := n-1 ;
                  IF i>0 THEN C[Cin].ChMess.B0[n-1] := 1   ; { Force On }
                  k := 0                          ;         { List pos count}
                  REPEAT
                    CLRSCR                        ;
                    WRITE (O6, ' [',k, '] = ')    ;
                    m := GetNumber (0,50,1)       ; { Contr num 0-127}
                    IF (E=0) THEN
                    BEGIN
                      IF NOT(m IN [0..127]) THEN E := 3              ;
                      IF i=1 THEN C[Cin].ChMessB0.ContrList1[k] := m ;
                      IF i=2 THEN C[Cin].ChMessB0.ContrList2[k] := m ;
                      INC (k)                                        ;
                     END                                             ;
                  UNTIL (E=10) OR (k>9)                              ;
               END                                    ; {E<>10 }
             END                                      ; {Y='Y' }
        END                                           ; { Case Y }

     EXIT1:

     m := 0                     ;
     INCM (m, C[Cin].ChMess.A0) ;
     INCM (m, C[Cin].ChMess.B0) ;
     INCM (m, C[Cin].ChMess.C0) ;
     INCM (m, C[Cin].ChMess.D0) ;
     INCM (m, C[Cin].ChMess.E0) ;
     C[CIn].ChMessX := m        ;

   UNTIL (Y='Q')                ;
 END                            ;
{-------------------------------}
PROCEDURE DoPatchXlat           ;
{-------------------------------}
PROCEDURE XPatch ;
CONST K = 6  ;
      L = 15 ;
      M = 24 ;
      U = 33 ;
      O = 42 ;
      P = 51 ;
      Q = 61 ;
      R = 71 ;
S1='ÚÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄ¿'+NL+
   '³IN  OUT ³IN  OUT ³IN  OUT ³IN  OUT ³IN  OUT ³IN  OUT ³ IN  OUT ³ IN  OUT ³'+NL;
S3='³16:[127]³32:[127]³48:[127]³64:[127]³80:[127]³96:[127]³112:[127]³128:[127]³'+NL;
S4='ÃÄÄÄÄÄÄÄÄÁÄÄÂÄÄÂÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄ´'+NL+
   '³CHANNEL IN:³  ³ PATCH TRANSLATION TABLE.                                 ³'+NL+
   'ÀÄÄÄÄÄÄÄÄÄÄÄÁÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ';

XYPos : ARRAY [0..127,0..1] OF BYTE = (
(K,3),(K,4),(K,5),(K,6),(K,7),(K,8),(K,9),(K,10),(K,11),(K,12),(K,13),(K,14),(K,15),(K,16),(K,17),(K,18),
(L,3),(L,4),(L,5),(L,6),(L,7),(L,8),(L,9),(L,10),(L,11),(L,12),(L,13),(L,14),(L,15),(L,16),(L,17),(L,18),
(M,3),(M,4),(M,5),(M,6),(M,7),(M,8),(M,9),(M,10),(M,11),(M,12),(M,13),(M,14),(M,15),(M,16),(M,17),(M,18),
(U,3),(U,4),(U,5),(U,6),(U,7),(U,8),(U,9),(U,10),(U,11),(U,12),(U,13),(U,14),(U,15),(U,16),(U,17),(U,18),
(O,3),(O,4),(O,5),(O,6),(O,7),(O,8),(O,9),(O,10),(O,11),(O,12),(O,13),(O,14),(O,15),(O,16),(O,17),(O,18),
(P,3),(P,4),(P,5),(P,6),(P,7),(P,8),(P,9),(P,10),(P,11),(P,12),(P,13),(P,14),(P,15),(P,16),(P,17),(P,18),
(Q,3),(Q,4),(Q,5),(Q,6),(Q,7),(Q,8),(Q,9),(Q,10),(Q,11),(Q,12),(Q,13),(Q,14),(Q,15),(Q,16),(Q,17),(Q,18),
(R,3),(R,4),(R,5),(R,6),(R,7),(R,8),(R,9),(R,10),(R,11),(R,12),(R,13),(R,14),(R,15),(R,16),(R,17),(R,18));

VAR Num, i, n, E : INTEGER ;
    AStr         : Str10   ;
    X, Y         : CHAR    ;
    xy           : BYTE    ;
    UpDate       : BOOLEAN ;

PROCEDURE WLine  ;
VAR i,n : INTEGER  ;
BEGIN
  FOR i := 0 TO 127 DO
  BEGIN
    TEXTCOLOR (Black)                                ;
    IF i<96 THEN n:=2 ELSE n:=3                      ;
    GOTOXY (XYPos[i,0]-2-n, XYPos[i,1])              ;
    WRITE (i:n)                                      ;
    TEXTCOLOR (Blue)                                 ;
    GOTOXY (XYPos[i,0], XYPos[i,1])                  ;
    WRITE ( C[CIn].PatchXlat[i]:3 )                  ;
  END                                                ;
END                                                  ;

BEGIN
  xy  := 0              ;
  UpDate := TRUE        ;

  REPEAT
   WriteBottom (4)                              ;
   DoWindow (LightGray, Blue, 3, 3, 78, 23 , 0,0) ;
   IF UpDate THEN
   BEGIN
      DoWindow (Cyan     , Blue, 1, 2, 80, 24 , 1,0) ;
      DoWindow (LightGray, Blue, 3, 3, 78, 23 , 1,0) ;
      WRITE (S1)                        ;
      FOR i := 1 TO 16 DO WRITE (S3)    ;
      WRITE (S4)                        ;
      WLine                             ;
    END                                 ;
    TEXTCOLOR (Red)                     ;
    GOTOXY (14,20) ; WRITE ( CIn+1:2 )  ;
    TEXTCOLOR (Blue)                    ;

    GOTOXY ( XYPos[xy,0],XYPos[xy,1] ) ;
    UpDate := FALSE                    ;
    Y := UPCASE (READKEY)              ;

    CASE Y OF
         #00: BEGIN
                X := KeyRead(X)          ;
                IF X = ArU THEN DEC (xy) ;
                IF X = ArD THEN INC (xy) ;
                IF xy=128 THEN xy:=0     ;
                IF xy=255 THEN xy:=127   ;
              END                        ;
    '0'..'9': BEGIN
                GOTOXY ( XYPos[xy,0],XYPos[xy,1] ) ;
                WRITE  ( '...', #8#8#8           ) ;
                WRITE (Y)            ;
                READLN (AStr)        ;
                AStr := Y + AStr     ;
                VAL ( AStr, Num, E ) ;
                IF (E=0) AND (Num IN [0..127]) THEN
                BEGIN
                  C[Cin].PatchXlat[xy] := Num        ;
                  GOTOXY ( XYPos[xy,0],XYPos[xy,1] ) ;
                  TEXTCOLOR (White)                  ;
                  WRITE (Num:3)                      ;
                END                                  ;
              END                                    ;
          'O':BEGIN
                GetCOut (C[Cin].PatchCOut,'Patch Channel Out Enable');
                UpDate := TRUE                                       ;
              END                                                    ;
          'I':DoCInCout ( CIn) ;
          'R':BEGIN
                FOR i := 0 TO 127 DO C[Cin].PatchXLat[i] := i ;
                UpDate := TRUE                                ;
              END                                             ;
       END                                                  ;  { Case Y }
  UNTIL (Y='Q') OR (Y=#27)                                  ;
END                                                         ;
{-------------- MAIN ---------------------------------------}
BEGIN
   XPatch                                        ;
END                                              ;
{---------------------}
{ Configure KeySplits }
{---------------------}
PROCEDURE DoKeyBSplit ;
CONST

K2 ='³  ÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝÝÛÝÝÛÝÝ³'+NL+
    '³  ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÝ³'+NL;
K4 ='³K:CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFG³'+NL;
K5 ='ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´'+NL;
{   '³1:CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFG³'}
{   '³2:CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFG³'}
{   '³3:CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFG³'}
{   '³4:CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFG³'}
K10='ÃÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄ´'+NL+
    '³Oct:O1  ³  O2  ³  O3  ³ O4   ³ O5   ³  O6  ³  O7  ³  O8  ³  O9  ³  1O     11 ³'+NL+
    '³N:OO--11³12--23³24--35³36--47³48--59³6O--71³72--83³84--95³96-1O7³1O8-119-127 ³'+NL;
K13='ÃÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄ´'+NL+
    '³ [ K:Key [C]-[B] ]  [ O:Octave [1]-[11] ]  [ N:Midi Note Code [OOO]-[127] ]  ³'+NL+
    'ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´'+NL;
K16='³ SplitPoints : Ch.In[  ]   ³ Num    ChOut   Begin   End    Pitch    Velocity ³'+NL+
    '³                           ³ [1] :  [10]    [100]-[100]    [R:-100] [R:-100] ³'+NL+
    '³ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿³ [2] :  [  ]    [   ]-[   ]    [      ] [      ] ³'+NL;
K19='³³                         ³³ [3] :  [  ]    [   ]-[   ]    [      ] [      ] ³'+NL+
    '³ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ³ [4] :  [  ]    [   ]-[   ]    [      ] [      ] ³'+NL+
    'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'+NL;
K22=' KEYBOARD  SPLITPOINT  DEFINITION  :  [Maximum 4 SplitPoints]. ';

B1='Ch.Out Num[1-16]  :'    ;
B2='Pitch[(c)(-)O-127]:'    ;
B3='Vel. [(c)(-)O-127]:'    ;
B4: STRING [76] =
'CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFG' ;
B5 ='K:'+L2+#10+'1:'+L2+'2:'+L2+'3:'+L2+'4:'                                  ;
B6 ='Ý'+#08#10+'Û'                                                            ;
B7 ='Ý'+#08#10+'Û'                                                            ;

TYPE BEType = RECORD
                Pos: INTEGER  ;
                FN : BOOLEAN  ;
              END             ;
VAR
 Note           : BYTE    ;
 xy             : BYTE    ;
 SNum           : INTEGER ;
 SStart, SStop  : INTEGER ;
 i, N           : INTEGER ;
 X, Y, Z        : CHAR    ;
 S              : Str10   ;
 B, E           : ARRAY [0..3] OF BEType  ;

CONST
  NArr : ARRAY [0..74] OF BYTE =
  ( 000,002,004,005,007,009,011, 012,014,016,017,019,021,023,
    024,026,028,029,031,033,035, 036,038,040,041,043,045,047,
    048,050,052,053,055,057,059, 060,062,064,065,067,069,071,
    072,074,076,077,079,081,083, 084,086,088,089,091,093,095,
    096,098,100,101,103,105,107, 108,110,112,113,115,117,119,
    120,122,124,125,127                                      ) ;
  FArr : ARRAY [0..74] OF BYTE =
  ( 000,001,003,005,006,008,010, 012,013,015,017,018,020,022,
    024,025,027,029,030,032,034, 036,037,039,041,042,044,046,
    048,049,051,053,054,056,058, 060,061,063,065,066,068,070,
    072,073,075,077,078,080,082, 084,085,087,089,090,092,094,
    096,097,099,101,102,104,106, 108,109,111,113,114,116,118,
    120,121,123,125,126                                      ) ;

 PROCEDURE WriteL (S:STRING)           ;
 BEGIN
   GOTOXY (3,19)                       ;
   WRITE  ('                         ');
   GOTOXY (3,19)                       ;
   WRITE (S)                           ;
 END                                   ;

 PROCEDURE InitBE                                                           ;
 CONST NotePos : ARRAY[0..11,0..1] OF BYTE =
 ((0,1),(1,0),(1,1),(2,0),(2,1),(3,1),(4,0),(4,1),(5,0),(5,1),(6,0),(6,1) ) ;
 { 0     1     2     3     4     5     6     7     8     9     10    11   }
 { C     Db    D     Eb    E     F     Gb    G     Ab    A     Bb    B    }
 {------------------------------------------------------------------------}
 VAR i            : INTEGER ;
     SB, SE, X, Y : BYTE    ;
 BEGIN
   FOR i := 0 TO 3 DO
   BEGIN
       SB := C[Cin].Split[i].Start        ;
       SE := C[Cin].Split[i].Stop         ;
       X  := SB MOD 12                    ;
       Y  := NotePos [X,0]                ;
       B[i].FN  := BOOLEAN(NotePos [X,1]) ;
       B[i].Pos := Y + (SB DIV 12)*7 + 1  ;
       X  := SE MOD 12                    ;
       Y  := NotePos [X,0]                ;
       E[i].FN  := BOOLEAN(NotePos [X,1]) ;
       E[i].Pos := Y + (SE DIV 12)*7 + 1  ;
   END                                    ;
 END                                      ;

 PROCEDURE WriteSplitPos                   ;
 VAR i, j, n, Color : INTEGER              ;

 BEGIN
   FOR i := -1 TO 3 DO
   BEGIN
     GOTOXY ( 4, 6+i-INTEGER(BOOLEAN(i=-1)) )                            ;
     FOR j := 1 TO 75 DO
     BEGIN
       IF i=-1 THEN BEGIN
                      IF (xy-3)=j THEN Color:=LightCyan ELSE Color:=White ;
                    END
       ELSE BEGIN
              Color := LightGray                                       ;
              IF (B[i].Pos<=j) AND (E[i].Pos>=j) THEN Color:=LightCyan ;
              IF ((j=B[i].Pos) AND NOT(B[i].FN)) OR
                 ((j=E[i].Pos) AND NOT(E[i].FN)) THEN Color:=LightBlue ;
              IF (C[Cin].SplitX=0)OR(C[CIn].Split[i].Stop=0) THEN
                  Color:=LightGray ;
              IF (xy-3)=j THEN IF SNum=i THEN Color := LightRed
                                         ELSE Color := White           ;
            END                                                        ;
       TEXTCOLOR (Color)                                               ;
       WRITE (B4[j])                                                   ;
     END                                                               ;
   END                                                                 ;
 END                                                                   ;

 PROCEDURE WriteSplit                      ;
 VAR i : INTEGER                           ;
 BEGIN
    FOR i := 0 TO 3 DO
    BEGIN
      GOTOXY (31,17+i)                                          ;
      IF SNum=i THEN TEXTCOLOR (LightRed) ELSE TEXTCOLOR (White);
      WRITE ('[',i+1:1,'] :  [',C[Cin].Split[i].COut+1:2,']')   ;
      GOTOXY (46,17+i)                           ;
      WRITE  ('[',C[Cin].Split[i].Start:3 ,']-') ;
      WRITE  ('[',C[Cin].Split[i].Stop:3  ,']')  ;
      GOTOXY (61,17+i)                           ;
      S := GetPitchVel (C[Cin].Split[i].Pitch)   ;
      WRITE  ('[',S:6 ,'] ')                     ;
      S := GetPitchVel (C[Cin].Split[i].Vel)     ;
      WRITE  ('[',S:6   ,']')                    ;
    END                                          ;
 END                                       ;
 {------------- MAIN ----------------------}
 BEGIN
   WriteBottom (2)                          ;
   DoWindow (Blue, LightCyan, 1, 2,80,23,1,0) ;

  Note := 0  ;
  xy   := 4  ;
  SNum := 0  ;
  InitBE     ;

  WRITE (StrL(79,0),NL,K2 ,K4 ,K5, K4, K4, K4, K4, K10,K13,K16,K19,K22 ) ;
  GotoColorStr (White,2,4,B5 )     ;
  GotoColorStr (Cyan ,4,2,B6 )     ;

  FOR i := 2 TO 74 DO
  BEGIN
    TEXTCOLOR (Cyan)                         ;
    GOTOXY (3+i,2)                           ;
    IF B4[i]='C' THEN WRITE (B7)             ;
  END                                        ;
  GotoColorStr (White,4,4,B4)                ;
  REPEAT
    {-----------------------------}
    { Update                      }
    {-----------------------------}
    C[Cin].SplitX := 0                 ;
    FOR i:= 0 TO 3 DO
    IF (C[CIn].Split[i].Start<C[Cin].Split[i].Stop) THEN INC (C[CIn].SplitX) ;
    WriteSplit        ;
    WriteSplitPos     ;

    TEXTCOLOR (White)                    ;
    GOTOXY (23,16)   ; WRITE ( Cin+1:2 ) ;
    GOTOXY (xy, 4)   ;

    Z := READKEY     ;
    X := UPCASE (Z)  ;

    CASE X OF #00: BEGIN
                     Y := READKEY   ;     { 2 code for special keys }
                     CASE Y OF  #75 : { Left Arrow  } xy := xy - 1   ;
                                #77 : { Right Arrow } xy := xy + 1   ;
                      END           ; { Case Y                       }
                      IF xy <  4  THEN xy := 78  ;
                      IF xy >  78 THEN xy := 4   ;
                   END   ; { Case X = #0          }

              '1': SNum := 0                                ;
              '2': SNum := 1                                ;
              '3': SNum := 2                                ;
              '4': SNum := 3                                ;

              'B': BEGIN   { Beginning      }
                     IF Z='B' THEN B[SNum].FN:=FALSE ELSE B[SNum].FN:=TRUE ;
                     IF Z='b' THEN C[CIn].Split[SNum].Start := NArr[xy-4]
                              ELSE C[CIn].Split[SNum].Start := FArr[xy-4]  ;
                     B[SNum].Pos := xy-3                                   ;
                     IF FArr[xy-4] = NArr[xy-4] THEN B[SNum].FN := TRUE    ;
                   END                      ;
              'E': BEGIN   { End            }
                     IF Z='E' THEN E[SNum].FN:=FALSE ELSE E[SNum].FN:=TRUE ;
                     IF Z='e' THEN C[CIn].Split[SNum].Stop := NArr[xy-4]
                              ELSE C[CIn].Split[SNum].Stop := FArr[xy-4] ;
                     E[SNum].Pos := xy-3                                 ;
                     IF FArr[xy-4] = NArr[xy-4] THEN E[SNum].FN := TRUE  ;
                   END                      ;
              'I': DoCInCout ( CIn)         ;
              'O': BEGIN   { Channel Out                         }
                     REPEAT
                       WriteL (B1)                               ;
                       N := GetNumber (0,22,19)                  ;
                       DEC (N)                                   ;
                     UNTIL N IN [0..15]                          ;
                     C[Cin].Split[SNum].COut := N                ;
                     IF C[CIn].Ch[N] = 0 THEN INC(C[CIn].ChX)    ;
                     C[Cin].Ch[N] := 1                           ;
                   END                                           ;
          'P','V': BEGIN   { Pitch  + Velocity                   }
                     REPEAT
                       IF X='P' THEN WriteL (B2)                 ;
                       IF X='V' THEN WriteL (B3)                 ;
                       N := GetNumber (0,22,19)                  ;
                     UNTIL (ABS(N) IN [0..255])                  ;
                     IF X='P' THEN C[Cin].Split[SNum].Pitch := N ;
                     IF X='V' THEN C[Cin].Split[SNum].Vel   := N ;
                   END                                ;
    END                                               ; { Case X }
  UNTIL (X='Q') OR (X=#27)                            ;
  UpDate := TRUE                                                           ;
 END                                                                       ;
{----------------------------------------}
{ Get Filename for CFG File              }
{----------------------------------------}
PROCEDURE GetCfgFilename (Option:BYTE)   ;
CONST X : Str15 = 'Enter filename '      ;
BEGIN
  WriteBottom (3)                           ;
  IF Option=1 THEN WRITE ( X, 'for save :') ;
  IF Option=2 THEN WRITE ( X, 'to load  :') ;
  READLN (FStr)                             ;
END                                         ;
{-------------------------}
{ Update Matrix           }
{-------------------------}
PROCEDURE UpdateMatrix ( PosX, PosY : ArrB16 )   ;
CONST B2 = #08#08                                ; { 2 BS      }
      NC : ARRAY [0..15] OF Str2 = ('O1','O2','O3','O4','O5','O6','O7',
                          'O8','O9','1O','11','12','13','14','15','16') ;
      X  : ARRAY [0..15,0..2] OF BYTE =
      (( 7, 1, 1),(15, 1, 1),(0, 1, 1),(14, 1, 1),  { 7=LightGray TextColor }
       ( 7, 4, 4),(15, 4, 4),(0, 4, 4),(14, 4, 4),  { 4=Red Background      }
       ( 7, 2, 2),(15, 2, 2),(0, 2, 2),(14, 2, 2),  { 2=Green Background    }
       ( 7, 2, 4),(15, 2, 4),(0, 2, 4),(14, 2, 4)) ;{ Red+Green Background  }
       Z : ARRAY [0..3] OF INTEGER = ( Cyan, White, LightMagenta, LightCyan) ;

VAR   i, n, k, m : INTEGER                                                 ;
      Y          : BYTE ;
BEGIN
    TEXTATTR := BlueLightCyan                                    ;
    GOTOXY (47, 2) ; WRITE (PC[C[0].PortIn])                     ;
    GOTOXY (63, 2) ; WritePortOut (C[0].SysPortOut,'')           ;

    FOR i := 0 TO 15 DO     { i = Y : Input Channels  }
    BEGIN
      FOR n := 0 TO 15 DO   { n = X : Output Channels }
      BEGIN
        GOTOXY ( PosX [n], PosY [i] )                                    ;

        IF (C[i].SplitX=0) THEN
        BEGIN
          Y := BYTE ( BOOLEAN (C[i].Ch[n]>0) )                             ;
          Y := Y + 2*BYTE
              ( BOOLEAN ( (C[i].ChMess.A0[n]>0) OR (C[i].ChMess.B0[n]>0) OR
                          (C[i].ChMess.C0[n]>0) OR (C[i].ChMess.D0[n]>0) OR
                          (C[i].ChMess.E0[n]>0) ) )                        ;
          Y := Y +  4*BYTE ( BOOLEAN (C[i].Pitch[n]<>0))                   ;
          Y := Y +  8*BYTE ( BOOLEAN (C[i].Vel[n]<>0))                     ;

          TEXTATTR := X[Y,0] + 16 * (X[Y,1]) ;
          WRITE (NC[n][1])                   ;
          TEXTATTR := X[Y,0] + 16 * (X[Y,2]) ;
          WRITE (NC[n][2])                   ;
        END ELSE
        BEGIN
          m := 0                                                   ;
          FOR k := 0 TO 3 DO
          IF (C[i].Split[k].COut=n) AND (C[i].Split[k].Stop>0) THEN m := 1 ;
          Y := BYTE (C[i].Ch[n]>0)                                 ;
          Y := Y + 2* (BYTE (m=1))                                 ;
          TEXTATTR := Z[Y] + 16*Blue                               ;
          WRITE (NC[n])                                            ;
        END                                                        ;
        TEXTATTR := BlueBlack                                      ;
        IF (C[i].Ch[n]>0) THEN TEXTATTR := BlueLightRed            ;
        WRITE (PortLetter(C[i].PortOut[n]))                        ;
      END                                ;  { n Loop               }

      GOTOXY (61, PosY [i])                                        ;
      TEXTATTR := BlueLightRed                                     ;
      IF C[i].SplitX>0 THEN WRITE ('[', C[i].SplitX,']')
                       ELSE WRITE ('[.] ')                         ;
    END                                  ;  { i Loop          }
END                                      ;
 {---------------------------------------}
 { Write Configuration Panel             }
 {---------------------------------------}
 PROCEDURE WriteMainPanel                ;
CONST
S1='     C H A N N E L  O U T    '       ;
S6=' PORT: Input[..]   SysOutput[....]'  ;
S2='O1'+L2+'O2'+L2+'O3'+L2+'O4'+L2+'O5'+L2+'O6'+L2+'O7'+L2+'O8'+L2
   +'O9'+L2+'1O'+L2+'11'+L2+'12'+L2+'13'+L2+'14'+L2+'15'+L2+'16'       ;
S3='O1 O2 03 O4 O5 O6 O7 O8  O9 1O 11 12 13 14 15 16   KeySplit'       ;
S4='C'+L1+'H'+L1+'A'+L1+'N'+L1+'N'+L1+'E'+L1+'L'+L1+' '+L1+'I'+L1+'N'  ;

VAR i : INTEGER                           ;
BEGIN
  DoWindow (Cyan,Black,  1,2,80,24,1,0)     ;
  DoWindow (Blue,Yellow, 5,3,77,22,1,1)     ;
  DoWindow (Blue,Yellow, 5,3,78,22,0,0)     ;
  GOTOXY (1,1) ; WRITE (StrL(73,0))       ;
  GOTOXY (1,20); WRITE (StrL(73,1))       ;
  FOR i := 2 TO 19 DO BEGIN
                        GOTOXY (1,i) ; WRITE ('³') ;
                        GOTOXY (73,i); WRITE ('³') ;
                      END                          ;
  GotoColorStr (White,6,4,S2)                      ;
  GOTOXY (10, 3)       ; WRITE (S3)                ;
  GotoColorStr (Yellow,3,4,S4)                     ;
  GOTOXY (5,  2)       ; WRITE (S1)                ;
  TEXTATTR := BlueLightGray; WRITE (S6)            ;
 END                                               ;
{----------------------------------------}
{ Edit Matrix                            }
{----------------------------------------}
PROCEDURE EditMatrix                     ;

CONST
    PosX: ArrB16 = (10,13,16,19,22,25,28,31, 35,38,41,44,47,50,53,56) ;
    PosY: ArrB16 = (04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19 ) ;

VAR X, Y   : CHAR                        ;
    n, i   : INTEGER                     ;
    PosXY  : ARRAY [0..256,0..1] OF BYTE ; { 256 Hex Area Pos XY }
    xy     : INTEGER                     ;
    S      : Str10                       ;
{--------------------------------------------------------------}
PROCEDURE DoVelPitch (VAR PVX: WORD; VAR PV: INTEGER; X:BYTE ) ;
CONST S = 'Enable Output Channel first...'        ;
VAR n : INTEGER                                   ;

BEGIN
   WriteBottom (3)                                ;
   IF C[CIn].Ch[COut] > 0 THEN
   BEGIN
     n := GetNumber ( 1, X, 1 )                   ;
     IF (n=0) AND (PVX>0) THEN DEC (PVX)          ;
     IF (n>0) AND (PV=0)  THEN INC (PVX)          ;
     PV := n                                      ;
   END ELSE BEGIN WRITE (S)       ;
                  DELAY (1000)    ;
             END                  ;
   UpDate := TRUE                                 ;
END                                               ;
{------------ MAIN --------------------------------}
BEGIN
  UpDate := TRUE                    ; { Update Panel }
  {-------------------------------------}
  { Load Position (XY) Matrix           }
  {-------------------------------------}
  FOR i := 0 TO 15 DO FOR n := 0 TO 15 DO
                      BEGIN
                        PosXY [i*16+n,0] := PosX [n] ;
                        PosXY [i*16+n,1] := PosY [i] ;
                      END                            ;
  xy := 0                                            ;
  {--------------------------------------------------}

  REPEAT
    IF UpDate THEN BEGIN
                     WriteMainPanel                     ;
                     WriteBottom (0)                    ;
                     DoWindow (Blue,Yellow,5,3,75,21,0,0) ;
                     UpDateMatrix (PosX, PosY)          ;
                   END                                  ;

    GOTOXY ( PosXY [xy,0], PosXY [xy,1] ) ;
    CIn  := xy DIV 16                     ; { Xlate 0..255 to CIn 0-15 }
    COut := xy MOD 16                     ; { COut 0-15 }

    X := UPCASE (READKEY) ;
    Update := FALSE       ;

    CASE X OF #00: BEGIN
                     Y := READKEY   ;     { 2 code for special keys }
                     CASE Y OF  #71 : { Home        } xy := 0        ;
                                #79 : { End         } xy := 255      ;
                                #72 : { Up Arrow    } xy := xy - 16  ;
                                #80 : { Down Arrow  } xy := xy + 16  ;
                                #75 : { Left Arrow  } xy := xy - 1   ;
                                #77 : { Right Arrow } xy := xy + 1   ;
                      END           ; { Case Y                       }
                      IF xy <  0   THEN xy := 0   ;
                      IF xy >= 255 THEN xy := 255 ;
                   END   ; { Case X = #0          }

              #13: BEGIN   { Return : display all parameters        }
                     WriteBottom (3)                                ;
                     TEXTATTR := LightGrayBlack                     ;
                     WRITE ('Ch.Out:[', COut+1, ']' )               ;
                     WRITE (OnOff[C[CIn].Ch[COut]])                 ;

                     IF (C[CIn].SplitX=0) THEN
                     BEGIN
                       S := GetPitchVel ( C[Cin].Pitch[Cout] )        ;
                       WRITE (' Pitch:['   , S, ']' )                 ;
                       S := GetPitchVel ( C[Cin].Vel[Cout] )          ;
                       WRITE (' Velocity:[', S  ,']' )                ;
                     END ELSE WRITE (' Split Keyboard:[',C[CIn].SplitX,']') ;

                     WRITE (' Ch.Mess:[' )                          ;
                     TEXTATTR := LightGrayLightRed                  ;
                     WriteController ( '' )                         ;
                     TEXTATTR := LightGrayBlack                     ;
                     WRITE ('] Num:[', C[Cin].ChMessX, ']' )        ;
                     WRITE (NL,'HW Port Out:[')                     ;
                     WritePortOut (C[CIn].PortOut[COut],'')         ;
                     WRITE ('] System HW Port Out:[')               ;
                     WritePortOut (C[CIn].SysPortOut,' ')           ;
                     WRITE (']   [ENTER] continue...')              ;
                     READLN                                         ;
                   END                                              ;

              'C': BEGIN   { Channel Select                  }
                    C[CIn].Ch[COut] := C[CIn].Ch[COut] XOR 1 ;
                    IF C[CIn].Ch[COut] = 0 THEN
                       DEC(C[CIn].ChX) ELSE INC(C[CIn].ChX)  ;
                   END                                       ;
              'N': GetOutPort (C[CIn].PortOut[COut])         ;
              'S': GetOutPort (C[0].SysPortOut)              ;
              'I': GetInPort (C[0].PortIn)                   ;
              'P': DoVelPitch    { Pitch Transpose           }
                   (C[Cin].PitchX, C[Cin].Pitch[COut], 65 )  ;
              'V': DoVelPitch    { Velocity Transpose        }
                   (C[Cin].VelX,   C[Cin].Vel[COut],   67 )  ;
              'A': DoReadWrite ('R')                         ;
              'O': DoA0ToEF                                  ;
              'K': DoKeyBSplit                               ;
    END            ; { Case X }
    IF X IN ['S','N','I','C',#13] THEN UpDate := TRUE        ;
  UNTIL (X='Q') OR (X=#27)    ;
 END                          ;
{----------------------------------------- MAIN -----------}
BEGIN
    CASE X OF
              'E': BEGIN   { Edit Matrix                     }
                    EditMatrix                               ;
                   END                                       ;
              'C': BEGIN   { Other channel messages    }
                     DoA0ToEF                          ;
                   END                                 ;
              'P': BEGIN   { Other channel messages    }
                     DoPatchXlat                       ;
                   END                                 ;
              'K': BEGIN   { KeyBoard Split            }
                     DoKeyBSplit                       ;
                   END                                 ;
              'S': BEGIN    { Save current Config   }
                     GetCFGFilename (1)             ;
                     CFGSave ( FStr, FSize )        ;
                   END                              ;
              'L': BEGIN    { Load saved Config     }
                     GetCFGFilename (2)             ;
                     CFGLoad (FStr, FSize)          ;
                     UpDate := TRUE                 ;
                   END                              ;
    END                                             ; { Case X }
END                                                 ;
