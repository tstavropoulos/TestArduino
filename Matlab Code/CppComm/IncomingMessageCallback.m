%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  IncomingMessageCallback(serialObj,event)
%
%  Callback function - executed when a message arrives from the Arduino.
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: Init
%        bCallback - Can be used to disable the callback.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function IncomingMessageCallback(serialObj,event)
    global bCallback;
    while(serialObj.BytesAvailable > 0 && bCallback)
        incChar=fscanf(serialObj,'%s',1);
        disp(incChar);
        UpdateButtons(incChar);
    end
end


