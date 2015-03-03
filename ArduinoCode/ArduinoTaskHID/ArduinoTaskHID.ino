 /* Basic Raw HID Example
   Teensy can send/receive 64 byte packets with a
   dedicated program running on a PC or Mac.

   You must select Raw HID from the "Tools > USB Type" menu

   Optional: LEDs should be connected to pins 0-7,
   and analog signals to the analog inputs.

   This example code is in the public domain.
*/

#include <Wire.h>

const char *arduinoIdentifier = "ARDUINO";

//restart function
void(* resetFunc) (void) = 0;

//Use this to ensure that mutually incompatible MatLab code and Arduino software aren't used together
const char versionIdentifier = 'C';

//The following variables are used to set up the Properties of the session.
////The cReflectiveInd character indicates the Arduino should bounce received messages back.
////Great for testing.
const char cReflectiveInd = 'R';
////The cPairedInd character indicates the Arduino is paired with a second one.
const char cPairedInd = 'P';
////The cMasterInd character indicates the Arduino will behave as the master in the pair.
const char cMasterInd = 'M';
////The cSlaveInd character indicates the Arduino will behave as the slave in the pair.
const char cSlaveInd = 'S';
////The cFinishedInd character indicates you are Finished setting properties.
const char cFinishedInd = 'F';
////The cRestartChar causes the Arduino to restart
const char cRestartChar = '~';
////The cAck character is used as a reply to acknowledge a property.
const char cAck = 'A';

//Global flags to keep track of the above properties.
boolean bReflective = false;
boolean bPaired = false;
boolean bMaster = true;

const long signed millisRewardTimeout = 2000;

const int pins[] = {2, 3, 4, 5, 6, 7};
const int rewardPins[] = {8, 9, 10};

const int PINNUMBER = sizeof(pins) / sizeof(int);
const int REWARDNUMBER = sizeof (rewardPins) / sizeof(int);

const char rewardOnSignals[] = {'1','2','3'};
const char rewardOffSignals[] = {'4','5','6'};
byte rewardTable[256];
long signed timeRewardLeft[REWARDNUMBER];

const char downStates[] = {'A', 'B', 'C', 'D', 'E', 'F'};
const char upStates[] = {'a', 'b', 'c', 'd', 'e', 'f'};
const long signed DEBTIME = 5;

const int MESSAGEPIN = 11;

int states[PINNUMBER];
long signed debounce[PINNUMBER];
long unsigned millisThisFrame;
long unsigned millisLastFrame;

byte byReadPos;
byte byWritePos;
char readBuffer[256];

// RawHID packets are always 64 bytes
byte buffer[64];

void setup()
{
   //Serial.begin(115200);
   millisLastFrame=0;
   
   byReadPos = 0;
   byWritePos = 0;
   
   establishConnection();

   identifyMyself();

   communicateProperties();
   
   if ( bPaired )
   {
      pairedSetup();
   }
   else
   {
      bMaster = true;
   }
   
   if ( bMaster)
   {
      masterSetup();
   }
   
}

void loop() {
   millisThisFrame=millis();

   HandleSerialComm();
   
   if ( bMaster )
   {
      CheckWireComm();
   
      CheckButtons();
   
      CheckRewardTimeout();
   }

   millisLastFrame=millisThisFrame;
}

void establishConnection()
{
   bool bFinished = false;
   
   while ( !bFinished )
   {   
      while ( !anyPendingMessages() )
      {
         delay ( 200 ); 
         sendChar ( versionIdentifier );
         delay ( 300 ); 
      }
   
      //Flush the buffer of the initialization character
      char tmpChar = readChar();
      
      if ( tmpChar == cRestartChar )
      {
         resetFunc();
      }
      else
      {
         bFinished = true;
      }
   }
}

void identifyMyself()
{
   delay ( 100 ); 
   sendChars ( arduinoIdentifier );
   delay ( 100 ); 
}

void communicateProperties()
{
   boolean bFinished = false;
   boolean bAck = false;

   while ( !bFinished )
   {
      if ( anyPendingMessages() )
      {
         char inChar = readChar();
        
         switch (inChar)
         {
            case versionIdentifier:
               bAck = false;
               break;
            case cReflectiveInd:
               bAck = true;
               bReflective = true;
               break;
            case cPairedInd:
               bAck = true;
               bPaired = true;
               break;
            case cMasterInd:
               bAck = true;
               bMaster = true;
               break;
            case cSlaveInd:
               bAck = true;
               bMaster = false;
               break;
            case cFinishedInd:
               bAck = true;
               bFinished = true;
               break;
            case cRestartChar:
               resetFunc();
               break;
            default:
               bAck = false;
               sendChar('!');
               break;
         }
        
         if ( bAck )
         {
            bAck = false;
            sendChar(cAck);  
         }
      }
   }
}

void pairedSetup()
{
   if ( bMaster )
   {
      Wire.begin();
      pinMode ( MESSAGEPIN, INPUT );
   }
   else
   {
      Wire.begin(2);
      
      //Function to call when receiving Wire data from the Master
      Wire.onReceive ( receiveEvent );
      
      //Function to call when Wire data is Requested by the Master
      Wire.onRequest ( requestEvent );
      
      pinMode ( MESSAGEPIN, OUTPUT );
   }  
}

