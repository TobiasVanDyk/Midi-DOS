{---------------------}
PROCEDURE SwopBuffers ;
{---------------------}
BEGIN
     INLINE ( $80/$3E/CurrentBuff/$03    { CMP CurrentBuff, $03 }
             /$74/$6E                    { JE Exit1:            }
     (***********************************************************
      IF CurrentBuff=3 THEN OverFlow  := 1 ;
      ***********************************************************)
     {L1:}
             /$80/$3E/CurrentBuff/$01    { CMP CurrentBuff, $01  }
             /$75/$40                    { JNE L2:               }
             /$80/$3E/BuffO/$00          { CMP BuffO, $00        }
             /$75/$20                    { JNE L3:               }
             /$80/$3E/BuffS/$00          { CMP BuffS, $00        }
             /$74/$59                    { JE Exit1:             }
             /$C6/$06/CurrentBuff/$03    { MOVB CurrentBuff, $03 }
             /$A1/SBuff                  { MOV AX, SBuff[0,1]    } {.P}
             /$A3/XBuff                  { MOV XBuff[0,1], AX    }
             /$A1/SBuff+2                { MOV AX, SBuff[2,3]    }
             /$A3/XBuff+2                { MOV XBuff[2,3], AX    }
             /$A1/SBuff+4                { MOV AX, SBuff[4,5]    } {.Max}
             /$A3/XBuff+4                { MOV XBuff[4,5], AX    }
             /$EB/$45                    { JMP Exit              }
      {L3:}
             /$C6/$06/CurrentBuff/$02    { MOVB CurrentBuff, $02 }
             /$A1/OBuff                  { MOV AX, OBuff[0,1]    } {.P}
             /$A3/XBuff                  { MOV XBuff[0,1], AX    }
             /$A1/OBuff+2                { MOV AX, OBuff[2,3]    }
             /$A3/XBuff+2                { MOV XBuff[2,3], AX    }
             /$A1/OBuff+4                { MOV AX, OBuff[4,5]    } {.Max}
             /$A3/XBuff+4                { MOV XBuff[4,5], AX    }
             /$EB/$2C                    { JMP Exit              }
     {L2:}
             /$80/$3E/CurrentBuff/$02    { CMP CurrentBuff, $02  }
             /$75/$20                    { JNE Exit1:Error       }
             /$80/$3E/BuffS/$00          { CMP BuffS, $00        }
             /$74/$19                    { JE Exit1:             }
             /$C6/$06/CurrentBuff/$03    { MOVB CurrentBuff, $03 }
             /$A1/SBuff                  { MOV AX, SBuff[0,1]    } {.P}
             /$A3/XBuff                  { MOV XBuff[0,1], AX    }
             /$A1/SBuff+2                { MOV AX, SBuff[2,3]    }
             /$A3/XBuff+2                { MOV XBuff[2,3], AX    }
             /$A1/SBuff+4                { MOV AX, SBuff[4,5]    } {.Max}
             /$A3/XBuff+4                { MOV XBuff[4,5], AX    }
             /$EB/$05                    { JMP Exit:             }
     {Exit1:}
             /$C6/$06/OverFlow/$01       { MOVB OverFlow, $01    }
     {Exit:}
             /$31/$C0                    { XOR AX, AX            }
             /$A3/XBuff+12               { MOV XBuff.Pos, AX     }
             /$C3              ) ;       { RET                   }
     (************************************************************
      IF CurrentBuff<3 THEN
      BEGIN CASE CurrentBuff OF
      1: BEGIN IF BuffS THEN BEGIN
                               XBuff := SBuff   ;
                               CurrentBuff := 3 ;
                             END                ;
               IF BuffO THEN BEGIN
                               XBuff := OBuff   ;
                               CurrentBuff := 2 ;
                             END                ;
         END                                    ;
      2: BEGIN IF BuffS THEN BEGIN
                               XBuff := SBuff   ;
                               CurrentBuff := 3 ;
                             END                ;
         END                                    ;
      END                                       ; { Case }
      END                                       ; { CurrentBuff < 3 }
      XBuff.Pos := 0                            ;
      **********************************************************)
