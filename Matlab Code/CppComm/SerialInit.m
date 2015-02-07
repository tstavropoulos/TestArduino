%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SerialInit(bRefl,bPair,bMaster)
%
%  Initializes the serial connection with the Arduino.
%
%  SerialInit() makes a non-reflective, non-paired connection.
%  SerialInit(bRefl) makes a connection with specified reflectivity.
%  SerialInit(bRefl,false) makes a connection with specified reflectivity.
%  SerialInit(bRefl,bPair,bMaster) makes a connection with specified values.
%
%  Passing in true for bPair and not supplying bMaster is invalid.
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: Init
%        bCallback - A boolean to disable the callback (when handling messages
%          manually)
%        wSerialInitChar - The character sent to the Arduino to start the program
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SerialInit(bRefl,bPair,bMaster)
	
	if ( ~libisloaded('ArduinoCommDLL'))
		loadArduinoDLL()
	end
	
    global bCallback;
	
	%Check the argument list to make sure a logical set was included.
	switch nargin
		case 0
			bRefl=false;
			bPair=false;
			bMaster=true;
		case 1
			bPair=false;
			bMaster=true;
		case 2
            if bPair
				error('SerialInit:bMasterMissing',...
					'if bPair is true, you must specify if Master or Slave');
			else
				bMaster=true;
            end
		case 3
    end
	
	iMaxPortNum = int32(10);
	piBuffer = libpointer('int32Ptr',zeros(iMaxPortNum,1));
	calllib('ArduinoCommDLL','FindArduinos',piBuffer,iMaxPortNum);
	arduinoPort = piBuffer.Value(1);
    
    try
		% Set communication properties
		setProperties(bRefl,bPair,bMaster);
		
        % Connect to the serial port
		calllib('ArduinoCommDLL','Connect',arduinoPort);
        
		bCallback = true;
		
    catch me
		rethrow(me);
    end
end

function loadArduinoDLL()
	mfilepath = fileparts(which('SerialInit.m'));
	addpath(fullfile(mfilepath,'..\..\ArduinoCommSuite\MATLAB\'));
	addpath(fullfile(mfilepath,'..\..\ArduinoCommSuite\ArduinoCommDLL\'));
	loadlibrary('ArduinoCommDLL','ArduinoCommDLL.h');
	calllib('ArduinoCommDLL','Init');
end

% function setProperties(bRefl,bPair,bMaster)
%
% Used to pass along properties to the Arduino.
%
function setProperties(bRefl,bPair,bMaster)
	assert(islogical(bRefl),'SerialInit:setProperties:bReflIncorrect',...
		'bRefl and bPair must be true or false.');
	assert(islogical(bPair),'SerialInit:setProperties:bPairIncorrect',...
		'bRefl and bPair must be true or false.');
	assert(islogical(bMaster),'SerialInit:setProperties:bMasterIncorrect',...
		'bRefl, bPair, and bMaster must be true or false.');
	
	calllib('ArduinoCommDLL','SetProperties',bRefl,bPair,bMaster);
end
