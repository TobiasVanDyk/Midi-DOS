
CONST
    CBuffMax   = 255   {1023}                ; { Temporary ReadWrite Buffer }
    BufferMax  = 65279                       ; { N Buff: 255*256            }

TYPE
     Str1  = STRING [1]        ;
     Str2  = STRING [2]        ;
     Str3  = STRING [3]        ;
     Str4  = STRING [4]        ;
     Str6  = STRING [6]        ;
     Str10 = STRING [10]       ;
     Str12 = STRING [12]       ;
     Str15 = STRING [15]       ;
     Str16 = STRING [16]       ;
     Str20 = STRING [20]       ;
     Str24 = STRING [24]       ;
     Str80 = STRING [80]       ;

     CBuffType   = ARRAY [0..CBuffMax]   OF BYTE ;
     BufferType  = ARRAY [0..60000]      OF BYTE ;

     BufferPtr   = ^BufferType                   ;

     ArrI16   = ARRAY [0..15] OF INTEGER         ;
     ArrINT16 = ARRAY [1..16] OF INTEGER         ;
     ArrB9    = ARRAY [0..9]  OF BYTE            ;
     ArrB16   = ARRAY [0..15] OF BYTE            ;
     ArrB18   = ARRAY [0..17] OF BYTE            ;
     ArrW12   = ARRAY [1..12] OF WORD            ;
     ArrW16   = ARRAY [1..16] OF WORD            ;
     ArrW15   = ARRAY [0..15] OF WORD            ;
     ArrL26   = ARRAY [1..26] OF LONGINT         ;
     TType    = ARRAY [0..11] OF Str16           ;
     SubTType = ARRAY [0..9]  OF Str16           ;
     BPosType = ARRAY [0..7]  OF BYTE            ;
     TPosType = ARRAY [0..11] OF BYTE            ;
     TChrType = ARRAY [0..11] OF CHAR            ;
     StatType = ARRAY [0..5]  OF Str3            ;
     NumType  = ARRAY [0..253] OF CHAR           ;
     {-------------------------------------------}
      QType = RECORD
              CASE BYTE OF
              0:( FillChar       ,
                  BlankChar      ,
                  SysExOption    ,        { Sys Ex Detail                   }
                  BuffOption     ,        { Buffer Storage flags            }
                  CodeOption     ,        { Display Codes                   }
                  FilterEnable   ,        { Flag Active-Run to use Filter   }
                  InitStrEnable  ,        { Flag use initial strings MidiStr}
                  RStatEnable    ,        { Running Status                  }
                  AutoClr        ,        { Clear Buffer before Run         }
                  TimeEnable     : BYTE); { Midi or Normal Storage Format   }
              1:( ClearQ         : ARRAY [0..9] OF BYTE )
              END                                         ;

     BuffType = RECORD  { Size 16   }
                  P     : BufferPtr ; { 00..04 }
                  Max   : WORD      ; { 05..06 }
                  Start : WORD      ; { 07..08 }
                  Stop  : WORD      ; { 09..10 }
                  Size  : WORD      ; { 11..12 }
                  Pos   : WORD      ; { 13..14 }
                  Num   : WORD      ; { 15..16 }
                END                 ;
     {------------------------------------------------------------}
     { 8250 UART Ports                                            }
     {------------------------------------------------------------}
     PortType = RECORD
     SERXtal        : BYTE     ; { Xtal Frequency                } {0    }
     SERPort        : INTEGER  ; { COM1-COM4 $2E8-$2F8-$3E8-$3F8 } {1 ,2 }
     SERNum         : BYTE     ; { 1..4                          } {3    }
     CASE BYTE OF
     0:(SERData     : INTEGER  ; { $3x8 Transmit + Receive Data  } {4 ,5 }
        SERIntE     : INTEGER) ; { $3x9 Interrupt Enable         } {6 ,7 }
     1:(SERBaudLo   : INTEGER  ; { $3x8 Baud Rate Lo divisor     } {4 ,5 }
        SERBaudHi   : INTEGER  ; { $3x9 Baud Rate Hi divisor     } {6 ,7 }
        SERIntID    : INTEGER  ; { $3xA Interrupt ID             } {8 ,9 }
        SERLineCtrl : INTEGER  ; { $3xB Line Control             } {10,11}
        SERModemCtrl: INTEGER  ; { $3xC Modem Control            } {12,13}
        SERLineStat : INTEGER  ; { $3xD Line Status              } {14,15}
        SERModemStat: INTEGER  ; { $3xE Modem Status             } {16,17}
        SERTempPad  : INTEGER  ; { $3xF Scratch Pad RW           } {18,19}
        SERDetect   : BOOLEAN  ; { Port Detected                 } {20   }
        SEREnable   : BOOLEAN  ; { Port may be used              } {21   }
     IRQ            : BYTE     ; { IRQ3,4,5,7 in here            } {22   }
     INTNumber      : INTEGER  ; { INT $0B = hardware IRQ03(COM2)} {23,24}
     SERUartType    : INTEGER ); { UART 8250A,8250B,16450,16550  } {25,26}
     2:( ClearPort  : ARRAY [0..7] OF INTEGER                    )
     END                       ;
     {------------------------------------------------------------}
     StartEnd = RECORD           { Size : 48 }
                  Start : ArrW12 ;
                  Stop  : ArrW12 ;
                END              ;
     {-----------------------------------------}
     { Record for file storage of HW CFG       }
     {-----------------------------------------}
     ConFigType = RECORD      { Size 7 bytes   }
                    IRQ       : BYTE    ;
                    INTNumber : BYTE    ;
                    SERNum    : BYTE    ;
                    SERPort   : INTEGER ;
                    XTL       : BYTE    ;
                    SEREnable : BOOLEAN ;
                  END                   ;
    {-------------------------------------------}
    { Record for Remote File Linked Keyboard    }
    {-------------------------------------------}
    RemoteType = RECORD
                   KEY  : BYTE  ;
                   CFG  : Str24 ;
                   PROG : Str24 ;
                   DTA  : Str24 ;
                 END            ;
    {-------------------------------------------}
    { Record for File Linked Keys               }
    {-------------------------------------------}
    FLinkType = ARRAY [1..26] OF Str24          ;
    {-------------------------------------------}
    { Record for F1-F12 Programmable Keys       }
    {-------------------------------------------}
    FKeyType = RECORD
                 N : StartEnd ;  { Normal  }
                 S : StartEnd ;  { Shift   }
                 C : StartEnd ;  { Control }
                 A : StartEnd ;  { Alt     }
               END            ;
    {-------------------------------------------------------}
    { Keyboard Split Record Types                           }
    {-------------------------------------------------------}
    SplitType = RECORD              { Size 7 bytes          }
                   COut   : BYTE    ;
                   Start  : BYTE    ;
                   Stop   : BYTE    ;
                   Pitch  : INTEGER ;
                   Vel    : INTEGER ;
                 END                ;

     POutType = RECORD A,B,C,D : BYTE ; END  ; { Output Ports A, B, C, D }

     EchoType = RECORD
                  Timer : WORD ;
                  COut  : BYTE ;
                  Enable: BYTE ;
                  Rep   : BYTE ;
                  Vel   : BYTE ;
                END            ;
     {------------------------------------------------------------------}
     { Midichannel Record                                               }
     {------------------------------------------------------------------}
     CType=RECORD                             { Size = 274 bytes  }
           CASE BYTE OF
           0:( ChX        : WORD                  ; {0,1}
               PitchX     : WORD                  ; {2,3}
               VelX       : WORD                  ; {4,5}
               Ch         : ArrI16                ; {6 ,37 }
               Pitch      : ArrI16                ; {38,69 }
               Vel        : ArrI16                ; {70,101}
               MidiStrX   : WORD                  ; {102,103}
               MidiStr    : ArrB18        ;         {104,121}
               FList      : ArrB18        ;         {122,139}
               ChMessX    : WORD          ;         {140,141}
               ChMess     : RECORD                  {142,221}
                              A0 : ArrB16 ;         {142,157}
                              B0 : ArrB16 ;         {158,173}
                              C0 : ArrB16 ;         {174,189}
                              D0 : ArrB16 ;         {190,205}
                              E0 : ArrB16 ;         {206,221}
                            END           ;
               ChMessB0   : RECORD                  {222,243}
                              COut1      : BYTE   ; {222} { 255 = not active }
                              COut2      : BYTE   ; {223}
                              ContrList1 : ArrB9  ; {224,233}
                              ContrList2 : ArrB9  ; {234,243}
                            END                       ;
               SplitX     : WORD                      ; {244,245}
               Split      : ARRAY [0..3] OF SplitType ; {246,273}
               PatchCOut  : ArrB16                    ; {274,289}
               PatchXlat  : ARRAY [0..127] OF BYTE    ; {290,417}
               PortOut    : ARRAY [0..15] OF POutType ; {418,481}
               SysPortOut : POutType                  ; {482,485}
               PortIn     : WORD                      ; {486,487}
               Echo       : EchoType                ) ; {488,493}
           1:( ClearCFGRec: ARRAY [0..487] OF BYTE  )
           END                                        ;
             {---------------------------------------------------------}
             { PortOut used for KeySplit as well                       }
             { C[0].SysPortOut: Ch independant messages: INT procedures}
             { C[1].SysPortOut: Polling Mode Procedures                }
             { C[0].PortIn: INT procedures                             }
             { C[1].PortIn: Polling Mode Procedures                    }
             { C[2..15].PortIn: Store Options Variables                }
             {---------------------------------------------------------}
