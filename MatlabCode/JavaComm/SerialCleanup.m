%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SerialCleanup()
%
%  Cleans up any detritus from the Serial Handle.
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: SerialInit
%        serialthing - A handle to the Serial object created to interface with
%          the Arduino.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SerialCleanup()
	global serialthing;

	fclose(serialthing);
	delete(serialthing);
    clear serialthing;
end