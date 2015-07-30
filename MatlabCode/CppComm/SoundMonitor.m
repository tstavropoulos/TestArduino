%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  SoundMonitor(deltaT)
%
%  Monitors sound requests.
%  Plays Sounds at appropriate times.
%    RegisterUpdate ( @SoundMonitor );
%
%  Global Variables:
%    Owned:
%      PendingSounds - Remaining Plays and amount of pause between each play
%    External:
%      Owner: Init
%        audioData - Audio Data
%        audioSampleRate - Audio Data
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function SoundMonitor(deltaT)
	global PendingSounds;
	global audioData;
	global audioSampleRate;
	
	persistent SoundDelay;
	persistent SoundState;
	
	%Initialize local persistent variables
	if isempty(SoundDelay)
		SoundDelay = 0;
		SoundState = 0;
	end
	
	%Decrement remaining time for sound delay
	SoundDelay = SoundDelay - SoundState * deltaT;
	
	%Add in new Sounds
	if (~isempty(PendingSounds))
		if ( SoundState == 0 )
			sound(audioData,audioSampleRate);
			SoundState = 1;
            SoundDelay = PendingSounds(1,2);
			PendingSounds(1,1) = PendingSounds(1,1)-1;
			
			%Remove pending request if no more plays
			if (PendingSounds(1,1) == 0)
				PendingSounds(1,:) = [];
			end
		end
	end
	
	%Play next sound if delay elapsed
	if ((SoundState == 1) && (SoundDelay <= 0))
		SoundState = 0;
		SoundDelay = 0;
	end
end

