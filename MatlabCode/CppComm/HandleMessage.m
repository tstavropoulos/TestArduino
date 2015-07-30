%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  HandleMessage(message)
%
%  Handle function - Used to process incomming messages.
%
%  Global Variables:
%    Owned:
%    External:
%       char cFlag
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function HandleMessage(message)
    global cFlag;
    disp(message);
    cFlag = message;

%            
end


