%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  RewardMonitor(deltaT)
%
%  Monitors reward channels.
%  Turns on and off reward at appropriate times.
%    RegisterUpdate ( @RewardMonitor );
%
%  Global Variables:
%    Owned:
%      PendingRewards - Amount of reward time remaining for each channel
%    External:
%      Owner: Init
%        rewardOn - Reward Onset Messages
%        rewardOff - Reward Ending Messages
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function RewardMonitor(deltaT)
	global PendingRewards;
	global rewardOn;
	global rewardOff;
    global rewardPause;
	
	persistent OngoingRewards;
	persistent RewardState;
	
	%Initialize local persistent variables
	if ( isempty(OngoingRewards) )
		OngoingRewards = zeros(1,3);
		RewardState = zeros(1,3);
	end
	
	%Decrement remaining time for rewards turned on
	OngoingRewards = OngoingRewards - RewardState * deltaT;
    
	%Add in new rewards
    i = 1;
	while ( i <= size(PendingRewards,1))
		if ( RewardState(PendingRewards(i,1)) == 0 )
			SendMessage(rewardOn(PendingRewards(i,1)));
			RewardState(PendingRewards(i,1)) = 1;
            OngoingRewards(PendingRewards(i,1)) = PendingRewards(i,2);
            PendingRewards(i,:) = [];
        else
            %Increment i only if you did not just remove an element
            i = i + 1;
		end
	end
	
	%Turn off expired rewards
	for i=1:3 
		if (((RewardState(i)==1) || (RewardState(i)==2)) && (OngoingRewards(i)<=0))
            if (RewardState(i) == 1)
                SendMessage(rewardOff(i));
                RewardState(i) = 2;
				%Since we multiply deltaT by RewardState, we need to double the value of rewardPause for it to work out.
                OngoingRewards(i) = 2*rewardPause;
            elseif (RewardState(i) == 2)
                RewardState(i) = 0;
                OngoingRewards(i) = 0;
            end
		end
	end
end

