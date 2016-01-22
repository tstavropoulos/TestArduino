%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%	SynchedTask.m
%
%	Synched Task
%	Use "RunSynchedTask.m" to run this
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function SynchedTask(master,filename)
global bQuit;
global cFlag;
fprintf('Synched Task.\n');

%Track the last time a timestamp was sent
lastTime = 0;
%Sets the period of a time flag (in seconds)
flagPeriod = 2;
%Initial Delay before first reward
initialDelay = 30;
%Pause between reward pulses, in milliseconds
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
bothNoReward = 5;

%Eyelink Messages
rewardMessages = [];
rewardMessages(bothSmallReward) = 'r';		%Both get small reward
rewardMessages(bothLargeReward) = 'R';		%Both get large reward
rewardMessages(masterSmallReward) = 'S';	%Slave gets large reward
rewardMessages(masterLargeReward) = 'M';	%Master gets large reward
rewardMessages(bothNoReward) = 'N';

RewardBlockFinishedMessage = 'Z';
DelayBlockFinishedMessage = 'z';
TaskFinishedMessage = 'Q';

%Reward Cue
beepMessage = 'i';

%Number of rewards per (balanced) block
bothSmallRewardPerBlock = 4;
bothLargeRewardPerBlock = 4;
masterSmallRewardPerBlock = 4;
masterLargeRewardPerBlock = 4;
bothNoRewardPerBlock = 4;

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

for i=1:bothNoRewardPerBlock
	rewardVectorTemplate = [rewardVectorTemplate bothNoReward];
end

rewardVector = rewardVectorTemplate(randperm(length(rewardVectorTemplate)));

%Delay Conditions (in seconds)
DelayCondition1 = 4;
DelayCondition2 = 8;
DelayCondition3 = 12;
DelayCondition4 = 16;

%Number of times each delay
%WARNING: If the total number of delay conditions is not equal to the total number of 
%reward conditions, your counter-balancing might be screwed up.  Currently there are 20 of each per block.
%4 each of 5 reward conditions, and 5 each of 4 delay conditions.
delayConditionEachPerBlock = 5;

delayVector = [];
delayVectorTemplate = [];

for i = 1:delayConditionEachPerBlock
	delayVectorTemplate = [delayVectorTemplate DelayCondition1 DelayCondition2 DelayCondition3 DelayCondition4];
end

delayVector = delayVectorTemplate(randperm(length(delayVectorTemplate)));

%Beep parameters
BeepDelay = 1;

%Session Limits
totalBlocksPerSession = 1;
currentBlock = 0;

%States
STATE_ITI = 1;
STATE_BEEP = 2;
STATE_REWARD_DELAY = 3;

currentState = STATE_ITI;
newState = 0;
progressTime = 0;


commandwindow;

PsychDefaultSetup(1);

cFlag = '';
bQuit = false;

Init();

%These properties can be used to set the Arduino into Reflective mode and Paired Mode
%%Reflective Mode has the Arduino reflect messages back to the sender.
bReflective = false;
%%Paired Mode has the Arduino initiate communication with another Arduino.
bPaired = true;
%Initiate the Serial Device and pass along the Property values.
SerialInit(bReflective,bPaired,master);

RegisterUpdate ( @MessageMonitor );
RegisterUpdate ( @RewardMonitor );
RegisterUpdate ( @SoundMonitor );

messages = ['A','B','C','D','E','F','G','H','I','J'];
nextMessage = 1;

%Set up spacebar to end experiment
stopkey = KbName('space');
%allow you to force quit the connection attempt
forceStop = false;
%Disable key output to Matlab window
ListenChar(2);

%%%%%
% Coordinate Master and Slave here
%%%%%
lastTime = toc;
if ( master == true )
	%Send a connection message
	SendMessage('A');
	%Spin indefinitely, waiting for a slave to connect
	while 1
		%Break out when a slave connects
		if ( SerialDataAvail() )
			fprintf('Heard from Slave.\n');
			%Clear his connection message first
			ReadChar();
			break;
		end
		
		%Send a message every second
		if toc > lastTime + 1
			fprintf('Pinging Slave.\n');
			lastTime = toc;
			SendMessage('A');
		end
		
		[keyIsDown, secs, keyCode] = KbCheck;
		if keyCode(stopkey)
			fprintf('Quit Command Received.\n');
			forceStop = true;
			break;
		end
	end
else
	%Spin indefinitely, waiting for a message from the master
	while 1
		%Break out when a Master sends its message
		if ( SerialDataAvail() )
			fprintf('Heard from Master.\n');
			%Clear the connection message first
			ReadChar();
			%Acknowledge connection from Master
			SendMessage('A');
			break;
		end
		
		if toc > lastTime + 2
			fprintf('Waiting for Master.\n');
			lastTime = toc;
		end
		
		[keyIsDown, secs, keyCode] = KbCheck;
		if keyCode(stopkey)
			fprintf('Quit Command Recieved.\n');
			forceStop = true;
			break;
		end
	end
