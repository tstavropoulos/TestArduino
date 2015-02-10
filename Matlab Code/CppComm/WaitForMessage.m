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
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function message = WaitForMessage(bytes)
	global iWaitTimeOut;
	global iReadBufferSize;
	
	if nargin > 1
		error('WaitForMessage:TooManyInputs','Supports only 0 or 1 inputs');
	elseif nargin == 0
		message = calllib('ArduinoCommCDLL','WaitForChar',iWaitTimeOut);
	else if nargin == 1
		pcBuffer = libpointer('int8',zeros(iReadBufferSize,1));
		calllib('ArduinoCommCDLL','WaitForChars',pcBuffer,bytes,iWaitTimeOut);
		message = pcBuffer.Value';
	end
end
