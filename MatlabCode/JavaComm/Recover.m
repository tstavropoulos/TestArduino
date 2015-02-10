%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  Recover()
%
%  Clears any Comm ports that remain open in Matlab.
%  If this doesn't solve all issues, also try pressing the reset button on the
%    Arduino.
%
%  Global Variables:
%    Owned:
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Recover()
	tmp = instrfind();
    if ~isempty(tmp)
        fclose(tmp);
        delete(tmp);
    end
end