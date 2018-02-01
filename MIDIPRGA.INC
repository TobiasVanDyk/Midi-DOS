
INLINE ( $A0/RemoteMaster       { MOV AL, RemoteMaster    }
        /$3A/$06/Data1          { CMP AL, Data1           }
        /$75/$16                { JNE D1:                 }
        /$82/$3E/Data2/$00      { CMPB Data2, 0           }
        /$74/$0C                { JE D2:                  }
        /$82/$3E/RemoteFlag/$00 { CMPB RemoteFlag, 0      }
        /$75/$05                { JNE D2                  }
        /$C6/$06/RemoteFlag/$01 { MOVB RemoteFlag, $01    }
  {D2:} /$E9/$9E/$00   {9F}     { JMP Exit                }

  {D1:} /$C7/$06/i/$00/$00   { MOVW i, $0000           }
       { FOR i := 0 TO 15 DO                           }
       { BEGIN                                         }
  {L0:} /$8B/$3E/i            { MOV DI, i              }
        /$D1/$E7              { SHL DI  2* = WORD      }
        /$03/$3E/CurCAddr     { ADD DI, CurCAddr C[MCh]}

        /$80/$7D/$06/$00      { CMPB [DI+$06], $00     }
        /$75/$03              { JNE N1:                }
        /$E9/$79/$00          { JMP End1               }
  {N1:} { IF (C[MCh].Ch[i] > 0) THEN                   }
        { BEGIN                                        }
        (**********************************************)
        /$BB/NewB             { MOV BX, NN=@NewB[0] }
        /$03/$1E/n            { ADD BX, n           }
        /$A1/i                { MOV AX, i           }
{+2}    /$0A/$06/CmdB         { OR AL, CmdB         }
        /$88/$07              { MOV [BX], AL        }
        { NewB[n]   := $90 + i  ;                   }
        {----------------------------------------------}
        { Check Pitch Xpose                            }
        {----------------------------------------------}
{-12}   /$8B/$45/$26          { MOV AX, [DI+$26]       }
        { Pp := C[MCh].Pitch[i]                   ;    }
        (******************************************************)
        /$82/$3E/Px/$00       { CMPB Px, 0             }
        /$74/$1C              { JE L5                  }
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
        (****************************************************
         IF (Px>0) THEN
                   BEGIN
                     IF HI(Pp) = 0 THEN NewB[n+1] := Data1+LO(Pp) ;
                     IF HI(Pp) = 1 THEN NewB[n+1] := Data1-LO(Pp) ;
                     IF HI(Pp) = 2 THEN NewB[n+1] := LO(Pp)       ;
                     IF NewB[n+1] > 127 THEN NewB[n+1] := Data1   ;
                   END
                   ELSE NewB[n+1] := Data1                        ;
         ***************************************************)
  {L7:} /$EB/$AD  {AC}        { JMP L0:                }
        {----------------------------------------------}
        { Velocity                                     }
        {----------------------------------------------}
  {L8:} /$8B/$45/$46          { MOV AX, [DI+$46]       }
        { Vv := C[MCh].Vel[i]                        ; }
        (******************************************************)
        /$82/$3E/Vx/$00       { CMPB Vx, 0             }
        /$74/$23              { JE L5                  }
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
        (****************************************************
         IF (Vx>0) AND (Data2>0) THEN   { NoteOff stay=0}
             BEGIN
               IF HI(Vv) = 0 THEN NewB[n+2] := Data2+LO(Vv) ;
               IF HI(Vv) = 1 THEN NewB[n+2] := Data2-LO(Vv) ;
               IF HI(Vv) = 2 THEN NewB[n+2] := LO(Vv)       ;
               IF NewB[n+2] > 127 THEN NewB[n+2] := Data2   ;
             END
             ELSE NewB[n+2] := Data2                        ;
          ****************************************************)
        /$81/$06/n/$03/$00     { ADD n, $0003         }
        { n := n+3                                   ;}
{End1}  { END                      ;          Ch[i]>0 }
        (*********************************************)
        /$FF/$06/i             { INCW i               }
        /$81/$3E/i/$10/$00     { CMPW i, $0010        }
        /$75/$B9           ) ; { JNE L7:              }
 {Exit} { END                        ;         i Loop  }
        (**********************************************)