end

if ( forceStop == true )
	fprintf('Quitting.\n');
	ListenChar(0);
	SerialCleanup();
	cleanup;
	commandwindow;
	return;
end

try
	dummymode=0;
	
	%Open Graphics Window
	%screenNumber = max(Screen('Screens'));
	%window = Screen('OpenWindow',screenNumber);
	
	%Get Eyelink some environment details
	%el = EyelinkInitDefaults(window);
	
	%Initialization of the connection, and exit in case of failure
	if ~EyelinkInit(dummymode,1)
		
		cleanup;
		SerialCleanup();
		return;
	end
	
	%Set up Eyelink to get Gaze data
	%Eyelink('Command','link_sample_data = LEFT,RIGHT,GAZE,AREA');
	Eyelink('command', 'link_event_data = GAZE,GAZERES,HREF,AREA,VELOCITY');
	Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,BLINK,SACCADE,BUTTON');
	
	%Open a file to record data to
	
	if (master)
		edfFile=strcat(filename,'Mas.edf');
	else
		edfFile=strcat(filename,'Sla.edf');
	end
	Eyelink('Openfile', edfFile);
	
	%Calibrate Eyetracker
	%EyelinkDoTrackerSetup(el);
	
	%Final check of calibration using driftcorrection
	%EyelinkDoDriftCorrection(el);
	
	%%%%%
	%Start Recording %
	%%%%%
	Eyelink('StartRecording');
	%Record a bit before starting
	WaitSecs(0.1);
	%Send Sync message
	Eyelink('Message', 'SYNCTIME');
	
	%Update the last message time right before we start
	lastFlagTime = 0;
	lastRewardTime = toc + initialDelay - delayVector(1);
	progressTime = toc + initialDelay;
	while 1
		%Check for Eyelink Errors
		error = Eyelink('CheckRecording');
		if ( error~=0)
			break;
		end
		
		%%Call This every loop to update any messages
		Looper();
		
		%%Quit if Space was pressed
		[keyIsDown, secs, keyCode] = KbCheck;
		if keyCode(stopkey)
			break;
		end
		
		%%Only the slave should be receiving messages
		if ( ~strcmp(cFlag,'') )
			%Play a sounds for Slave
			if (strcmp(cFlag,char(rewardMessages(bothLargeReward))) || strcmp(cFlag,char(rewardMessages(masterSmallReward))))
				%Slave Large Reward
				%Play largeRewardPulses number of sounds, with a delay between them equal to rewardPause + rewardPulseDuration
				SoundPlayer(largeRewardPulses, rewardPause + rewardPulseDuration);
			elseif (strcmp(cFlag,char(rewardMessages(bothSmallReward))) || strcmp(cFlag,char(rewardMessages(masterLargeReward))))
				%Slave Small Reward
				%Play smallRewardPulses number of sounds, with a delay between them equal to rewardPause + rewardPulseDuration
				SoundPlayer(smallRewardPulses, rewardPause + rewardPulseDuration);
			elseif (strcmp(cFlag,char(beepMessage)))
				%Play the reward cue beep
				beep2;
            elseif (strcmp(cFlag,char(TaskFinishedMessage)))
                break;
            end
			%No reward for BothNoReward condition
			
			%%Write cFlag to Data HERE
			Eyelink('Message',cFlag);
		end
		
		%Just the master should send messages
		if ( master==true )
			%%Always want to send flag messages
			if( toc > lastFlagTime + flagPeriod )
				%%Advance the lastTime, since we're sending a message
				lastFlagTime = toc;
				cFlag = messages(nextMessage);
				%%Write cFlag to Data HERE
				Eyelink('Message',cFlag);
				%%Send cFlag to other Arduino HERE
				SendMessage(cFlag);
				%%Progress to next message
				nextMessage = mod(nextMessage,10)+1;
			end
			
			%State Logic
			%Just logic that needs to execute every frame in each state
			if ( currentState == STATE_ITI )
				%If it's time to progress, advance the time by BeepDelay and advance to the Beep state.
				if ( toc > progressTime )
					newState = STATE_BEEP;
				end
			elseif ( currentState == STATE_BEEP )
				if ( toc > progressTime )
					newState = STATE_REWARD_DELAY;
				end
			elseif ( currentState == STATE_REWARD_DELAY )
				if ( toc > progressTime )
					newState = STATE_ITI;
				end
			end
			
			%State Progression Logic
			if ( newState > 0 )
				if ( newState == STATE_ITI )
					newState = 0;
					currentState = STATE_ITI;
					
					%Remove the most recent trial type
					%%Now the value that was in rewardVector(2) is in rewardVector(1)
					rewardVector(1)=[];
					delayVector(1)=[];
					
					%Replenish the trial types if the vector is depleted
					if (length(rewardVector) == 0 )
						disp('Block Finished');
						Eyelink('Message',RewardBlockFinishedMessage);
                        SendMessage(RewardBlockFinishedMessage);
                    	currentBlock = currentBlock + 1;
                        if (currentBlock < totalBlocksPerSession)
                            rewardVector = rewardVectorTemplate(randperm(length(rewardVectorTemplate)));
                        else
                            SendMessage(char(TaskFinishedMessage));
                            break;
                        end
					end
					
					if (length(delayVector) == 0 )
						%disp('Delay Block Finished');
						%Eyelink('Message',DelayBlockFinishedMessage);
						delayVector = delayVectorTemplate(randperm(length(delayVectorTemplate)));
					end
					
					progressTime = toc + delayVector(1);
					
				elseif ( newState == STATE_BEEP )
					newState = 0;
					currentState = STATE_BEEP;
					
					SendMessage(char(beepMessage));
					Eyelink('Message','Beep');
					
					beep2;
					
					progressTime = progressTime + BeepDelay;
				
				elseif ( newState == STATE_REWARD_DELAY )
					newState = 0;
					currentState = STATE_REWARD_DELAY;
				
					Eyelink('Message',char(rewardMessages(rewardVector(1))));
					SendMessage(char(rewardMessages(rewardVector(1))));
					%%Dispense Reward
					if ( rewardVector(1)==bothLargeReward || rewardVector(1)==masterLargeReward)
						%Master Large Reward
						%%Master gets a big reward -> largeRewardPulses number of rewards of rewardPulseDuration duration
						for i=1:largeRewardPulses
							Reward(1, rewardPulseDuration);
						end
						
						%%Play largeRewardPulses number of sounds, with a delay between them equal to rewardPause + rewardPulseDuration
						SoundPlayer(largeRewardPulses, rewardPause + rewardPulseDuration);
					elseif ( rewardVector(1)==bothSmallReward || rewardVector(1)==masterSmallReward)
						%Master Small Reward
						%%Master gets a small reward -> smallRewardPulses number of rewards of rewardPulseDuration duration
						for i=1:smallRewardPulses
							Reward(1, rewardPulseDuration);
						end
						
						%%Play smallRewardPulses number of sounds, with a delay between them equal to rewardPause + rewardPulseDuration
						SoundPlayer(smallRewardPulses, rewardPause + rewardPulseDuration);
					end
					%No reward for BothNoReward condition
					
					%Master Arduino controls the rewards of both Master and Slave
					if ( rewardVector(1)==bothLargeReward || rewardVector(1)==masterSmallReward)
						%Slave gets Large Reward
						for i=1:largeRewardPulses
							Reward(2, rewardPulseDuration);
						end
					elseif ( rewardVector(1)==bothSmallReward || rewardVector(1)==masterLargeReward)
						%Slave gets Small Reward
						for i=1:smallRewardPulses
							Reward(2, rewardPulseDuration);
						end
					end
					%No reward for BothNoReward condition
					
					%Control timing of Progress
					%State should last (largeRewardPulses - 1)*rewardPause + largeRewardPulses*rewardPulseDuration if a large reward is given
					%State should last (smallRewardPulses - 1)*rewardPause + smallRewardPulses*rewardPulseDuration if only small reward is given
					%State should last 0s if no reward is given
					
					if ( rewardVector(1) == bothLargeReward || rewardVector(1) == masterSmallReward || rewardVector(1) == masterLargeReward )
						progressTime = toc + (largeRewardPulses - 1)*rewardPause + largeRewardPulses*rewardPulseDuration;
					elseif ( rewardVector(1) == bothSmallReward )
						progressTime = toc + (smallRewardPulses - 1)*rewardPause + smallRewardPulses*rewardPulseDuration;
					else
						progressTime = toc;
					end
				end
			end
		end
		
		cFlag = '';
	end
	ListenChar(0);
	WaitSecs(0.1);
	
	%Finish up - stop recording Eye Movements
	Eyelink('StopRecording');
	WaitSecs(0.1);
	%Close Data File
	Eyelink('CloseFile');
	WaitSecs(0.1);
	%download data file
	try
		fprintf('Receiving data file ''%s''\n',edfFile);
		%Request the file
		status = Eyelink('ReceiveFile',edfFile,pwd,1);
		if status > 0
			fprintf('ReceiveFile status %d\n', status);
		end
		WaitSecs(0.1);
		if 2==exist(edfFile,'file')
			frpintf('Data file ''%s'' can be found in ''%s''\n',edfFile,pwd);
		end
	catch rdf
		fprintf('Problem receiving data file ''%s''\n',edfFile);
		rdf;
	end
	
	%Serial Cleanup
	SerialCleanup();
	
	%PsychToolbox Cleanup
	cleanup;
catch myerr
	cleanup;
	commandwindow;
	myerr
	myerr.message
	myerr.stack.line
end
end


% Cleanup routine:
function cleanup
% finish up: stop recording eye-movements,
% close graphics window, close data file and shut down tracker
Eyelink('Shutdown');
SerialCleanup();

% Close window:
sca;

% Restore keyboard output to Matlab:
ListenChar(0);
end
