%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SerialCleanup()
%
%  Cleans up any detritus from the Serial Handle.
%
%  Global Variables:
%    Owned:
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SerialCleanup()
	if ( libisloaded('ArduinoCommCDLL'))
        calllib('ArduinoCommCDLL','Disconnect');
        unloadlibrary('ArduinoCommCDLL');
	end
end