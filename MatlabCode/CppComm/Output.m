%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  Output( rewardChan, rewardDuration )
%
%  Queues reward to the specified channel.
%  Just calls Reward, since the difference is merely semantic.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Output( outputChan, outputDuration )
	Reward(outputChan, outputDuration);
end
