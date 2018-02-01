
{-----------------------------------------}
{ Check if any parameters on commandline  }
{-----------------------------------------}
PROCEDURE CheckParamStr                   ;

VAR E, F, G               : BYTE          ;
    Px, P                 : Str20         ;
    N                     : INTEGER       ;
    Config                : ARRAY [0..3] OF ConfigType    ;
    ConfigF               : FILE                          ;
    FError                : INTEGER                       ;
    i                     : INTEGER                       ;
    Size                  : LONGINT                       ;
{----------------------------------------------------------}
{ COM: Parameter : Serial Port Num                         }
{----------------------------------------------------------}
PROCEDURE CheckCOM ( CStr: Str20; X: CHAR  )               ;
PROCEDURE DoPx     ( VAR P: PortType )                     ;
BEGIN
  CASE CStr[1] OF
       'E': P.SEREnable := TRUE                ;
       'D': P.SEREnable := FALSE               ;
  '1'..'4': BEGIN
              P.SERNum := ORD(CStr[1]) - 48    ;
              P.SERPort := SERArr [P.SERNum-1] ;
            END                                ;
   ELSE E := 1                                 ;
  END                                          ;
END                                            ;
BEGIN
  CASE X OF
  'A':  DoPx (P1)  ;
  'B':  DoPx (P2)  ;
  'C':  DoPx (P3)  ;
  'D':  DoPx (P4)  ;
  ELSE E := 1      ;
  END              ;
END                ;
{------------------------------------}
{ XTL: Parameter : Xtal Frequency    }
{------------------------------------}
PROCEDURE CheckXTL ( CStr: STRING )  ;
BEGIN
  N := ORD (CStr[1]) - 48            ;
  IF N IN [1..4] THEN Xtal := N      { 1=DIV 1= 5 MHz, 2=DIV 2 = 10 MHz }
  ELSE G := 1                        ;
END                                  ;
{-----------------------------------------------}
{ IRQ: Parameter : INT Request                  }
{-----------------------------------------------}
PROCEDURE CheckIRQ ( CStr: Str20; X: CHAR  )    ;
PROCEDURE DoPx (VAR S1:BYTE; VAR S2: INTEGER)   ;
BEGIN
  N := ORD(CStr[1])-48                          ;
  IF N IN [3..5,7] THEN
  BEGIN
    S1 := N             ;
    S2 := INTArr [N]    ;
  END ELSE F := 1       ;
END                     ;
BEGIN
  CASE X OF
  'A':  DoPx (P1.IRQ,P1.INTNumber)  ;
  'B':  DoPx (P2.IRQ,P2.INTNumber)  ;
  'C':  DoPx (P3.IRQ,P3.INTNumber)  ;
  'D':  DoPx (P4.IRQ,P4.INTNumber)  ;
  ELSE F := 1                       ;
  END                               ;
END                                 ;
{-------------------------------------------------------------}
PROCEDURE WriteLoad ( P : Str20; Size: LONGINT; VAR Z: BYTE ) ;
{-------------------------------------------------------------}
CONST S = NL+' Loaded: '                  ;
BEGIN
  IF (Size>0) THEN
  BEGIN
    WRITE ( S,P:12,' [Size:',Size:5,']')  ;
    Z := 0                                ;
  END                                     ;
END                                       ;
{-----------------------------------------}
{ CFG: Parameter : Filename               }
{-----------------------------------------}
PROCEDURE CheckCFG ( CStr: STRING)        ;
BEGIN
  CFGLoad ( P, Size )                     ;
  WriteLoad ( P, Size, CFG )              ;
END                                       ;
{---------------------------------------}
{ KEY: Parameter : Filename  Prog Keys  }
{---------------------------------------}
PROCEDURE CheckKEY ( CStr: STRING)      ;
BEGIN
  KEYLoad ( 3, P, Size )                ;
  WriteLoad ( P, Size, KEY )            ;
END                                     ;
{---------------------------------------}
{ Buffer Data File                      }
{---------------------------------------}
PROCEDURE CheckDTA  (NStr : STRING)     ;
VAR n   : WORD                          ;
BEGIN
  DTALoad ( NStr, Size, 0 )                                    ;
  IF (Size>0) THEN
  BEGIN
    WriteLoad ( P, Size, DTA )                                 ;
    NBuff.Size := Size                                         ;
    IF (Size<BufferMax) THEN
        FOR n := (Size+1) TO BufferMax DO NBuff.P^[n] := 0      ;
  END                                                          ;
