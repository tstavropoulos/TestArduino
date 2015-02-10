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
	if ( libisloaded('ArduinoCommDLL'))
        calllib('ArduinoCommDLL','Disconnect');
        unloadlibrary('ArduinoCommDLL');
	end
end