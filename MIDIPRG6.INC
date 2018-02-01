{----------------------------------}
{ Miscellaneous Options            }
{----------------------------------}
PROCEDURE DoOptions (SubOp: CHAR)  ;

VAR   K1, K2  : CHAR     ;
      i       : INTEGER  ;
      N       : WORD     ;
      NStr    : Str10    ;
      FStr    : Str24    ;
      F       : FILE     ;
      FError  : INTEGER  ;
      FPos    : WORD     ;
      FPosStr : Str24    ;
      FEnd    : LONGINT  ;
      FEndStr : Str24    ;
      FSizeStr: Str24    ;
      FSize   : LONGINT  ;
      SizeF   : LONGINT  ;
CONST
      S1 = NL+' Selected Buffer(s) [N,O,S]: Total Size:[' ;
      S2 = NL+' Load Options :'+
           NL+' [ENTER]      Load at position 0 in current Buffer.'+
           NL+' [Num][ENTER] Load at position [Num] in current Buffer.'+
           NL+' (Load continues into any other selected buffers.)'     ;
      S3 = NL+' Save Options :'+
           NL+' From:[ENTER=0][Num=Position] in current Buffer.'+
           NL+' Upto:[ENTER]    All selected Buffers:0-current positions.'+
           NL+'      [P][ENTER] Current Buffer only :0-current position. '+
           NL+'      [E][ENTER] Current Buffer only :0-end of buffer   . ';
{------------------------------}
PROCEDURE ClearL (Y:INTEGER)   ;
BEGIN
  GOTOXY (2,Y)       ;
  WRITE  (Sp23,Sp23) ;
  GOTOXY (2,Y)       ;
END ;
{------------------------------------------------}
PROCEDURE CloseF (B: Byte; S: WORD )             ;
BEGIN
  ClearL (2)                                     ;
  IF B=0 THEN WRITE  ('[',S,'] bytes written.')  ;
  IF B=1 THEN WRITE  ('[',S,'] bytes read.   ')  ;
  DELAY (1000)                                   ;
END ;
{-------------------------------------------------------}
PROCEDURE GetFName (Option:BYTE; VAR FStr: Str24)       ;
{-------------------------------------------------------}
CONST
  C1 = 'Enter FileName to  Load:'                       ;
  C2 = 'Enter FileName for Save:'                       ;
VAR E : INTEGER                                         ;

BEGIN
    E := 0                                  ;
    ClearL (4)                              ;
    IF Option=1 THEN WRITE (C1)             ;
    IF Option=2 THEN WRITE (C2)             ;
    IF Option<3 THEN READLN (FStr)          ;

    IF Option=4 THEN
       BEGIN
         ClearL (3)                        ;
         WRITE (C1)                         ;
         READLN (FStr)                      ;
         REPEAT
           ClearL (4)                                  ;
           WRITE ('Load Position [0-',XBuff.Max,']: ') ;
           READLN (FPosStr)                            ;
           IF FPosStr = '' THEN FPosStr := '0'         ;
           VAL (FPosStr, FPos, E )                     ;
           IF (FPos>XBuff.Max) THEN E:=1               ;
         UNTIL (E=0)                                   ;
       END                                             ;
    IF Option=3 THEN
       BEGIN
         ClearL (2)                                   ;
         WRITE (C2)                                   ;
         READLN (FStr)                                ;

         REPEAT
           ClearL (3)                                                  ;
           WRITE ('Save from:[0-',XBuff.Max,'] : ')                    ;
           READLN (FPosStr)                                            ;
           IF FPosStr = '' THEN FPosStr := '0'                         ;
           VAL (FPosStr, FPos, E )                                     ;
           IF (E=0) THEN IF (FPos > XBuff.Max) THEN E:=1               ;
         UNTIL (E=0)                                                   ;

         REPEAT
           FEnd := 0                                                   ;
           ClearL (4)                                                  ;
           WRITE ('Save upto:[1-',XBuff.Max,'] : ')                    ;
           READLN (FEndStr)                                            ;
           FEndStr[1] := UPCASE(FEndStr[1])                            ;
           IF (FEndStr='')     THEN FEnd := MaxBuffSize * -1           ;
           IF (FEndStr[1]='E') THEN FEnd := XBuff.Max                  ;
           IF (FEndStr[1]='P') THEN FEnd := XBuff.Pos                  ;
           IF (FEnd=0) THEN BEGIN
                              VAL (FEndStr, FEnd, E )                        ;
                              IF (E=0) THEN E := BYTE(FEnd>XBuff.Max)        ;
                            END                                              ;
           FSize := FEnd - FPos  + 1                                         ;
         UNTIL (E=0)                                                         ;
    END                                                                      ;
