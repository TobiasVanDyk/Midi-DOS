{ MIDIPRGC.INC }
        INLINE (
         $C7/$06/i/$00/$00   { MOVW i, $0000           }
        { FOR i := 0 TO 3 DO                           }
        { BEGIN                                        }
        (**********************************************)
  {L0:} /$8B/$3E/i            { MOV DI, i              } { INC i by 7 }
        /$03/$3E/CurCAddr     { ADD DI, CurCAddr C[MCh]}
        /$81/$C7/$F6/$00      { ADD DI, $00F6 = 246    } { Split[i] }

        /$8B/$45/$01          { MOV AX, [DI+$01]       } { AL=Start, AH=Stop}
        /$3A/$06/Data1        { CMP AL, Data1          }
        /$77/$72              { JA nexti               } { Data1<Start }
        /$3A/$26/Data1        { CMP AH, Data1          }
  {D0:} /$72/$6C              { JB nexti               } { Data1>Stop }
        /$90/$90              { NOP                    }
        { IF (Data1 >= C[MCh].Split[i].Start) AND      }
        {    (Data1 <= C[MCh].Split[i].Stop)  THEN     }
        (**********************************************)
  {D1:} /$BB/NewB             { MOV BX, NN=@NewB[0]    }
        /$03/$1E/n            { ADD BX, n              }
        /$B8/$90/$00          { MOV AX, $0090          } { NoteOn }
        /$0A/$45/$00          { OR AL, [DI+$00]        } { COut }
        /$88/$07              { MOV [BX], AL           }
        {   NewB[n]   := $90 + C[MCh].Split[i].COut    }
        (**********************************************)
        /$8B/$45/$03          { MOV AX, [DI+$03]       } { Pitch }
        {   Pp := C[Mch].Split[i].Pitch         ;      }
        /$8A/$16/Data1        { MOV DL, Data1          }
        /$86/$C4              { XCHG AL, AH            }
        /$3C/$01              { CMP AL, $01            }
        /$72/$06              { JB L1:        HI(Pp)=0 }
        /$77/$08              { JA L2:        HI(Pp)=2 }

        /$2A/$D4              { SUB DL, AH    HI(Pp)=1 }
        /$EB/$06              { JMP L3                 }
  {L1:} /$02/$D4              { ADD DL, AH             }
        /$EB/$02              { JMP L3                 }
  {L2:} /$8A/$D4              { MOV DL, AH             }

  {L3:} /$86/$D0              { XCHG AL, DL            }
        /$3C/$80              { CMP AL, $80            }
        /$72/$03              { JB L6                  }
  {L5:} /$A0/Data1            { MOV AL, Data1          }
  {L6:} /$88/$47/$01          { MOV [BX+1],AL NewB[n+1]}
        /$EB/$02              { JMP L8:                }
        {   IF HI(Pp)=0 THEN NewB[n+1] := Data1 + LO(Pp) ;   }
        {   IF HI(Pp)=1 THEN NewB[n+1] := Data1 - LO(Pp) ;   }
        {   IF HI(Pp)=2 THEN NewB[n+1] := LO(Pp)         ;   }
        {   IF NewB[n+1]>127 THEN NewB[n+1] := Data1     ;   }

  {L7:} /$EB/$AB              { JMP L0:                }
        {----------------------------------------------}
  {L8:} /$8B/$45/$05          { MOV AX, [DI+$05]       } { Velocity }
        (**********************************************)
        /$82/$3E/Data2/$00    { CMPB Data2, 0          }
        /$74/$1C              { JE L5                  }
        /$8A/$16/Data2        { MOV DL, Data2          }
        /$86/$C4              { XCHG AL, AH            }
        /$3C/$01              { CMP AL, $01            }
        /$72/$06              { JB L1:        HI(Vv)=0 }
        /$77/$08              { JA L2:        HI(Vv)=2 }

        /$2A/$D4              { SUB DL, AH    HI(Vv)=1 }
        /$EB/$06              { JMP L3                 }
  {L1:} /$02/$D4              { ADD DL, AH             }
        /$EB/$02              { JMP L3                 }
  {L2:} /$8A/$D4              { MOV DL, AH             }

  {L3:} /$86/$D0              { XCHG AL, DL            }
        /$3C/$80              { CMP AL, $80            }
        /$72/$03              { JB L6                  }
  {L5:} /$A0/Data2            { MOV AL, Data2          }
  {L6:} /$88/$47/$02          { MOV [BX+2],AL NewB[n+2]}
        {   IF (Data2>0) THEN                                }
        {   BEGIN   NoteOff stay=0                           }
        {     Vv := C[MCh].Split[i].Vel                    ; }
        {     IF HI(Vv)=0 THEN NewB[n+2] := Data2 + LO(Vv) ; }
        {     IF HI(Vv)=1 THEN NewB[n+2] := Data2 - LO(Vv) ; }
        {     IF HI(Vv)=2 THEN NewB[n+2] := LO(Vv)         ; }
        {     IF NewB[n+2]>127 THEN NewB[n+2] := Data2     ; }
        {   END ELSE NewB[n+2] := Data2                    ; }
        (****************************************************)
        /$81/$06/n/$03/$00     { ADD n, $0003         }
        {   n := n+3                                  }
        { END                      ;          Ch[i]>0 }
        (*********************************************)
{Nexti} /$81/$06/i/$07/$00     { ADD i, $0007         }
        /$81/$3E/i/$1C/$00     { CMPW i, $001C=28=7*4 }
        /$75/$BE           ) ; { JNE L7:              }
 {Exit} { END                ;         i Loop         }
        (*********************************************)