END                                     ;
{--------------------}
PROCEDURE WriteCode2 ;
{--------------------}
    BEGIN
      (****************************************************************)
      INLINE ( $FF/$06/WriteCount           { INCW WriteCount LO  }
              /$75/$04                      { JNE L0:             }
              /$FF/$06/WriteCount+2 ) ;     { INCW WriteCount HI  }
        {L0:}
      (*****************************************************************
      IF SysExOption = 1 THEN
      BEGIN
        IF WriteByte = $F0 THEN SysBegin := WriteCount ;
        IF WriteByte = $F7 THEN SysEnd   := WriteCount ;
       END                                          ;
       *****************************************************************)
      INLINE ( $A0/WriteByte                { MOV AL, WriteByte    }
              /$82/$3E/Q+2/$01              { CMPB SysExOption, 1  }
              /$75/$22                      { JNE L8:              }
              /$3C/$F0                      { CMPB AL, $F0         }
              /$74/$12                      { JE L9:               }
              /$3C/$F7                      { CMPB AL, $F7         }
              /$75/$1A                      { JNE L8:              }

              /$A1/WriteCount               { MOV AX, WriteCount LO}
              /$A3/SysEnd                   { MOV SysEnd LO, AX    }
              /$A1/WriteCount+2             { MOV AX, WriteCount HI}
              /$A3/SysEnd+2                 { MOV SysEnd HI, AX    }
              /$EB/$0C                      { JMP L8:              }
        {L9:}
              /$A1/WriteCount               { MOV AX, WriteCount LO}
              /$A3/SysBegin                 { MOV SysBegin LO, AX  }
              /$A1/WriteCount+2             { MOV AX, WriteCount HI}
              /$A3/SysBegin+2     ) ;       { MOV SysBegin HI, AX  }
       {L8:}
      (***********************************************************)
      INLINE ( $A0/WriteByte                { MOV AL, WriteByte   }
              /$82/$3E/Q+7/$00              { CMPB RStatEnable, 0 }
              /$75/$03                      { JNE L2:             }
              /$A2/PrevCmd   ) ;            { MOVB PrevCmd, AL    }
       {L2:}
      (*****************************************************************
      IF BuffOption = 1 THEN
      BEGIN
        Buffer^[BufferPos] := WriteByte                                 ;
        IF BufferPos<BufferMax THEN INC(BufferPos) ELSE BufferPos := 0  ;
      END                                                               ;
      ******************************************************************)
      INLINE ( $82/$3E/Q+3/$01              { CMPB BuffOption, $01      }
              /$75/$1C                      { JNE L3:                   }

              /$1E                          { PUSH DS                   }
              /$8B/$3E/XBuff+12             { MOV DI, XBuff.Pos         }
              /$C5/$1E/XBuff                { LDS BX, XBuff.Ptr         }
              /$88/$01                      { MOV [BX+DI], AL           }
              /$1F                          { POP DS                    }
              /$39/$3E/XBuff+4              { CMP DI, XBuff.Max         }
              /$76/$06                      { JBE L7:                   }
              /$FF/$06/XBuff+12             { INCW BufferPos            }
              /$EB/$04                      { JMP L3:                   }
        {L7:} /$FF/$16/SwopBuffersAddr ) ;  { CALL SwopBuffers          }
        {L3:}
    {--------------------------------------------------------}
    { Check OK to Send Byte                                  }
    {--------------------------------------------------------}
    INLINE (   $8B/$3E/CAddr             { MOV DI, C[0]      }
              /$81/$C7/$E2/$01           { ADD DI, $01E2=482 }{ SysPortOut }
              {------------------ PORT1 ---------------------}
              /$80/$3D/$00               { CMPB [DI], $00    }
              /$74/$11                   { JE X1:            }
      {L4:}   /$8B/$16/P1+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P1+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE                       { OUT DX, AL        }
              {------------------ PORT2 ---------------------}
       {X1:}  /$47                       { INC DI            }
              /$80/$3D/$0                { CMP [DI], $00     }
              /$74/$11                   { JE X2:            }
      {L4:}   /$8B/$16/P2+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P2+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE                       { OUT DX, AL        }
              {------------------ PORT3 ---------------------}
       {X2:}  /$47                       { INC DI            }
              /$80/$3D/$0                { CMP [DI], $00     }
              /$74/$11                   { JE X3:            }
      {L4:}   /$8B/$16/P3+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P3+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE                       { OUT DX, AL        }
              {------------------- PORT4 --------------------}
       {X3:}  /$47                       { INC DI            }
              /$80/$3D/$0                { CMP [DI], $00     }
              /$74/$11                   { JE X4:            }
      {L4:}   /$8B/$16/P4+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P4+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE              ) ;      { OUT DX, AL        }
    END {X4:}                     ;
{--------------------}
PROCEDURE WriteCode3 ;
{--------------------}
    BEGIN
             (****************************************************
              FOR i := 1 TO n-1 DO
              BEGIN
                WriteByte := NewB[i] ;
                WriteCode            ;
              END                    ;
              Count := 0             ;
              ****************************************************)
      INLINE ( $A1/n                        { MOV AX, n           }
              /$48                          { DEC AX              }
              /$91                          { XCHG AX, CX         }
              /$BE/$01/$00                  { MOV SI, 0001        }
              /$C6/$06/Count/$00            { MOVB Count, $00     }
{L000}
              /$BB/NewB                     { MOV BX, NN=@NewB[0] }
              /$8A/$00                      { MOV AL, [BX+SI]     }
              /$A2/WriteByte                { MOV WriteByte, AL   }
              /$46              ) ;         { INC SI              }
      (****************************************************************)
      INLINE ( $A0/WriteByte                { MOV AL, WriteByte   }
              /$FF/$06/WriteCount           { INCW WriteCount LO  }
              /$75/$04                      { JNE L0:             }
              /$FF/$06/WriteCount+2 ) ;     { INCW WriteCount HI  }
        {L0:}
      (*****************************************************************
      (********************************************
      IF (RStatEnable=0) AND (WriteByte > $7F) THEN
      BEGIN
        IF (WriteByte>$EF) THEN PrevCmd := 0
        ELSE
        BEGIN
          IF WriteByte = PrevCmd THEN EXIT  ;
          PrevCmd := WriteByte              ;
        END                                 ;
      END                                   ;
      **********************************************)
      INLINE ( $A0/WriteByte                { MOV AL, WriteByte   }
              /$82/$3E/Q+7/$00              { CMPB RStatEnable, 0 }
              /$75/$1C                      { JNE L2:+4           }
              /$3C/$80                      { CMPB AL, $80        }
              /$72/$18                      { JB L2:+4            }
              /$3C/$F0                      { CMPB AL, $F0        }
              /$72/$07                      { JB L1:              }
              /$C6/$06/PrevCmd/$00          { MOVB PrevCmd, $00   }
              /$EB/$09                      { JMP L2:             }
       {L1:}  /$3A/$06/PrevCmd              { CMP AL, PrevCmd     }
              /$74/$2E                      { JE (L5:)            }
              /$A2/PrevCmd                  { MOVB PrevCmd, AL    }
        {L2:} /$EB/$02                      { JMP Over            }

{L00}         /$EB/$C4            ) ;       { JMP Back (CX LOOP) L000: }
      (*****************************************************************
      IF BuffOption = 1 THEN
      BEGIN
        Buffer^[BufferPos] := WriteByte                                 ;
        IF BufferPos<BufferMax THEN INC(BufferPos) ELSE BufferPos := 0  ;
      END                                                               ;
      ******************************************************************)
      INLINE ( $82/$3E/Q+3/$01              { CMPB BuffOption, $01      }
              /$75/$1C                      { JNE L3:                   }

              /$1E                          { PUSH DS                   }
              /$8B/$3E/XBuff+12             { MOV DI, XBuff.Pos         }
              /$C5/$1E/XBuff                { LDS BX, XBuff.Ptr         }
              /$88/$01                      { MOV [BX+DI], AL           }
              /$1F                          { POP DS                    }
              /$39/$3E/XBuff+4              { CMP DI, XBuff.Max         }
              /$76/$06                      { JBE L7:                   }
              /$FF/$06/XBuff+12             { INCW BufferPos            }
              /$EB/$0A                      { JMP L3:                   }
        {L7:} /$FF/$16/SwopBuffersAddr      { CALL SwopBuffers          }
              /$EB/$04                      { JMP L3:                   }

{L00}         /$EB/$D7 {A1}                 { JMP Back (CX LOOP) L000: }
{(L5:)}       /$EB/$79             ) ;      { JMP L5:                  }
{L3:}
    {--------------------------------------------------------}
    { Check OK to Send Byte                                  }
    {--------------------------------------------------------}
    INLINE (   $A0/WriteByte             { MOV AL, WriteByte }
              /$3C/$80                   { CMP AL, $80       }
              /$72/$0B                   { JB X0:            }
              /$24/$0F                   { AND AL, $0F       } { MCh }
              /$B4/$00                   { MOV AH, $00       }
              /$D1/$E0                   { SHL AX            }{*4: A B C D }
              /$D1/$E0                   { SHL AX            }
              /$A2/MCh                   { MOV MCh, AL       }
      {X0:}   /$8B/$3E/CurCAddr          { MOV DI, C[MCh]    }
              /$81/$C7/$A2/$01           { ADD DI, $01A2=418 }{ PortOut[0].A}
              /$03/$3E/MCh               { ADD DI, MCh       }{ MchDummy=0  }
              /$80/$3D/$00               { CMPB [DI], $00    }
              /$74/$11                   { JE X1:            }

              {------------------ PORT1 ---------------------}
      {L4:}   /$8B/$16/P1+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P1+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE                       { OUT DX, AL        }
              {------------------ PORT2 ---------------------}
       {X1:}  /$47                       { INC DI            }
              /$80/$3D/$0                { CMP [DI], $00     }
              /$74/$11                   { JE X2:            }
      {L4:}   /$8B/$16/P2+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P2+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE                       { OUT DX, AL        }
              {------------------ PORT3 ---------------------}
       {X2:}  /$47                       { INC DI            }
              /$80/$3D/$0                { CMP [DI], $00     }
              /$74/$11                   { JE X3:            }
      {L4:}   /$8B/$16/P3+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P3+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE                       { OUT DX, AL        }
              {------------------- PORT4 --------------------}
       {X3:}  /$47                       { INC DI            }
              /$80/$3D/$0                { CMP [DI], $00     }
              /$74/$11                   { JE X4:            }
      {L4:}   /$8B/$16/P4+14             { MOV DX,Linestatus }
              /$EC                       { IN AL, [DX]       }
              /$A8/$20                   { TEST AL, $20      }
              /$74/$F7                   { JZ L4:            }
              /$8B/$16/P4+4              { MOV DX, DataReg   }
              /$A0/WriteByte             { MOV AL, WriteByte }
              /$EE              ) ;      { OUT DX, AL        }
       {L5:}
    INLINE (   $E2/$81          ) ;      { LOOP L00: (DEC CX,JNZ) }
    INLINE (   $C3              ) ;      { RET                    }
    END                           ;