END                                                                          ;
{----------------------------------------------------------------------------}
{ Read Prog Key Files                                                        }
{----------------------------------------------------------------------------}
PROCEDURE DoFRead (Option: BYTE; Size: LONGINT; FStr: STRING ) ;

BEGIN
  ClearL (4)                              ;
  KEYLoad ( Option, FStr, SizeF )         ;
  IF (SizeF>0) THEN CloseF (1, Size )     ;
END                                       ;
{--------------------------------------------------------------}
{ Write Prog Key Files                                         }
{--------------------------------------------------------------}
PROCEDURE DoFWrite ( Option: BYTE; Size: LONGINT; FStr: STRING ) ;

BEGIN
  ClearL (4)                              ;
  KEYSave  ( Option, FStr, Size  )        ;
  IF (Size>0) THEN CloseF (0, Size )      ;
END                                       ;

{-----------------------------------------}
PROCEDURE DoBRead ;
{-----------------------------------------}
BEGIN
  ClearL ( 4 )                                     ;
  DTALoad ( FStr, SizeF, FPos )                    ;
  IF (SizeF=0) OR ((SizeF+FPos)>MaxBuffSize) THEN
  BEGIN
    WRITE ('Warning: File [',FStr,'] to big..')    ;
    DELAY (2000)                                   ;
  END
  ELSE BEGIN
         CloseF (1, SizeF )                        ;
       END                                         ;
END                                                ;
{--------------------------------------------------}
PROCEDURE DoBWrite ;
{--------------------------------------------------}
BEGIN
  SizeF := FSize                           ;
  ClearL (4)                               ;
  DTASave ( FStr, SizeF, FPos )            ;
  CloseF (0, SizeF )                       ;
END                                        ;
{----------------------------}
PROCEDURE FileList           ;             { Directory List }
{----------------------------}
CONST
  S2 = ' Num Filename.Ext   Size  FileType '    ;
  {    ' ### NNNNNNNN.EEE.SSSSSS.          '  }
  S3 = ' Search pattern [RETURN=Default *.*]: ' ;
VAR
  n          : BYTE      ;
  TotalSize  : LONGINT   ;
  SearchFile : SEARCHREC ;
  FileStr,S1 : Str20     ;
  Path       : PATHSTR   ;
  Dir        : DIRSTR    ;
  Name       : NAMESTR   ;
  Ext        : EXTSTR    ;
{-----------------------------------------}
FUNCTION SizeCheck (Size:LONGINT) : Str20 ;
BEGIN
    S1 := 'Data File'                               ;
    IF Size=SIZEOF(C)             THEN S1 := 'Configuration'    ;
    IF Size=SIZEOF(Remote)        THEN S1 := 'Remote Keys'      ;
    IF Size=SIZEOF(FKey)          THEN S1 := 'BufferLink Keys'  ;
    IF Size=SIZEOF(FLink)         THEN S1 := 'FileLink Keys'    ;
    IF Size=SIZEOF(ConfigType)*4  THEN S1 := 'HW Config.'       ;
    IF Size=AllKeySize            THEN S1 := 'All Prog.Keys'    ;
    SizeCheck := S1                                             ;
END                                                             ;
BEGIN
  DoSubWindow (17)                      ;
  WRITE  (NL, S3 )                      ;
  READLN (FileStr )                     ;
  IF FileStr = '' THEN FileStr := '*.*' ;  { Default }
  CLRSCR                                ;
  TEXTCOLOR (Black)                     ;
  WRITE (S2)                            ;

  DoWindow (LightGray, Blue, 11,7,74,4+17,1,0) ;

  FINDFIRST ( FileStr, 0, SearchFile )       ;  { Find 1st File  }
  FSPLIT ( SearchFile.Name, Dir, Name, Ext ) ;  { Split PathName }

  n         := 1 ;
  TotalSize := 0 ;

  WHILE DOSERROR = 0 DO
  BEGIN
    WRITE (n:4, ' ', Name:8, Ext:4, ' ', SearchFile.Size:6, '  ' ) ;
    TotalSize := TotalSize + SearchFile.Size                       ;
    S1 := SizeCheck (SearchFile.Size)                              ;
    WRITE (S1, NL)                                                 ;
    IF ((n MOD 14)=0) THEN BEGIN
                             Choice := KeyRead(Choice) ;
                             CLRSCR                    ;
                           END                         ;
    FINDNEXT ( SearchFile ) ;
    FSPLIT ( SearchFile.Name, Dir, Name, Ext )      ;  { Split PathName }
    INC (n)                                         ;
  END                                               ;

  TEXTCOLOR (Black)                                    ;
  WRITE (' Size of Files listed:[', TotalSize:8, ']' ) ;
  WRITE (' Disk Free:[', DISKFREE(0):8, '] bytes.')    ;
  Choice := KeyRead(Choice)                            ;