CONST
  NumStr : Str4 = '    '                                                    ;
  NumArr : NumType =
  '  O  1  2  3  4  5  6  7  8  9 1O 11 12 13 14 15 16 17 18 19 2O 21 22 23'+
  ' 24 25 26 27 28 29 3O 31 32 33 34 35 36 37 38 39 4O 41 42 43 44 45 46 47'+
  ' 48 49 5O 51 52 53 54 55 56 57 58 59 6O 61 62 63 64 65 66 67 68 69 7O 71'+
  ' 72 73 74 75 76 77 78 79 8O 81 82 83 8'                                  ;
  Num1Arr: NumType =
  '4 85 86 87 88 89 9O 91 92 93 94 95 96 97 98 991OO1O11O21O31O41O51O61O71O'+
  '81O911O11111211311411511611711811912O12112212312412512612712812913O13113'+
  '213313413513613713813914O14114214314414514614714814915O15115215315415515'+
  '615715815916O1611621631641651661671681'                                  ;
  Num2Arr: NumType =
  '6917O17117217317417517617717817918O18118218318418518618718818919O1911921'+
  '931941951961971981992OO2O12O22O32O42O52O62O72O82O921O2112122132142152162'+
  '1721821922O22122222322422522622722822923O23123223323423523623723823924O2'+
  '4124224324424524624724824925O251252253'                                  ;
  Num3Arr: ARRAY [0..5] OF CHAR = '254255'                                  ;

       OnOff : ARRAY [0..1] OF Str3 = ('Off',' On')         ;
       OnOff1: ARRAY [0..1] OF Str3 = ('.'  ,'*'  )         ;
       OnOff2: ARRAY [0..1] OF Str3 = (' No','Yes')         ;
       PC    : ARRAY [0..3] OF Str2 = ('..','A.','.B','AB') ;

       INTArr: ARRAY [0..7] OF BYTE=($08, $09, $0A, $0B, $0C, $0D, $0E, $0F);
       {                             IRQ0 IRQ1 IRQ2 IRQ3 IRQ4 IRQ5 IRQ6 IRQ7}
       SERArr: ARRAY [0..3] OF INTEGER=($3F8, $2F8, $3E8, $2E8);
       {                                COM1  COM2  COM3  COM4}
       HW    : Str6 = 'HW.CFG'                                 ;
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

