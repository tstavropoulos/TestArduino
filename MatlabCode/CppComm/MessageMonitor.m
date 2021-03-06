%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  MessageMonitor(~)
%
%  Monitors Serial Communication.
%    RegisterUpdate ( @MessageMonitor );
%
%  Global Variables:
%    Owned:
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function MessageMonitor(~)
	
	if ( SerialDataAvail() )
		HandleMessage ( ReadChar() );
	end
end

