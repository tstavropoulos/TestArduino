%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SoundPlayer( rewardChan, rewardDuration )
%
%  Queues sounds for playing alongside rewards.
%
%  Global Variables:
%    Owned:
%      PendingSounds - A list of sounds that have not yet initiated
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SoundPlayer( playNum , pauseDuration)
	global PendingSounds;
    
	%Add the new sound after the end of the list
	PendingSounds(end+1,1) = playNum;
	
	%Add the sound pause duration to the data at the (new) end of the list
	PendingSounds(end,2) = pauseDuration;
end
