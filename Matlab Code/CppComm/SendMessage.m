%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SendMessage( message )
%
%  Sends the passed in message to the Arduino.
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: SerialInit
%        serialthing - A handle to the Serial object created to interface with
%          the Arduino.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SendMessage( message )
	global serialthing;
    fprintf(serialthing,'%s',message);
end
