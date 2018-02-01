
{-----------------------}
PROCEDURE CheckFilters  ;
{-----------------------}

      INLINE (

       $80/$3E/Q+5/$00     { CMPB FilterEnable,01 }{ FilterEnable = 1 }
      /$75/$03             { JNE L00              }
      /$E9/$99/$00         { JMP Exit             }

{L00} /$A0/WriteByte       { MOV AL, WriteByte    }
      /$24/$80             { AND AL, $80          }
      /$74/$1D             { JZ L2 (Exit)         }{ Data byte }

      /$A0/WriteByte       { MOV AL, WriteByte    }

      /$3C/$F7             { CMP AL, $F7          }{ EOSysEx ? }
      /$75/$0F             { JNE L0               }
      /$80/$3E/Filter/$FF  { CMP Filter, $FF      }
      /$75/$5A             { JNE  L5              }
      /$C6/$06/Filter/$01  { MOVB Filter , $01    }
      /$E9/$7C/$00         { JMP Exit             }

{L0}  /$80/$3E/Filter/$00  { CMPB Filter, $00     }{ Filter = 0 }
      /$74/$03             { JE L1                }
{L2}  /$E9/$72/$00         { JMP Exit             }

{L1}  /$24/$F0             { AND AL, $F0          }{ IN [80..EF] }
      /$3C/$E0             { CMP AL, $E0          }{ IN [F0..FF] }
      /$76/$03             { JBE  L3              }
      /$E9/$3F/$00         { JMP  L5              }
                           (**********************)
{L3}  /$A0/WriteByte       { MOV AL, WriteByte    }
      /$24/$0F             { AND AL, $0F          }
      /$A2/MidiCh          { MOV MidiCh, AL       )
{L4}
      /$B4/$00             { MOV AH, $00          }
      /$D1/$E0             { SHL AX               }{*2 for WORD length }
      /$BB/CAddr           { MOV BX, NN=@CAddr    }
      /$01/$C3             { ADD BX, AX           }
      /$8B/$3F             { MOV DI, [BX]         }
      /$81/$C7/>CFList     { ADD DI, CFList       }{ force word ! }
                           (**********************)
      /$B1/$04             { MOV CL, $04          }
      /$A0/WriteByte       { MOV AL, WriteByte    }
      /$24/$F0             { AND AL, $F0          }
      /$D2/$E8             { SHR AL, #[CL]        }
      /$50                 { PUSH AX              }
      /$BB/X80E0Filter     { MOV BX, NN=@X80E0Filt}
      /$D7                 { XLAT                 }
      /$A2/Filter          { MOV Filter, AL       }

      /$58                 { POP AX               }
      /$BB/X80E0Pos        { MOV BX, NN=@X80E0Pos }
      /$D7                 { XLAT                 }

      /$B4/$00             { MOV AH, $00          }
      /$01/$C7             { ADD DI, AX           }
      /$80/$7D/$00/$00     { CMP [DI] + $00, $00  }
      /$75/$05             { JNE  (Exit)          }
      /$C6/$06/Filter/$00  { MOV Filter , $00     }
{(Exit)}
      /$E9/$2A/$00         { JMP  Exit            }
{L5}
      /$BB/CAddr           { MOV BX, NN=@CAddr    }
      /$8B/$3F             { MOV DI, [BX]         }
      /$81/$C7/>CFList     { ADD DI, CFList       }{ force word ! }

      /$A0/WriteByte       { MOV AL, WriteByte    }
      /$24/$0F             { AND AL, $0F          }
      /$50                 { PUSH AX              }
      /$BB/XF0FFFilter     { MOV BX, NN=@XF0FFFilt}
      /$D7                 { XLAT                 }
      /$A2/Filter          { MOV Filter, AL       }

      /$58                 { POP AX               }
      /$BB/XF0FFPos        { MOV BX, NN=@XF0FFPos }
      /$D7                 { XLAT                 }

      /$B4/$00             { MOV AH, $00          }
      /$01/$C7             { ADD DI, AX           }
      /$80/$7D/$00/$00     { CMP [DI] + $00, $00  }
      /$75/$05             { JNE  Exit            }
      /$C6/$06/Filter/$00  { MOV Filter , $00     }
{Exit:}    ) ;