END                                                    ;
{-------------------------- MAIN ----------------------------------------}
BEGIN
    CASE SubOp OF
        'B' : Q.BuffOption       := Q.BuffOption       XOR 1 ;
        'S' : Q.SysExOption      := Q.SysExOption      XOR 1 ;
        'C' : Q.CodeOption       := Q.CodeOption       XOR 1 ;
        'F' : Q.FilterEnable     := Q.FilterEnable     XOR 1 ;
        'P' : Q.InitStrEnable    := Q.InitStrEnable    XOR 1 ;
        'R' : Q.RStatEnable      := Q.RStatEnable      XOR 1 ;
        'A' : Q.AutoClr          := Q.AutoClr          XOR 1 ;
        'T' : Q.TimeEnable       := Q.TimeEnable       XOR 1 ;
        'I' : BEGIN
                GetInPort (C[1].PortIn)                  ;
                OK := PortInOK (C[1].PortIn)             ;
              END                                        ;
        'O' : BEGIN
                GetOutPort(C[1].SysPortOut)              ;
                OK := PortOutOK (C[1].SysPortOut)        ;
              END                                        ;
    END                                                  ;

    IF SubOp IN ['1'..'6','8'] THEN
    BEGIN
      DoSubWindow (15)            ;
      SizeF := GetMaxBuffSize     ;
      ClearL (2)                             ;
      WRITE ( ' ENTER: [S]ave or [L]oad : ') ;
      REPEAT
        GOTOXY (30, 2)                       ;
        K2 := KeyRead ( K2 )                 ;
      UNTIL K2 IN ['S','L',#27]              ;
      IF SubOp = '6' THEN
      BEGIN
        GOTOXY (1,8) ; TEXTCOLOR (Black)       ;
        IF K2='S' THEN WRITE (S1,SizeF,']',S3) ;
        IF K2='L' THEN WRITE (S1,SizeF,']',S2) ;
      END                                      ;
        IF K2=#27 THEN SubOp := ' '            ;
    END                                        ;

    CASE SubOp OF
    '7' : FileList                                              ;
    '8' : IF K2='S' THEN BEGIN          { Save current Config   }
                           GetFName (2, FStr)       ;
                           CFGSave ( FStr, SizeF )  ; END
                    ELSE BEGIN          { Load saved Config     }
                           GetFName (1, FStr)       ;
                           CFGLoad (FStr, SizeF)    ; END       ;

    '6' : IF K2='S' THEN BEGIN GetFName (3, FStr)               ;
                               DoBWrite                         ; END
                    ELSE BEGIN GetFName (4, FStr)               ;
                               DoBRead                          ; END ;
    '5' : IF K2='S' THEN BEGIN GetFName (2, FStr)               ;
                               DoBWrite                         ; END
                    ELSE BEGIN GetFName (1, FStr)               ;
                               DoBRead                          ;
                               NBuff.Size := SizeF+FPos         ; END ;
    '3' : IF K2='S' THEN BEGIN GetFName (2, FStr)               ;
                               DoFWrite (0, 1, FStr)            ; END
                    ELSE BEGIN GetFName (1, FStr)               ;
                               DoFRead  (0, 1, FStr)            ; END ;
    '1' : IF K2='S' THEN BEGIN GetFName (2, FStr)               ;
                               DoFWrite (1, SIZEOF(FKey), FStr) ; END
                    ELSE BEGIN GetFName (1, FStr)               ;
                               DoFRead  (1, SIZEOF(FKey), FStr) ; END ;
    '2' : IF K2='S' THEN BEGIN GetFName (2, FStr)               ;
                               DoFWrite (2, SIZEOF(FLink), FStr); END
                    ELSE BEGIN GetFName (1, FStr)               ;
                               DoFRead  (2, SIZEOF(FLink), FStr); END ;
    '4' : IF K2='S' THEN BEGIN GetFName (2, FStr)               ;
                               DoFWrite (3, 1, FStr)            ; END
                    ELSE BEGIN GetFName (1, FStr)               ;
                               DoFRead  (3, 1, FStr)            ; END ;
    END                            ; { Case }

    IF SubOp IN ['4','2'] THEN FOR i := 1 TO 26 DO
       FSizeArr[i] := CheckFileSize ( FLink[i] ) ;
END                                              ;
