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
	while ( ~isempty(PendingRewards) )
		if ( RewardState(PendingRewards(1,1)) == 0 )
			SendMessage(rewardOn(PendingRewards(1,1)));
			RewardState(PendingRewards(1,1)) = 1;
            %disp(['Starting Reward ',num2str(PendingRewards(1,1))]);
		end
		OngoingRewards(PendingRewards(1,1)) = OngoingRewards(PendingRewards(1,1)) + PendingRewards(1,2);
		PendingRewards = PendingRewards(2:end,:);
	end
	
	%Turn off expired rewards
	for i=1:3 
		if ((RewardState(i)==1) && (OngoingRewards(i)<=0))
			SendMessage(rewardOff(i));
            %disp(['Ending Reward ',num2str(i)]);
			RewardState(i) = 0;
			OngoingRewards(i) = 0;
		end
	end
end

