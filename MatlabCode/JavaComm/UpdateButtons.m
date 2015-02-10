%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  UpdateButtons ( char )
%
%  Updates the current state of buttons when receiving messages from the Arduino
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: Init
%        ButtonStates - An array of the current state of each button connected
%          to the Arduino
%        ButtonReverseLookup - A hashtable of the button-related messages
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function UpdateButtons(message)
    global ButtonStates;
	global ButtonReverseLookup;
	
	if ButtonReverseLookup.isKey(message)
		tmp = ButtonReverseLookup(message);
        newState = tmp(1);
        buttonPressed = tmp(2);
        ButtonStates(buttonPressed) = newState;
	end
	
end