END                                                            ;
{--------------------------------------------}
PROCEDURE CheckStr ( n:INTEGER )             ;
{--------------------------------------------}
VAR i, Len : INTEGER                         ;
BEGIN
  P     := ParamStr(n)                       ;
  Len   := LENGTH (P)                        ;
  FOR i := 1 TO Len DO P[i] := UPCASE(P[i])  ;
  Px    := COPY (P, 1, 3)                    ;
  P     := COPY (P, 6, Len-5)                ;
  CASE Px[1] OF
  'S': CheckCOM (P,P[4])  ;  { SERX: }
  'I': CheckIRQ (P,P[4])  ;  { IRQX: }
  'X': CheckXTL (P)       ;  { XTAL: }
  'C': CheckCFG (P)       ;  { CONF: }
  'P': CheckKEY (P)       ;  { PKEY: }
  'D': CheckDTA (P)       ;  { DATA: }
  END                     ;
END                       ;
{------------------------------------------------------------------------}
PROCEDURE PToConfig ( VAR P: PortType; VAR C: ConfigType; Option: BYTE ) ;
{------------------------------------------------------------------------}
BEGIN
  IF Option=0 THEN
  BEGIN
     P.SEREnable := C.SEREnable ;
     P.SERDetect := TRUE        ;
     P.IRQ       := C.IRQ       ;
     Xtal        := C.XTL       ;
     P.INTNumber := C.INTNumber ;
     P.SERPort   := C.SERPort   ;
     P.SERNum    := C.SERNum    ;
  END
  ELSE
  BEGIN
    C.SEREnable := P.SEREnable ;
    C.IRQ       := P.IRQ       ;
    C.XTL       := Xtal        ;
    C.INTNumber := P.INTNumber ;
    C.SERPort   := P.SERPort   ;
    C.SERNum    := P.SERNum    ;
  END                          ;
END                            ;
{------------------------------------------------------------------------}
BEGIN
  ASSIGN  (ConfigF, HW )             ;
{$I-}
  RESET   (ConfigF, SIZEOF(Config) ) ;
  FError := IORESULT                 ;
{$I+}
  IF (FError<>0) THEN
  BEGIN
    DoFError ( FError, HW, 1 )    ;
    InitialiseHWConfig  (1)       ;
    IF FILEREC(ConFigF).Mode<>FMCLOSED THEN CLOSE (ConfigF) ;
    EXIT                          ;
  END                             ;
  BLOCKREAD (ConfigF, Config, 1)  ;
  CLOSE (ConfigF)                 ;

  PToConfig (P1,Config[0],0)      ;
  PToConfig (P2,Config[1],0)      ;
  PToConfig (P3,Config[2],0)      ;
  PToConfig (P4,Config[3],0)      ;

  E := 0                          ;
  F := 0                          ;
  G := 0                          ;

  CASE PARAMCOUNT OF
       0    : EXIT                                    ; { No Parameters }
       1..20: FOR i := 1 TO PARAMCOUNT DO CheckStr(i) ;
  END                                                 ;

  IF (KEY=0) THEN FOR i := 1 TO 26 DO
      FSizeArr[i] := CheckFileSize ( FLink[i] ) ;

   IF (E+F+G=0) THEN
   BEGIN
     PToConfig (P1,Config[0],1)      ;
     PToConfig (P2,Config[1],1)      ;
     PToConfig (P3,Config[2],1)      ;
     PToConfig (P4,Config[3],1)      ;

     Size := CheckFileSize ( HW )    ;

     IF (Size=SIZEOF(Config)) THEN
     BEGIN
       ASSIGN  (ConfigF, HW )        ;
      {$I-}
       REWRITE (ConfigF, SIZEOF(Config) )  ;
       FError := IORESULT                  ;
      {$I+}
       IF (FError<>0) THEN
       BEGIN
         DoFError ( FError, HW, 1 )        ;
         InitialiseHWConfig  (1)           ;
         CLOSE (ConfigF)                   ;
         EXIT                              ;
       END                                 ;
       BLOCKWRITE (ConfigF, Config, 1)     ;
       CLOSE (ConfigF)                     ;
       WriteLoad ( HW, Size, G )           ;
    END                                    ;
   END
   ELSE  WRITE (NL, ' Error in HW-CFG parameters' ) ;
END                                     ;
{---------------------------------------}
