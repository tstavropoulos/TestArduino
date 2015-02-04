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
%      serialthing - A handle to the Serial object created to interface with
%        the Arduino.
%    External:
%      Owner: Init
%        bCallback - A boolean to disable the callback (when handling messages
%          manually)
%        wSerialInitChar - The character sent to the Arduino to start the program
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SerialInit(bRefl,bPair,bMaster)
	global serialthing;
    global bCallback;
    baudRate = 115200;
	
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
	
    arduinoPort = findArduino(baudRate);
    
    try
        % COM Port is set right here
        serialthing=serial(arduinoPort);

        %Various and sundry other properties of the connection
	    %%Bit rate of communication
        set(serialthing,'BaudRate',baudRate);
		%%Number of bits per message
        set(serialthing,'DataBits',8);
		%%Number of bits after the message indicating a stop
        set(serialthing,'StopBits',1);
		%%Inclusion of a Parity bit
        set(serialthing,'Parity','none');
		%%What characters should be substituted for a '\n' in a message
        set(serialthing,'Terminator','CR/LF');

		%Callback function parameters
		%%Number of bytes to be collected before calling callback
        serialthing.BytesAvailableFcnCount = 1;
		%%Set the callback function to byte mode
        serialthing.BytesAvailableFcnMode = 'byte';
		%%Set the callback function
        serialthing.BytesAvailableFcn = @IncomingMessageCallback;
        
		%Initiate the connection
        fopen(serialthing);
		
		%The Arduino is configured to send a message when it's ready.  Wait for it.
		waitForConfirmation();
		
        %Skip over the Arduino Identity Information
        WaitForMessage(7);
        
		setProperties(bRefl,bPair,bMaster);
		bCallback = true;
		
    catch me
		rethrow(me);
        SerialCleanup();
    end
end

% function findArduino()
%
% Encapsulates the Arduino-finding code
%
function outPort = findArduino(baudRate)
    outPort='';
    for i=1:9
        try
            port = sprintf('COM%u',i);
            ser = serial(port); %#ok<TNMLP>
            set(ser,'BaudRate',baudRate);
            set(ser,'DataBits',8);
            set(ser,'StopBits',1);
            set(ser,'Parity','none');
            set(ser,'Terminator','CR/LF');
            fopen(ser);
			waitForConfirmation();
            ident = fscanf(ser,'%s',7);
            if strcmp(ident,'ARDUINO')
               outPort = port;
			   SendMessage('~');
               fclose(ser);
               return;
            end
            fclose(ser);
        catch excp
            if ~strcmp(excp.identifier,'MATLAB:serial:fopen:opfailed')
                rethrow(excp);
            end
        end
    end
    if outPort==''
       error('SerialInit:findArduino:NoArduinoFound',...
                'No port (1-9) matched the signature');
    end
end

% function waitForConfirmation()
%
% Encapsulates the confirmation wait.
%
function waitForConfirmation()
    global wSerialInitChar;
    
	w = WaitForMessage();
	if ( w == wSerialInitChar )
		display('Connected');
		SendMessage(wSerialInitChar);
	else
		%The value wasn't what we expected.  Throw an error.
		throwException(w);
	end
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
	
	if bRefl
		SendMessage('R');
		waitForAck();
	end
	
	if bPair
		SendMessage('P');
		waitForAck();
		
		if bMaster
			SendMessage('M');
			waitForAck();
		else
			SendMessage('S');
			waitForAck();
		end
	end
	
	SendMessage('F');
	waitForAck();
end

% function waitForAck()
%
% Waits for acknowledgement from the arduino and intercepts error messages.
%
function waitForAck()
	w = WaitForMessage();
	if w == '!' 
		messageID = 'SerialInit:waitForAck:BadAck';
		message = 'Serial Ack Failed.  Arduino did not understand Property.';
		exception = MException(messageID,message);
		throw(exception);
	end
end

% function throwException(serialChar)
%
% Encapsulates a large block of exception-throwing code.
function throwException(serialChar)
    global wSerialInitChar;

	messageID = 'SerialInit:BadInitMessage';
	message = 'Serial Initialization Failed. Perhaps Arduino code is the wrong version?\n';
	message = [message,'  Expected Value: ',wSerialInitChar];
	baseException = MException(messageID,message);
	
	if ischar(serialChar) 
		message2 = ['  Received Value: ',serialChar,'\n'];
		try
			assert(serialChar == wSerialInitChar,'SerialInit:throwException:MessageIncorrect',message2);
		catch causeException
			baseException = addCause(baseException,causeException);
		end
		
	end
	
	try
		assert(ischar(serialChar),'SerialInit:throwException:MessageNotChar',...
			'Unable to interpret message.');
	catch causeException2
		baseException = addCause(baseException,causeException2);
	end
	
	throw(baseException);
end
