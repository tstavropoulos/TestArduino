%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  SoundTest.m
%
%  A Sound Test
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function SoundTest
clear all;
%#ok<*MSNU>
global bQuit;
fprintf('Sound Test.\n');

%%%%%%%
%%IMPORTANT
%%This should be true on one machine and false on the other, to
%%coordinate
%%%%%%
master = true;

%Sets the period of reward batches (in seconds)
rewardPeriod = 10;
%Pause between reward pulses (in milliseconds)
global rewardPause;
rewardPause = 1000;

%Load the audio data
global audioData;
global audioSampleRate;
[audioData,audioSampleRate] = audioread('Click.wav');

%Reward Parameters
smallRewardPulses = 1;
largeRewardPulses = 5;
rewardPulseDuration = 100; %in milliseconds

%Constants for identifying rewards
bothSmallReward = 1;
bothLargeReward = 2;
masterSmallReward = 3;
masterLargeReward = 4;

%Number of rewards per (balanced) block
bothSmallRewardPerBlock = 5;
bothLargeRewardPerBlock = 5;
masterSmallRewardPerBlock = 5;
masterLargeRewardPerBlock = 5;

rewardVector = [];
rewardVectorTemplate = [];

for i=1:bothSmallRewardPerBlock
    rewardVectorTemplate = [rewardVectorTemplate bothSmallReward];
end

for i=1:bothLargeRewardPerBlock
    rewardVectorTemplate = [rewardVectorTemplate bothLargeReward];
end

for i=1:masterSmallRewardPerBlock
    rewardVectorTemplate = [rewardVectorTemplate masterSmallReward];
end

for i=1:masterLargeRewardPerBlock
    rewardVectorTemplate = [rewardVectorTemplate masterLargeReward];
end


rewardVector = rewardVectorTemplate(randperm(length(rewardVectorTemplate)));

commandwindow;
bQuit = false;

Init();

RegisterUpdate ( @SoundMonitor );

%Set up spacebar to end experiment
stopkey = KbName('space');
%Disable key output to Matlab window
ListenChar(2);

try
    fprintf('One computer sound test\n');
    
    %Update the last message time right before we start
    lastRewardTime = toc - rewardPeriod + 2;
    while 1
        %%Call This every loop to update any messages
        Looper();
        
        %%Quit if Space was pressed
        [keyIsDown, secs, keyCode] = KbCheck;
        if keyCode(stopkey)
            break;
        end
        
        %Just the master should send messages
        if ( master==true )
            if ( toc > lastRewardTime + rewardPeriod )
                lastRewardTime = toc;
                
                %%Dispense Reward
                if ( rewardVector(1)==bothLargeReward || rewardVector(1)==masterLargeReward)
					%Master Large Reward
                    fprintf('Large Reward\n');
					
					%%Play largeRewardPulses number of sounds, with a delay between them equal to rewardPause + rewardPulseDuration
					SoundPlayer(largeRewardPulses, rewardPause + rewardPulseDuration);
                else
					%Master Small Reward
                    fprintf('Small Reward\n');
					
					%%Play smallRewardPulses number of sounds, with a delay between them equal to rewardPause + rewardPulseDuration
					SoundPlayer(smallRewardPulses, rewardPause + rewardPulseDuration);
                end
                
                %Remove the most recent trial type
				%%Now the value that was in rewardVector(2) is in rewardVector(1)
                rewardVector(1)=[];
                
                %Replenish the trial types if the vector is depleted
                if (length(rewardVector) == 0 )
                    rewardVector = rewardVectorTemplate(randperm(length(rewardVectorTemplate)));
                end
                
            end
        end
        
    end
    ListenChar(0);
   
    %PsychToolbox Cleanup
    cleanup;
catch myerr
    cleanup;
    commandwindow;
    myerr;
    myerr.message
    myerr.stack.line
end
end


% Cleanup routine:
function cleanup

% Close window:
sca;

% Restore keyboard output to Matlab:
ListenChar(0);
end
