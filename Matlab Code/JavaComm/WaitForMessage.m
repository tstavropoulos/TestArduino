%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  WaitForMessage() returns message
%
%  Waits for a message from the Arduino, Pausing the script in the meantime.
%  WaitForMessage() waits for one byte to arrive (default).
%  WaitForMessage(bytes) waits for the indicated number of bytes to arrive.
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: SerialInit
%        serialthing - A handle to the Serial object created to interface with
%          the Arduino.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function message = WaitForMessage(bytes)
	global serialthing;
	
	if nargin > 1
		error('WaitForMessage:TooManyInputs','Supports only 0 or 1 inputs');
	elseif nargin == 0
		bytes = 1;
	end
	
    message = fscanf(serialthing,'%s',bytes);
end
