%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SendMessage( message )
%
%  Sends the passed in message to the Arduino.
%
%  Global Variables:
%    Owned:
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SendMessage( message )
	calllib('ArduinoCommCDLL','SendChars', message, length(message));
end