void masterSetup()
{
   for ( int iPin = 0; iPin < PINNUMBER; iPin++ )
   {
      pinMode ( pins[iPin], INPUT );
      states[iPin] = 0;
      debounce[iPin] = 0;
   }

   for ( int iRewardChan = 0; iRewardChan < REWARDNUMBER; iRewardChan++ )
   {
      pinMode ( rewardPins[iRewardChan], OUTPUT );
      timeRewardLeft[iRewardChan] = 0;
      rewardTable[byte(rewardOnSignals[iRewardChan])] = (iRewardChan | B11000000 );
      rewardTable[byte(rewardOffSignals[iRewardChan])] = (iRewardChan | B10100000 );
   }
}

void HandleSerialComm()
{
   if (bMaster)
   {
      while ( anyPendingMessages() )
      {
         HandleSerialCommMaster();
      }
   }
   else
   {
      HandleSerialCommSlave();  
   }
}

void HandleSerialCommMaster()
{
   char inSerial = readChar();
   
   if ( inSerial == cRestartChar )
   {
      resetFunc();
   }

   if ( bReflective )
   {
      sendChar(inSerial);
   }
   
   byte entry = rewardTable[byte(inSerial)];
   if ( entry )
   {
      byte rewardChan = ( entry & B11111 );
      if ( entry & B1000000 )
      {
         timeRewardLeft[rewardChan] = millisRewardTimeout;
         digitalWrite ( rewardPins[rewardChan], HIGH );
      }
      else
      {
         timeRewardLeft[rewardChan] = 0;
         digitalWrite ( rewardPins[rewardChan], LOW );
      }
   }
   else if ( bPaired )
   {
      Wire.beginTransmission(2);
      Wire.write ( inSerial );
      Wire.endTransmission();
   }
}

void HandleSerialCommSlave()
{
   digitalWrite ( MESSAGEPIN, HIGH );
}

void CheckWireComm()
{
   if ( bPaired && digitalRead ( MESSAGEPIN ) == HIGH )
   {
      Wire.requestFrom ( 2, 1 );

      while ( Wire.available() )
      {
         char c = Wire.read();
         if ( c != -1 && c != '\r' && c != '\n' )
         {
            sendChar ( c );
         }
      }
   }
}

void CheckButtons()
{
   for ( int iPin = 0; iPin < PINNUMBER; iPin++ )
   {
      if ( debounce[iPin] <= 0 )
      {
         int tmpState = digitalRead(pins[iPin]);
         if ( tmpState != states[iPin] )
         {
            states[iPin] = tmpState;
            if ( states[iPin] == HIGH )
            {
               sendChar ( downStates[iPin]  ); 
            }
            else
            {
               sendChar ( upStates[iPin] ); 
            }

            debounce[iPin] = DEBTIME;
         }
      }
      else
      {
         debounce[iPin] -= (millisThisFrame - millisLastFrame);
      }
   }
}

void CheckRewardTimeout()
{
   for ( int iRewardChan = 0; iRewardChan < REWARDNUMBER; iRewardChan++ )
   {
      if ( timeRewardLeft[iRewardChan] > 0 )
      {
         timeRewardLeft[iRewardChan] -= (millisThisFrame - millisLastFrame);

         if ( timeRewardLeft[iRewardChan] <= 0 )
         {
            digitalWrite ( rewardPins[iRewardChan], LOW );
         }
      }
   }
}

void receiveEvent ( int incomingBytes )
{
   while ( Wire.available() )
   {
      char incChar = Wire.read();
      sendChar ( incChar );
   }
}

void requestEvent()
{
   char commChar = readChar();
   
   if ( commChar != '\n' && commChar != '\r' && commChar != '\0' )
   {
      Wire.write ( commChar );
   }
   
   if ( !anyPendingMessages() )
   {
      digitalWrite ( MESSAGEPIN, LOW );
   }
}

void sendChar(const char c)
{
   //Serial.print("Sending character ");
   //Serial.println(&c);
   buffer[0] = byte(c);
   //while (!RawHID.send(buffer, 0));
   RawHID.send(buffer,100);
   buffer[0] = '\0';
}

void sendChars(const char *c)
{
   //Serial.print("Sending characters ");
   //Serial.println(c);
   for ( int n =0; n <63 && c[n]!='\0'; n++)
   {
      buffer[n] = byte(c[n]);
   }
   
   //while(!RawHID.send(buffer, 0));
   RawHID.send(buffer, 100);
   clearBuffer();
}

char readChar()
{
   if ( anyPendingMessages() )
   {
      return readBuffer[byReadPos++];
   }
   
   return '\0';
}

boolean anyPendingMessages()
{
   updatePendingBuffer();
   /*
   if ( byReadPos != byWritePos )
   {
      return true;  
   }
   */
   return ( byReadPos != byWritePos );
}

void updatePendingBuffer()
{
   if ( RawHID.recv(buffer,0) > 0 )
   {
      int i = 0;
      while ( (buffer[i] != 0) && (byWritePos+1 != byReadPos) )
      {
         readBuffer[byWritePos++] = char(buffer[i++]);
      }
      
      clearBuffer();
   }
}

void clearBuffer()
{
   for ( int n = 0; n < 64; n++ )
   {
      buffer[n] = '\0';
   } 
}

