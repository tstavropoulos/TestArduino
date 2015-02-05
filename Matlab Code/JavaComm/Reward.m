%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  Reward( rewardChan, rewardDuration )
%
%  Queues reward to the specified channel.
%
%  Global Variables:
%    Owned:
%      PendingRewards - A list of rewards that have not yet initiated
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Reward( rewardChan, rewardDuration )
	global PendingRewards;

	%Add the new reward after the end of the list
	PendingRewards(end+1,1) = rewardChan;
	
	%add the reward duration to the data at the (new) end of the list
	PendingRewards(end,2) = rewardDuration;
end
