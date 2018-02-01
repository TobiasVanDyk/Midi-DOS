{-----------------------------------------}
{ Check if File Exists and its size       }
{-----------------------------------------}
FUNCTION CheckFileSize ( NameStr : STRING ) : LONGINT ;
{-----------------------------------------------------}
{ TYPE SearchRec = RECORD                             }
{                     Fill : ARRAY [1..21] OF BYTE ;  }
{                     Attr : BYTE                  ;  }
{                     Time : LONGINT               ;  }
{                     Size : LONGINT               ;  }
{                     Name : STRING[12]            ;  }
{-----------------------------------------------------}
VAR SR : SearchRec                                    ;
BEGIN
  SR.Name := ''                 ;
  FINDFIRST ( NameStr, 0, SR )  ;
  IF (SR.Name<>'') THEN CheckFileSize := SR.Size
                   ELSE CheckFileSize := 0      ;
END                                             ;
{-------------------------------------------------------}
{ Write File Errors                                     }
{-------------------------------------------------------}
PROCEDURE DoFError  (FError: INTEGER ; Name : STRING; Size: LONGINT )  ;

CONST D = ' File Error: '                      ;
VAR E : Str24                                  ;

BEGIN
  E := 'Refer to DOS Manual'                  ;
  CASE FError OF
    0   : E := ' '                            ;
    2   : E := 'File not found'               ;
    3   : E := 'Path not found'               ;
    4   : E := 'To many open files'           ;
    5   : E := 'File access denied'           ;
    6   : E := 'Invalid Handle'               ;
    8   : E := 'Not enough memory'            ;
    10  : E := 'Invalid Environment'          ;
    11  : E := 'Invalid Format'               ;
    18  : E := 'No more files'                ;
    100 : E := 'Disk read error'              ;
    150 : E := 'Write protected disk'         ;
    151 : E := 'Unknown Unit'                 ;
    152 : E := 'Drive not ready'              ;
    255 : E := 'Wrong Filetype'               ;
  END                                         ;
  WRITE (NL, D, Name, ':[', FError,'] ',E )        ;
  IF FError = 255 THEN WRITE (' Size:[',Size,']')  ;
  DELAY (2000)                                     ;
END                                                ;
{------------------------------------------------------------------}
{ Load Buffer Data File                                            }
{------------------------------------------------------------------}
PROCEDURE DTALoad ( NStr: STRING; VAR Size: LONGINT; Pos: WORD   ) ;

VAR F      : FILE                       ;
    FError : INTEGER                    ;
    FStr   : Str10                      ;
    SubSize: LONGINT                    ;

BEGIN
  Size   := CheckFileSize (NStr)                      ;
  IF (Size=0) OR (Size>MaxBuffSize) THEN
  BEGIN
    IF (Size=0) THEN FStr := 'not found.'
                ELSE FStr := 'too big..'              ;
    WRITE (NL,' Warning: File [', NStr, ']', FStr)    ;
    DELAY (2000)                                      ;
    EXIT                                              ;
  END                                                 ;

  ASSIGN  (F, NStr )     ;
{$I-}
  RESET   (F, 1)         ;
  FError := IORESULT     ;
{$I+}
  IF (FError<>0) THEN BEGIN
                        DoFError ( FError, NStr, Size ) ;
                        EXIT                            ;
                      END                               ;

  IF (Size+Pos<=XBuff.Max) THEN
  BEGIN
    WHILE NOT(EOF(F)) DO BLOCKREAD (F, XBuff.P^[Pos], Size ) ;
    CLOSE (F)                                                ;
    EXIT                                                     ;
  END                                                        ;

  SubSize := XBuff.Max - Pos            ;
  BLOCKREAD (F, XBuff.P^[Pos], SubSize) ;
  Size := Size - SubSize                ;
  IF (Size<=0) OR NOT NextBuff THEN BEGIN
                                     CLOSE (F) ;
                                     EXIT      ;
                                   END         ;

  WHILE NOT(EOF(F)) DO BLOCKREAD (F, XBuff.P^[0], XBuff.Max) ;

  Size := Size - XBuff.Max                                   ;

  IF (Size<=0) OR NOT NextBuff THEN BEGIN
                                     CLOSE (F) ;
                                     EXIT      ;
                                   END         ;

  WHILE NOT(EOF(F)) DO BLOCKREAD (F, XBuff.P^[0], XBuff.Max) ;

  CLOSE ( F )                                                ;
END                                                               ;
{-----------------------------------------------------------------}
PROCEDURE DTASave ( NStr : STRING; VAR Size: LONGINT; Pos: WORD ) ;
{-----------------------------------------------------------------}
VAR F        : FILE       ;
    FError   : INTEGER    ;
    AllBuff  : BOOLEAN    ;

BEGIN
  AllBuff := (Size<0)    ;
  ASSIGN  (F, NStr )     ;
{$I-}
  REWRITE (F, 1)         ;
  FError := IORESULT     ;
{$I+}
  IF (FError<>0) THEN DoFError (FError, NStr, Size)
  ELSE BEGIN
         IF NOT AllBuff THEN BLOCKWRITE (F, XBuff.P^[Pos], Size)
         ELSE BEGIN
                BLOCKWRITE (F, XBuff.P^[Pos], XBuff.Max-Pos )      ;
                IF NextBuff THEN
                BLOCKWRITE (F,XBuff.P^[0],XBuff.Pos)               ;
                IF NextBuff THEN
                BLOCKWRITE (F,XBuff.P^[0],XBuff.Pos)               ;
              END                                                  ;
         CLOSE (F)                                                 ;
       END                                                         ;
 END                                                               ;
{------------------------------------------------------------------}
{ Read Prog Key Files                                              }
{------------------------------------------------------------------}
PROCEDURE KEYLoad (Option: BYTE; NStr: Str24; VAR Size: LONGINT ) ;

