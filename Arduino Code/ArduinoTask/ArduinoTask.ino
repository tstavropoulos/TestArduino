/*
 ArduinoTask Compatible with v1.02
 
 Digital Pins 2 through 7 are used for Buttons A through F
 Uppercase letter sent when the button is depressed
 Lowercase letter sent when the button is released
 
 Digital Pins 8, 9, and 10 will control reward.
 
 Digital Pin 11 and Analog Pins 4 and 5 are used for Arduino-Arduino communication
 
 Master/Slave determiniation made upon initialization
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


void setup()
{                        
   Serial.begin(115200);
   
   millisLastFrame=0;
   
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

void loop()
{
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
      while ( Serial.available() <= 0 )
      {
         delay ( 200 ); 
         Serial.print ( versionIdentifier );
         delay ( 300 ); 
      }
   
      //Flush the buffer of the initialization character
      char tmpChar = Serial.read();
      
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
   Serial.print ( arduinoIdentifier );
   delay ( 100 ); 
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
   
   if ( inSerial == cRestartChar )
   {
      resetFunc();
   }

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
            Serial.print ( c );
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
      Serial.print ( incChar );
   }
}

void requestEvent()
{
   char commChar = Serial.read();
   
   if ( commChar != '\n' && commChar != '\r' && commChar != '\0' )
   {
      Wire.write ( commChar );
   }
   
   if ( !Serial.available() )
   {
      digitalWrite ( MESSAGEPIN, LOW );
   }
}


