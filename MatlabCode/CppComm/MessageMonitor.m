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
	
	while ( SerialDataAvail() )
		HandleMessage ( ReadChar() );
	end
end

