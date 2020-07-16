# Midi-DOS
This used a modified RS232 serial card with an 8250 UART (the crystal is replaced with a 5MHz or 10MHz crystal) as a Midi interface card. It is used to capture and send all midi data types. Refer to Readme.txt for more information (or as copied below).

  ### HARDWARE CONFIGURATION : Page 1                                          
  (1) Configure: Interrupt Request (IRQ), Serial Port (SER), Board Freq.   
      (XTAL) by using the DOS commandline :                                
      MIDIWISE IRQY:X SERY:X XTAL:X {ENTER}.                               
      X : IRQY: Number as set on serial card, with X = {3,4,5,or 7}.       
          SERY: Number (COM port) as set on card, with X = {1,2,3,or 4}.   
          XTAL: Board oscillating frequency, X={1,2,3,4}={5,10,15,20} MHz. 
          (X=E or X=D: Enable or disable specified serial port.)           
      Y : Specify Serial Port assignment to Programme Ports {A,B,C,D} for  
          SERY: and IRQY: parameters.                                      
  (2) Disable Ports with SERB:D, Port B Disabled, Enable with SERB:E.      
      Program will do auto-detection of COM1 to COM4. The Disable-Enable   
      state are indicated by Ena='Y' in the Menu:Options display. Serial   
      port detection are indicated by Inst='Y' or 'N' in the same panel.   
  (3) The order, number of spaces, sequence, and case of the 2 parameters  
      above is not important: Sera:1 Irqc:3, IRQC:3 SERA:1, irqC:3 serA:1, 
      are all allowed. If any equivalent parameter is repeated the last    
      definition in the commandline will be used.                          
  (4) The hardware configuration is stored in the file HW.CFG. Do not use  
      this name for any of the Key, Channel or Data Files, and must be     
      present in the same directory as the main executable (Midiwise.exe)  
      file. (Size of HW.CFG = 28 bytes.)                                   
                                                                           
  ### HARDWARE CONFIGURATION : Page 2                                          
  (6) Problems: Check for IRQ assignment clashes, for example a network,   
      printer, or sound card using the same IRQ number. It is possible to  
      use sections of the program with no IRQ assignment, for example the  
      Code Transmitter, Buffer Read and Send Options from the Main Menu    
      are used in Polling Mode i.e. IRQ line not used. ( The Activate-Run  
      Option does require an IRQ assignment. ) Note that the other         
      options do need a SERY:X allocation however, i.e. to configure these 
      use the DOS commandline {MidiWise SERY:X}.                           
  (7) Examples and Set-Up :                                                
      (a) MIDIWISE SERA:3 SERB:4 SERC:1 SERD:2 SERD:D SERC:D SERA:E SERB:E 
          Assign Port A = Serial Port 3 (COM3), and enable Port A.         
          Assign Port B = Serial Port 4 (COM4), and enable Port B.         
          Assign Port C = Serial Port 1 (COM1), and enable Port C.         
          Assign Port D = Serial Port 2 (COM2), and enable Port D.         
          This means that when Port A is selected in the program, output   
          and input to Port A will be sent-received by COM3. Note that     
          only Ports A and B may be selected for input simultanously, but  
          any combination of A, B, C, D for output.                        
      (b) MIDIWISE SERA:4 IRQA:3 IRQD:7 XTAL:2                             
          Assign PortA to COM4, Set PortD (previously assigned to COMX)    
          for INT Request 7, PortA to IRQ 3, use XTAL Freq=10 MHz.         
                                                                           
  ### DOS COMMAND LINE PARAMETERS  Page 1                                      
  (1) Hardw.Config:{SERY:X X=1,2,3,4,D,E. Y=A,B,C,D.} (Assign ComX>PortY)  
                   {IRQY:X X=3,4,5,7 Y=A-D.} (Assign INTReqX>PortY )       
                   {XTAL:X=1,2,3,4} Xtal freq: 5,10,15,20 MHz.             
  (2) Midi Config.:{CONF:(D:Drive)(P=Pathname)Filename}.                   
  (3) Key Config. :{PKEY:(D:Drive)(P=Pathname)Filename}.                   
  (4) Data(Editor):{DATA:(D:Drive)(P=Pathname)Filename}.                   
  MIDI CONFIGURATION FILE : SIZE:7684 bytes.                               
  Stores channel reassignment, Keyboard Split, Initialisation Strings,     
  Controllers redirection, Filter Detail, Pitch and Velocity changes.      
  KEY CONFIGURATION FILE  : SIZE:842 bytes.                                
  Stores File and Buffer Linked Programmable Keys configuration. The 1st   
  are linked to individual files on disk, and the last to specific editor  
  (data or buffer dump) disk files via the editor.                         
  DATA FILES : SIZE 1-60000  bytes. (Larger files sent to Overflow buffer) 
  Stores midi codes data from the editor or buffer.                        
  For example : Entering                                                   
  MIDIWISE IRQA:5 SERA:1 PKEY:A:Keys\Perform.1 CONF:Midi1.c1 DATA:DumpA.01 
  from the DOS commandline (or via a batch file) will load the following   
  files: Perform.1 from drive A: DIR Keys, and HW.CFG, DumpA.01, and       
  Midi1.c1 from the current Directory. The sequence, combination, and      
  case of these parameters are not important.                              
                                                                           
  ### MAIN MENU HELP                                                           
  {E}ditor-Key Configuration: Full feature Data Editor. Data displayed     
                              as 256 byte HexDump. Programmable Key        
                              configuration defined inside editor.         
  {C}onfiguration Panel: Midi Channel, Velocity, Pitch, Controllers,       
                         and Keyboard split. Used by {A}ctivate option.    
  {B}uffer Display: Data displayed as 300 byte decimal dump. Colour        
                    tagging of midi code types. No editing from here.      
  {A}ctivate-Run: Serves as a midi through, by piping input midi codes     
                  through the configuration as set in options {F} and {C}. 
                  Programmable Keys, if defined, are also active.          
  {F}ilter Configuration: Allows selected midi codes (or code groups),     
                          present in the input stream, to be filtered      
                          from the output stream.                          
  {O}ptions-Files: Options: Toggle Buffer storage, SysEx Detail, Code      
                            Display, Port Cfg, and Filter Enable.          
  {D}isk Files: Load and save, Data, Cfg, Key and Buffer (data) files.     
                Directory display.                                         
  {X}mit: Transmit midicodes entered a line, or singly, at a time.         
  {R}ead into Buffer: Raw input mididatastream stored in buffer. Not       
                      affected by any of the {C} option settings.          
  {S}end from Buffer: Transmit selected section (or all of) buffer.        
                                                                           
  ### BUFFER DUMP                                                              
  (1) Displays midi data in decimal format as a 300 byte dump per page.    
  (2) Midi codes are color tagged as follows:                              
      CODES            COLOUR                                              
      OOO-127 (OO-7F)  Blue    Midi Data Bytes.                            
      128-159 (8O-9F)  Black   Note Off and Note On Command Codes.         
      16O-239 (AO-EF)  Green   Channel Command Messages.                   
      241-255 (F1-FF)  Red     Timing, Active Sense etc.                   
      24O,241 (FO,F7)  Yellow  Sys Ex Start and End.                       
  (3) The following keys are active during the buffer display:             
      {Home}         : Return to page 0.                                   
      {End}          : Go to last page (Page 234).                         
      {Page Up-Down} : Previous and Next Pages.                            
      {E}            : End of Dump                                         
      {Q}{ESC}       : Return to Main Menu.                                
      {Any other key}: Next Page.                                          
  (4) No Editing is possible from this option, but the Hex Dump as found   
      in the Editor (Option{E}) offers comprehensive editing functions.    
  (5) Both the normal buffer and overflow buffer (if filled) are displayed 
      as one. This differs from the Editor where the buffer to be edited   
      must be selected from the normal or overflow buffer. As a result,    
      the total dump size can vary between 60000 - 120000 bytes, depending 
      on memory available.                                                 
   ACTIVATE RUN {Midi Through}: Activates a MIDI-IN->MIDI-OUT function:    
                                                                         
    MIDI DATA IN   : {COM:1-4} {IRQ3,4,5,7} {Port A and/or B} (XTL:1-4)    
                   Serial Port configured as in file HW.CFG.             
                                                                        
   INTERRUPT CONTR : Expands all running status codes to normal.           
                   Controls circular temporary, input buffer: 256 bytes. 
                : Main Menu {C}:(A) Output Port Assignment A,B,C,D      
   CHANNEL CONFIG.                 (B) Channel and Controllers redirection 
                                 (C) Pitch, Velocity configuration.      
                                (D) Keyboard Split configuration.       
   FILTER CONFIG.  : Main Menu {F}: Define seperate filter list for each   
                                  midi input channel.                    
                : 4 Sets: (A) Remote Keyboard Control.       {12 keys}  
  PROG.KEYS CONFIG           (B) Initialising Strings.          {16 keys}  
                           (C) F1-F12: (Norm,Shift,Ctrl,Alt). {48 keys}  
                          (D) A-Z: File-linked Keys (A-Z).   {26 keys}  
   OPTIONS CONFIG  : (A) Initialising Strings Enable: (Not)Transmitted.    
                   (B) Buffer storage : OUTPUT Data stored in buffer.    
                     (C) Codes display  : INPUT Data displayed.            
                  (D) System Ex Detail Enable: (Not)Displayed.          
   MIDI DATA OUT   : Output Data stream {COM:1-4} and Ports A-D.           
                      
