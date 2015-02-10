/*
 Arduino Timing Test
 
 Digital Pins 2 through 7 are used for Buttons A through F
 Uppercase letter sent when the button is depressed
 Lowercase letter sent when the button is released
 
 Digital Pins 8, 9, and 10 will control reward.
 
 Digital Pin 11 and Analog Pins 4 and 5 are used for Arduino-Arduino communication
 
 Master/Slave determiniation made upon initialization
 */

#include <Wire.h>

const char *arduinoIdentifier = "ARDUINO";

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
////The cAck character is used as a reply to acknowledge a property.
const char cAck = 'A';

//Global flags to keep track of the above properties.
boolean bReflective = false;
boolean bPaired = false;
boolean bMaster = true;

const long signed millisRewardTimeout = 20000;

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

char slaveReadBuffer[256] = {};
byte bufferBegin = 0;
byte bufferEnd = 0;

const int MESSAGEPIN = 11;

int states[PINNUMBER];
long signed debounce[PINNUMBER];
long unsigned microsThisFrame;
long unsigned microsLastFrame;
long unsigned microsFrameTime;

const byte TIMETRACKINDEXNUM = 24;

long unsigned timeArray[TIMETRACKINDEXNUM][4] = {};
long unsigned frameTimeArray[TIMETRACKINDEXNUM] = {};
long unsigned timeRef = 0;
byte bufferTimeSubIndex = 0;
byte bufferTimeEnd = 0;
bool bDump = false;


void setup()
{                        
   Serial.begin(115200);
   
   microsLastFrame=micros();

   identifyMyself();

   establishConnection();

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

void loop()
{
   
   microsThisFrame=micros();
   microsFrameTime = microsThisFrame - microsLastFrame;
   
   timeReset();

   HandleSerialComm();
   stamp();
   
   if ( bMaster )
   {
      CheckWireComm();
      stamp();
   
      CheckButtons();
      stamp();
   
      CheckRewardTimeout();
      stamp();
   }
   
   if ( bDump )
   {
      bDump = false;
      dumpTime();
   }

   microsLastFrame=microsThisFrame;
}

void identifyMyself()
{
   Serial.print ( arduinoIdentifier );
   delay ( 500 ); 
}

void establishConnection()
{
   while ( Serial.available() <= 0 )
   {
      Serial.print ( versionIdentifier );
      delay ( 1000 ); 
   }

   //Flush the buffer of the initialization character
   Serial.read();
}

void communicateProperties()
{
   boolean bFinished = false;
   boolean bAck = false;

   while ( !bFinished )
   {
      if ( Serial.available() )
      {
         char inChar = Serial.read();
        
         switch (inChar)
         {
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
            default:
               bAck = false;
               Serial.print('!');
               break;
         }
        
         if ( bAck )
         {
            bAck = false;
            Serial.print(cAck);  
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
      while ( Serial.available() > 0 )
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
   char inSerial = Serial.read();

   if ( bReflective )
   {
      Serial.print(inSerial);
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
   
   if ( inSerial == 'D' )
   {
     bDump = true; 
   }
}

void HandleSerialCommSlave()
{
   //Signal to the Master that you have a message
   digitalWrite ( MESSAGEPIN, HIGH );

   while ( Serial.available() )
   {
      //Write the next message in the next spot on the buffer
      slaveReadBuffer[bufferEnd++] = Serial.read();
   }

}

void CheckWireComm()
{
   if ( bPaired && digitalRead ( MESSAGEPIN ) == HIGH )
   {
      Wire.requestFrom ( 2, 1 );

      while ( Wire.available() )
      {
         Serial.print ( Wire.read() );
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
               Serial.print ( downStates[iPin]  ); 
            }
            else
            {
               Serial.print ( upStates[iPin] ); 
            }

            debounce[iPin] = DEBTIME;
         }
      }
      else
      {
         debounce[iPin] -= microsFrameTime;
      }
   }
}

void CheckRewardTimeout()
{
   for ( int iRewardChan = 0; iRewardChan < REWARDNUMBER; iRewardChan++ )
   {
      if ( timeRewardLeft[iRewardChan] > 0 )
      {
         timeRewardLeft[iRewardChan] -= microsFrameTime;

         if ( timeRewardLeft[iRewardChan] <= 0 )
         {
            digitalWrite ( rewardPins[iRewardChan], LOW );
         }
      }
   }
}

//receiveEvent is called on the slave when the master wishes to send a message
void receiveEvent ( int incomingBytes )
{
   //Read all the messages
   while ( Wire.available() )
   {
      //Send the messages to the host computer
      Serial.print ( Wire.read() );
   }
}


//requestEvent is called on the Slave when the master is ready for a transmission
void requestEvent()
{
   //Send the next waiting character
   Wire.write ( slaveReadBuffer[bufferBegin++] );
   
   //If you've read all the characters, you're done
   if ( bufferBegin == bufferEnd )
   {
      digitalWrite ( MESSAGEPIN, LOW );
   }
}

void timeReset()
{
   bufferTimeSubIndex = 0;
   frameTimeArray[bufferTimeEnd] = microsFrameTime;
   bufferTimeEnd = (bufferTimeEnd + 1)%TIMETRACKINDEXNUM;
   timeRef = micros();
}

void stamp()
{
   timeArray[bufferTimeEnd][bufferTimeSubIndex++] = micros() - timeRef;
   timeRef = micros();
}

void dumpTime()
{
   frameTimeArray[bufferTimeEnd] = micros() - microsThisFrame;
   
   Serial.print ( F("\nArduino Uno\n\nDumping Time:\tMicroSeconds\n\n") );
   
   Serial.print ( F("Run Number\tFrameTime\tHandleSerialComm()\tCheckWireComm()\tCheckButtons()\tCheckRewardTimeout()\n" ));
   
   
   for ( int i = 0; i < TIMETRACKINDEXNUM; i++ )
   {
      int trueIndex = (bufferTimeEnd+i+1)%TIMETRACKINDEXNUM;
      Serial.print ( "Loop " );
      Serial.print ( i );
      Serial.print ( "\t" );
      Serial.print ( frameTimeArray[trueIndex] );
      for ( int j = 0; j < 4; j++ )
      {
          Serial.print ( "\t" );
          Serial.print ( timeArray[trueIndex][j] );
      }
      Serial.print ( "\n" );
   }
   
   Serial.println ( F("\n Done Dumping Time\n") );
   
   
}
