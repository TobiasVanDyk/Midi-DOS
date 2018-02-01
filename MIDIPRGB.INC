
       {----------------------------------------------------------}
       INLINE  ( $A0/CmdB               { MOV AL, CmdB            }
                /$3C/$C0                { CMP AL, $C0             }
                /$74/$06                { JE L1                   }
                /$3C/$D0                { CMP AL, $D0             }
                /$74/$02                { JE L1                   }
                /$EB/$74                { JMP Exit                }

       {L1:}    /$C7/$06/n/$01/$00      { MOVW n, $0001           }
                /$C7/$06/i/$00/$00      { MOVW i, $0000           }

          {L6:} /$8B/$3E/CurCAddr       { MOV DI, CurCAddrC[MCh]  }
                /$81/$C7/$AE/$00        { ADD DI, $AE             }
                /$03/$3E/i              { ADD DI, i               }
                /$A0/CmdB               { MOV AL, CmdB            }
                /$3C/$C0                { CMP AL, $C0             }
                /$75/$05                { JNE L2                  }
                /$8B/$45/$00            { MOV AX, [DI(+$AE+i)]    }
                /$EB/$03                { JMP L3                  }
          {L2:} /$8B/$45/$10            { MOV AX, [DI(+$BE+i)]    }
          {L3:} /$3C/$01                { CMP AL, $01             }
                /$75/$2D                { JNE L4:                 }

                /$BB/NewB               { MOV BX, NN=@NewB[0]     }
                /$03/$1E/n              { ADD BX, n               }
                /$A1/i                  { MOV AX, i               }
                /$0A/$06/CmdB           { OR AL, CmdB             }
                /$88/$07                { MOV [BX], AL            }

                /$A0/Data1              { MOV AL, Data1           }

                /$80/$7D/$64/$01        { CMP [DI+(274+i)], $01   }
                /$75/$0B                { JNE L5:                 }{NoXlat}
                /$53                    { PUSH BX                 }
                /$8B/$1E/CurCAddr       { MOV BX, CurrC [MCh]     }
                /$81/$C3/$22/$01        { ADD BX, 290=OFSPatchXLat}
                /$D7                    { XLAT                    }
                /$5B                    { POP BX                  }

          {L5:} /$88/$47/$01            { MOV [BX+1],AL NewB[n+1] }
                /$81/$06/n/$02/$00      { ADD n, $0002            }
          {L4:} /$FF/$06/i              { INCW i                  }
                /$81/$3E/i/$10/$00      { CMPW i, $0010           }
                /$75/$A8                { JNE L6:                 }

                /$82/$3E/n/$01          { CMPB n, $01             }
                /$74/$04                { JE L7:                  }

          {L7:} /$FF/$16/Write3Addr     { CALL WriteCode3         }
                /$C6/$06/Count/$00  );  { MOVB Count, $00         }
      {Exit}   (*************************************************************)
               { IF (CmdB IN [$C0,$D0]) THEN                                 }
               { BEGIN                                                       }
               {   n := 1                 ;                                  }
               {   FOR i := 0 TO 15 DO                                       }
               {   BEGIN                                                     }
               {     B := 0                                                  }
               {     IF (CmdB=$C0) AND (C[MCh].ChMess.C0[i]=1) THEN B := 1 ; }
               {     IF (CmdB=$D0) AND (C[MCh].ChMess.D0[i]=1) THEN B := 1 ; }
               {     IF B=1 THEN                                             }
               {     BEGIN                                                   }
               {       NewB[n]   := CmdB + i    ;                            }
               {       IF C[MCh].PatchCOut[i] = 1 THEN                       }
               {                 NewB[n+1] := C[MCh].PatchXlat[Data1]        }
               {            ELSE NewB[n+1] := Data1                     ;    }
               {       n := n+2                 ;                            }
               {     END                        ;                            }
               {   END                          ;  i Loop                    }
               {   IF n>1 THEN WriteCode1       ;                            }
               { END                            ;  CmdB=C0,D0 and Count=2    }
               (*************************************************************)