VAR FError : INTEGER ;
    F      : FILE    ;
    i      : LONGINT ;

BEGIN
  Size   := CheckFileSize (NStr)                                          ;
  IF (Option=2) OR (Option=1) THEN i := Size ELSE i :=  1                 ;
  FError := 0                                                             ;

  CASE Option OF
       0: IF (Size<>SIZEOF(Remote)+1) THEN FError:=255 ;
       1: IF (Size<>SIZEOF(FKey))     THEN FError:=255 ;
       2: IF (Size<>SIZEOF(FLink))    THEN FError:=255 ;
       3: IF (Size<>AllKeySize)       THEN FError:=255 ;
  END                                                  ;

  IF (FError=255) THEN DoFError (255, NStr, Size)
  ELSE BEGIN
         ASSIGN  (F, NStr )     ;
        {$I-}
         RESET   (F, i)         ;
         FError := IORESULT     ;
        {$I+}
         IF (FError<>0) THEN DoFError (FError, NStr, Size)
         ELSE BEGIN
                CASE Option OF
                  0: BEGIN
                       BLOCKREAD (F, Remote, SIZEOF(Remote))   ;
                       BLOCKREAD (F, RemoteMaster, 1)          ;
                     END                                       ;
                  1: BLOCKREAD (F, FKey, 1)  ;
                  2: BLOCKREAD (F, FLink, 1) ;
                  3: BEGIN
                       BLOCKREAD (F, FKey,  SIZEOF(FKey))    ;
                       BLOCKREAD (F, FLink, SIZEOF(FLink))   ;
                       BLOCKREAD (F, Remote, SIZEOF(Remote)) ;
                       BLOCKREAD (F, RemoteMaster, 1)        ;
                     END                                     ;
                END                                          ; { Case }
                CLOSE (F)                                    ;
              END                                            ; { FError=0 }
       END                                                   ; { FError<>255}
END                                                          ;
{--------------------------------------------------------------------}
{ Write Prog Key Files                                               }
{--------------------------------------------------------------------}
PROCEDURE KEYSave  ( Option: BYTE; NStr: Str24; VAR Size: LONGINT ) ;

VAR FError : INTEGER ;
    F      : FILE    ;

BEGIN
  ASSIGN  (F, NStr )     ;
{$I-}
  REWRITE (F, Size)      ;
  FError := IORESULT     ;
{$I+}
  IF (FError<>0) THEN DoFError ( FError, NStr, Size )
  ELSE BEGIN
       CASE Option OF
         0: BEGIN
              BLOCKWRITE (F, Remote, SIZEOF(Remote)) ;
              BLOCKWRITE (F, RemoteMaster, 1)        ;
              Size := SIZEOF (Remote) + 1            ;
            END                                      ;
         1: BLOCKWRITE (F, FKey, 1)                  ;
         2: BLOCKWRITE (F, FLink, 1)                 ;
         3: BEGIN
              BLOCKWRITE (F, FKey,   SIZEOF(FKey))               ;
              BLOCKWRITE (F, FLink,  SIZEOF(FLink))              ;
              BLOCKWRITE (F, Remote, SIZEOF(Remote))             ;
              BLOCKWRITE (F, RemoteMaster, 1)                    ;
              Size := AllKeySize                                 ;
            END                                                  ;
       END                                                       ; { Case }
       CLOSE (F)                                                 ;
   END                                                           ;
END                                                              ;
{-----------------------------------------------------}
{ Save CFG File                                       }
{-----------------------------------------------------}
PROCEDURE CFGSave ( NStr: STRING; VAR Size: LONGINT ) ;
VAR F            : FILE                               ;
    FError       : INTEGER                            ;
    BytesWritten : INTEGER                            ;

BEGIN
  ASSIGN  (F, NStr )                 ;
{$I-}
  REWRITE  (F, SIZEOF(C) )           ;
  FError := IORESULT                 ;
{$I+}
  IF (FError>0) THEN DoFError ( FError, NStr, Size )
      ELSE BEGIN
             InitOptions3                                           ;
             BLOCKWRITE (F, C, 1, BytesWritten)                     ;
             CLOSE (F)                                              ;
             IF (BytesWritten<>1) THEN
             WRITE (NL,' File Error: ', NStr,' [',BytesWritten,']') ;
             Size := SIZEOF (C)                                     ;
           END                                                      ;
END                                                                 ;
{-----------------------------------------------------}
{ Load CFG File                                       }
{-----------------------------------------------------}
PROCEDURE CFGLoad ( NStr: STRING; VAR Size: LONGINT ) ;

VAR F         : FILE                       ;
    FError    : INTEGER                    ;
    BytesRead : INTEGER                    ;

BEGIN
  Size := CheckFileSize (NStr)             ;
  IF (Size=SIZEOF(C)) THEN
  BEGIN
      ASSIGN  (F, NStr )                   ;
     {$I-}
      RESET   (F, SIZEOF(C) )              ;
      FError := IORESULT                   ;
     {$I+}
      IF (FError>0) THEN DoFError ( FError, NStr, Size )
          ELSE BEGIN
                 BLOCKREAD (F, C, 1, BytesRead)                      ;
                 CLOSE (F)                                           ;
                 InitOptions2                                        ;
                 IF (BytesRead<>1) THEN
                 WRITE (NL,' File Error: ', NStr,' [',BytesRead,']') ;
               END                                                   ;
  END
  ELSE DoFError ( 255, NStr, Size )                                  ;
  FEFilter := C[0].FList[9]             ;
  F8Filter := C[0].FList[5]             ;
END                                     ;
