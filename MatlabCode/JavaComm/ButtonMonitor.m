%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  ButtonMonitor(deltaT)
%
%  Monitors the buttons.  Called every frame to run logic on the button states.
%  Meant to be used with the Looper.
%  Added to the loop with:
%    RegisterUpdate ( @ButtonMonitor );
%
%  Global Variables:
%    Owned:
%      PressDurationForReward - Amount of ms required to dispense reward
%    External:
%      Owner: Init
%        ButtonStates - An array of the current state of each button connected
%          to the Arduino
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function ButtonMonitor(deltaT)
    global ButtonStates;
	persistent depressedTime;
	global PressDurationForReward;
	
	if ( isempty(depressedTime))
		depressedTime = zeros(1,6);
		PressDurationForReward = 1000;
	end
	
	depressedTime = ButtonStates.*(depressedTime + deltaT);
	
	%Button 2 rewards on channel 1, Button 5 rewards on channel 2
	if ( depressedTime(2) >= PressDurationForReward )
		depressedTime(2) = depressedTime - PressDurationForReward;
		Reward(1,100);
	end
	
	if ( depressedTime(5) >= PressDurationForReward )
		depressedTime(5) = depressedTime - PressDurationForReward;
		Reward(2,100);
	end
	
	
end