VAR C     : ARRAY [0..15] OF CType         ; { 16 Midichannel Records   }
    CAddr : ARRAY [0..15] OF WORD          ; { Addresses of C[0]..C[15] }
    FEFilter : BYTE                        ;
    F8Filter : BYTE                        ;
    CFG   : BYTE                           ; { Flag: C[x] from disk     }
    KEY   : BYTE                           ; { Flag: Key Config from disk }
    DTA   : BYTE                           ; { Flag: Data File from disk  }
    FKey  : FKeyType                       ;
    FLink    : FLinkType                   ; { File linked Keys A..Z }
    FSizeArr : ArrL26                      ; { Size of FLink Files   }
    Remote   : ARRAY [1..12] OF RemoteType ; { Remote Control        }
    RemoteFlag      : INTEGER              ; { Remote Busy           }
    RemoteMaster    : BYTE                 ; { Remote Start Key      }
    Write3Addr      : WORD                 ; { Addr of WriteCode1    }
    SwopBuffersAddr : WORD                 ; { Addr of SwopBuffers   }
    P               : POINTER              ; { Used in above Addr's  }
    PortA   , PortB   ,
    PortC   , PortD   ,                      { HW Ports present      }
    PortInA , PortInB ,
    PortOutA, PortOutB,
    PortOutC, PortOutD       : BOOLEAN     ;
    PortNum                  : INTEGER     ; { HW Ports 1-4               }
    SERData                  : WORD        ;
    SERLStat                 : WORD        ;
    SERError                 : BYTE        ;
    SERErrorCount            : WORD        ;
    SERErrorPos              : LONGINT     ;
    SERReadByte              : BYTE        ;
CONST
      Q    : QType =
        ( FillChar  :0; BlankChar   :0; SysExOption  :0; BuffOption :1  ;
          CodeOption:1; FilterEnable:1; InitStrEnable:0; RStatEnable:1) ;
      CIn  : INTEGER = 0 ;  { Global Channel In                         }
      COut : INTEGER = 0 ;  { Global Channel Out                        }

    {----------------- Used by Filter and INT routines --------------------}
    X80E0Filter : ArrB16 = (0,0,0,0,0,0,0,0,3  ,3  ,3  ,3  ,2  ,2  ,3  ,0) ;
    X80E0Pos    : ArrB16 = (0,0,0,0,0,0,0,0,$0B,$0C,$0D,$0E,$0F,$10,$11,0) ;
    {                        F0   F2 F3     F6   F8   FA FB FC   FE  FF    }
    XF0FFFilter : ArrB16 = ($FF,0,3 ,2 ,0,0,1 ,0,1 ,0,1 ,1 ,1 ,0,1 ,1  )   ;
    XF0FFPos    : ArrB16 = (00 ,0,01,02,0,0,03,0,05,0,06,07,08,0,09,$0A)   ;
    {                       0 1 2 3 4 5 6 7 8 9 A B C D E F                }
    INTXlat     : ArrB16 = (0,0,0,0,0,0,0,0,1,1,1,1,2,2,1,0)               ;

    CSize      = SIZEOF(CType)*16                                          ;
    HWSize     = SIZEOF(ConFigType)*4                                      ;
    AllKeySize = SIZEOF(FKeyType)+SIZEOF(FLinkType)+12*SIZEOF(RemoteType)+1;

    CFList = 122                   ; { Change this to OFS(FList) in C[x] }

     Sp  = ' '      ;
     Sp4 = '    '   ;
     Sp6 = '      ' ;
     Sp23 = '                       ' ;

     BlueWhiteBlink    = White     + 16*Blue + BLINK ;
     BlueLightRedBlink = LightRed  + 16*Blue + BLINK ;
     BlueYellow        = Yellow    + 16*Blue         ;
     BlueWhite         = White     + 16*Blue         ;
     LightGrayBlue     = Blue      + 16*LightGray    ;
     BlueLightGray     = LightGray + 16*Blue         ;
     GreenRed          = Red       + 16*Green        ;
     BlueRed           = Red       + 16*Blue         ;
     BlueLightRed      = LightRed  + 16*Blue         ;
     LightGrayBlack    = Black     + 16*LightGray    ;
     CyanGreen         = Green     + 16*Cyan         ;
     BlueLightCyan     = LightCyan + 16*Blue         ;
     BlueBlack         = Black     + 16*Blue         ;
     CyanBlack         = Black     + 16*Cyan         ;
     CyanWhite         = White     + 16*Cyan         ;
     BlackYellow       = Yellow    + 16*Black        ;
     LightGrayLightRed = LightRed  + 16*LightGray    ;


     NL  = #13#10      ; { LF + CR   }
     BS  = #08         ; { BS        }
     BS8 = BS+BS+BS+BS+BS+BS+BS+BS+BS ;
     LF0 = #10         ; { LF        }
     LF1 = #10#8       ; { LF + 1.BS }
     LF2 = #10#8#8     ; { LF + 2.BS }
     LF3 = #10#8#8#8   ; { LF + 3.BS }

     ArR = #77         ; { Right Arrow }
     ArL = #75         ; { Left Arrow  }
     ArD = #80         ; { Down Arrow  }
     ArU = #72         ; { Up Arrow    }

     HexDigit  : ARRAY [0..15] OF CHAR = 'O123456789ABCDEF'; { Hex Digits }
     {--------------------------------------------------------------------}

     ManufacturerName: ARRAY [0..14] OF Str16 =
    ( 'SEQUENTIAL SCI','BIG BRIAR','OCTAVE','MOOG','PASSPORT DESIGNS',
      'LEXICON','ENSONIQ','OBERHEIM','BONTEMPI','SIEL','KAWAI','ROLAND',
      'KORG','YAMAHA','CASIO') ;
     ManufacturerNumber: ARRAY [0..14] OF BYTE =
    ( 1,2,3,4,5,6,15,16,32,33,64,65,66,67,68) ;

     SysEx    = $F0       ;   { System Exclusive Start }
     EndSysEx = $F7       ;   { System Exclusive End   }
     NoteOn   = $90       ;   { NoteOn $90..$9F        }
     NoteOff  = $80       ;   { NoteOn $80..$8F        }

     {-----------------------------------------------------}
     { INT Controller Ports (8259)                         }
     {-----------------------------------------------------}
     PICCtrl : INTEGER = $20 ; { Control Port              }
     PICMask : INTEGER = $21 ; { Interrupt Mask Port       }

     BuffChr1: ARRAY [0..3] OF CHAR = 'UNOS'               ; { CurrentBuff }
     BuffChr2: ARRAY [0..7] OF BYTE = (1,2,0,0,0,3,0,0)    ;
     {         Buff Char - 78       :  N O p q r S t U     }
VAR
     Xtal      : BYTE ; { Xtal 1=5, 2=10, 3=15, 4=20 MHz in here   }
     {--------------------------------------------------------}
     { 8250 UART Ports                                        }
     {--------------------------------------------------------}
     P1,P2,P3,P4 : PortType ; { Hardware Ports A,B,C,D        }

      OldINTVector     : POINTER ; { Stores old INT vector in here        }
      OldTimerINT      : POINTER ; { Store old Timer INT vector           }
      OK               : BOOLEAN ;

      ComSize, ComCode , ComCount,
      ComSysEx,ComClass, ComStatus : BYTE ; { INT Handler }

      MidiCh           : BYTE    ;  { MidiChannel 0..15 }
      Choice           : CHAR    ;
      Filter           : BYTE    ;  { Flag INTHandler that filtering is on }
      PrevCmd          : BYTE    ;  { Prev Cmd                     }
      WriteByte        : BYTE    ;  { Midi byte to send            }
      WDummy           : BYTE    ;  { Dummy if WORD WriteByte load }
      ReadByte         : BYTE    ;  { Midi byte read               }
      RDummy           : BYTE    ;  { Dummy if ReadByte WORD read  }
      NewB             : ARRAY [0..16*4] OF BYTE ;{ Space for 16 Note seq. }
      Count            : BYTE                    ;{ Used in Active         }
      MCh              : BYTE                    ;
      MChDummy         : BYTE                    ;
      CmdB             : BYTE                    ;
      Cx,Cc,Px,Pp,Vx,Vv: INTEGER                 ;
      CurCAddr         : WORD                    ;
      Data1, Data2     : BYTE                    ;
      n, i             : INTEGER                 ;{ NewB[x] Counter        }
      WriteCount       : LONGINT                 ;{ #Bytes Transmitted     }
      ReadCount        : LONGINT                 ;{ #Bytes Received        }
      SysExBusy        : BYTE                    ;{ Busy with SysExBytes   }
      SysBegin, SysEnd : LONGINT                 ;
      SysSize          : LONGINT                 ;

      OverFlow    : BYTE                  ; { Overflow Flag          }
      Dummy3      : BYTE                  ;
      Memory      : LONGINT               ;
      {-----------------------------------}
      { Timer Variables                   }
      {-----------------------------------}
      TimerCnt    : LONGINT               ;
      EventCnt    : WORD                  ;
      Time        : LONGINT               ;
      TimerDiv    : WORD                  ;
      TimeOn      : BOOLEAN               ;
CONST Hz192       = 6215                  ; { DIV $1847           }
      Hz96        = 12429                 ; { DIV $308D           }
      HzNormal    = 65535                 ; { DIV $FFFF = 18.2 Hz }
      BPM90       = 8286                  ; { DIV $205D           }
      Timer0      = $40                   ; { 8253 Timer Port 0   }
      Timer2      = $42                   ; { 8253 Timer Port 2   }
      TimerCtrl   = $43                   ; { 8253 Timer Control  }
VAR
      EchoTimerVal: ArrW15                ; { Set = C.Echo.Timer        }
      EchoTimerCnt: ArrW15                ; { Down Counter : Timer -> 0 }
      EchoNote    : ArrB16                ; { Current Echo Note 16 Max  }
      EchoCh      : ArrB16                ; { Set = C.Echo.COut         }
      EchoVel     : ArrB16                ; { Decreasing velocity       }

      CBuff      : CBuffType                ; { Circular Xmit-Receive Buffer }
      CBuffPutLo : BYTE                     ; { Storage pointer in CBuff     }
      CBuffPutHi : BYTE                     ;
      CBuffPut   : WORD ABSOLUTE CBuffPutLo ;
      CBuffGetLo : BYTE                     ; { Retrieval pointer in CBuff   }
      CBuffGetHi : BYTE                     ;
      CBuffGet   : WORD ABSOLUTE CBuffGetLo ;

      BuffN,BuffO,BuffS        : BOOLEAN ; { Select Buffer for edit,save    }
      NBuff,OBuff,SBuff,XBuff  : BuffType; { Normal, Overflow, Select Buffer}
      BuffStartStr,BuffEndStr,BuffSizeStr: Str10                ;
      CurrentBuff  : BYTE                ; { Current Buffer 0=none, 1-3 }
      StartBuff    : BYTE                ; { First Buffer used          }
      MaxBuffSize  : LONGINT             ; { Total Size of ON Buffers   }
      BuffSeq      : ARRAY [1..3] OF BYTE; { [1]=1 BuffN [1]=0 NOT BuffN}

      KBrd       : RECORD
                     CASE BYTE OF
                     0:( Ascii : BYTE       ;
                         Scan  : BYTE       ;
                         Class : BYTE       ;
                         FKey  : BYTE       ;
                         UpCh  : CHAR       ;
                         Pos   : BYTE    )  ;
                     1:( Val   : WORD    )  ;
                     2:( Ch    : CHAR    )  ;
                     3:( On    : BOOLEAN )
                   END                      ;

      SearchN     :  ArrI16                 ; { Used by GetNumberStr }
      ReplaceN    :  ArrI16                 ; { Used by GetNumberStr }
      Search      :  RECORD
                       Buff     : BYTE    ;
                       Num      : BYTE    ;
                       Count    : INTEGER ;
                       CurCount : INTEGER ;
                       Pos      : ArrW16  ;
                     END                  ;
