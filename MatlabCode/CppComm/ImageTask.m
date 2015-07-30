%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  EyelinkImageTask.m
%
%  Global Variables:
%    Owned:
%      bQuit - An exposed variable to allow other scripts to declare the end of
%        the task.
%       cFlag - This frame's flag character
%    External:
%
%  Required Functions:
%    Init
%    MessageMonitor
%    SerialInit
%    RegisterUpdate
%    RewardUpdate
%    Reward
%    Looper
%    SendMessage
%    SerialCleanup
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ImageTask
clear all;
%#ok<*MSNU>
global bQuit;
global cFlag;
fprintf('Image Task.\n');

%Sets the period of a time flag (in seconds)
flagPeriod = 6;
%Initial Delay before first trial
initialDelay = 3;

%Turn this on to skip eyelink and use the mouse position instead of gaze coordinates
useMouse = false;

%Eyelink File
edfFile = 'Img0001.edf';

%State Durations
ITIDuration = 0.7;
FixationPeriodDuration = 1.0;
DisplayDuration = 15.0;

FixationDuration = 0.3;

%Delay after last reward before ending experiment
FinalDelay = 10;

%Reward Parameters
rewardDuration = 100; %in milliseconds

%Constants for identifying output channel
rewardChannel = 1;

%Conditions
IMAGE_CATEGORY_1 = 1;
IMAGE_CATEGORY_2 = 2;
IMAGE_CATEGORY_3 = 3;
IMAGE_CATEGORY_4 = 4;
IMAGE_CATEGORY_5 = 5;
IMAGE_CATEGORY_6 = 6;

%Category Name Strings
IMAGE_CATEGORY_NAMES = {};
IMAGE_CATEGORY_NAMES{IMAGE_CATEGORY_1} = 'Category_1';
IMAGE_CATEGORY_NAMES{IMAGE_CATEGORY_2} = 'Category_2';
IMAGE_CATEGORY_NAMES{IMAGE_CATEGORY_3} = 'Category_3';
IMAGE_CATEGORY_NAMES{IMAGE_CATEGORY_4} = 'Category_4';
IMAGE_CATEGORY_NAMES{IMAGE_CATEGORY_5} = 'Category_5';
IMAGE_CATEGORY_NAMES{IMAGE_CATEGORY_6} = 'Category_6';

CATEGORY_NUM = 6;

%Condition Directories
IMAGE_DIRECTORY = {};
IMAGE_DIRECTORY{IMAGE_CATEGORY_1} = 'dir1\';
IMAGE_DIRECTORY{IMAGE_CATEGORY_2} = 'dir2\';
IMAGE_DIRECTORY{IMAGE_CATEGORY_3} = 'dir3\';
IMAGE_DIRECTORY{IMAGE_CATEGORY_4} = 'dir4\';
IMAGE_DIRECTORY{IMAGE_CATEGORY_5} = 'dir5\';
IMAGE_DIRECTORY{IMAGE_CATEGORY_6} = 'dir6\';

%Load image files
IMAGE_FILE = {};

scriptName = mfilename('fullpath');
scriptPath = fileparts(scriptName);

for condition=1:CATEGORY_NUM
	files = dir([fullfile(scriptPath,IMAGE_DIRECTORY{condition}) '*.jpg']);
	IMAGE_FILE{condition} = {};
	
	for fileNum=1:length(files)
		IMAGE_FILE{condition}{fileNum} = files(fileNum).name;
	end
end

%Set up trial template
IMAGES_PER_BLOCK = [];
IMAGES_PER_BLOCK(IMAGE_CATEGORY_1) = 5;
IMAGES_PER_BLOCK(IMAGE_CATEGORY_2) = 5;
IMAGES_PER_BLOCK(IMAGE_CATEGORY_3) = 5;
IMAGES_PER_BLOCK(IMAGE_CATEGORY_4) = 5;
IMAGES_PER_BLOCK(IMAGE_CATEGORY_5) = 5;
IMAGES_PER_BLOCK(IMAGE_CATEGORY_6) = 5;

trialVector = [];
trialVectorTemplate = [];

for condition=1:CATEGORY_NUM
	for count=1:IMAGES_PER_BLOCK(condition)
		trialVectorTemplate = [trialVectorTemplate condition];
	end
end

trialVector = trialVectorTemplate(randperm(length(trialVectorTemplate)));

%State Enums
STATE_ITI = 0;
STATE_FIXATION_PERIOD = 1;
STATE_DISPLAY = 2;
STATE_WAIT_FOR_END = 3;

currentState = STATE_ITI;

commandwindow;
PsychDefaultSetup(1);

cFlag = '';
bQuit = false;
Init();

%These properties can be used to set the Arduino into Reflective mode and Paired Mode
%%Reflective Mode has the Arduino reflect messages back to the sender.
bReflective = false;
%%Paired Mode has the Arduino initiate communication with another Arduino.
bPaired = false;
%Initiate the Serial Device and pass along the Property values.
SerialInit(bReflective,bPaired);

RegisterUpdate ( @MessageMonitor );
RegisterUpdate ( @RewardMonitor );

messages = [1,2,3];
charMessages = ['1','2','3'];
eventOutputChan = [2,3];
nextMessage = 1;

%Set up spacebar to end experiment
stopkey = KbName('space');
%Disable key output to Matlab window
ListenChar(2);

try
	%Initialize Eyelink
	error = eyeTrackingInit(edfFile);
	
	if error
        fprintf('Init failed.\n');
        cleanup();
		return;
	end
	
    %%%%%
    %Set up your Screen here %
    %%%%%
    %Open Graphics Window
    const.screenNumber = max(Screen('Screens'));
	const.bgColor = [0 0 0];
	const.fixColor = [0 0 255];
	
	[window, wRect] = Screen('OpenWindow', const.screenNumber, const.bgColor, [], 32);%#ok<*NASGU>
	const.blankScreen = Screen('OpenOffscreenWindow', const.screenNumber, const.bgColor, [], 32);
	const.screenCenter = round([mean(wRect([1 3])) mean(wRect([2 4]))]);
	Screen('CopyWindow',const.blankScreen,window,wRect,wRect);
	Screen('Flip',window);
	
	const.fixationTargShape = [-48 -48; 48  48];
	const.FixationTarg = shiftPoints(const.fixationTargShape, const.screenCenter)';
	const.FixationTarg = const.FixationTarg(:)';
    
	const.imageTargShape = [-70 -70; 70  70];
	const.ImageTarg = shiftPoints(const.imageTargShape, const.screenCenter)';
	const.ImageTarg = const.ImageTarg(:)';
	
    
    %Record a bit before starting
    WaitSecs(0.1);
    %Send Sync message
	submitDataFlag('SYNCTIME');
  
    %Update the last message time right before we start
    lastFlagTime = toc - flagPeriod;
	progressTime = toc + initialDelay;
    while 1
        %Check for Eyelink Errors
        error = checkRecording();
        if ( error~=0)
            break;
        end
	
        %Call This every loop to update any messages
        Looper();
        
        %Quit if Space was pressed
        [keyIsDown, secs, keyCode] = KbCheck;
        if keyCode(stopkey)
            break;
        end
        
		%We're not using input this experiment
        if ( ~strcmp(cFlag,'') )
            display(cFlag);
        end
        
        %This if block will send your neural data synchronization flags
        if( toc > lastFlagTime + flagPeriod )
            flagMessage = messages(nextMessage);
            %Advance the lastFlagTime, since we're sending a message
            lastFlagTime = lastFlagTime + flagPeriod;
            %Send flagMessage to Arduino HERE
            if ( flagMessage & 1 > 0 )
                Reward(eventOutputChan(1), 50);
            end
            
            if ( flagMessage & 2 > 0 )
                Reward(eventOutputChan(2), 50);
            end
			
			submitDataFlag(charMessages(nextMessage));
            
            %Progress to next message
            nextMessage = mod(nextMessage,3)+1;
        end

        if (currentState==STATE_ITI)
            %Simply Wait
            
            %Progress to next state
            if ( toc > progressTime )
                currentState = STATE_FIXATION_PERIOD;
                progressTime = progressTime + FixationPeriodDuration;
				
                %Display Fixation Point
				Screen('FillRect', window, const.fixColor, const.FixationTarg);
				Screen('Flip', window);
				
				submitDataFlag('FixationDisplayed');
                cumulativeFixation = 0;
				timeLastFrame = toc;
            end
        elseif (currentState==STATE_FIXATION_PERIOD)
            %Wait for fixation or timeout
            
			%Accumulate fixation
			currentTime = toc;
			
			%if new data is ready...
			if (newGazeDataReady())
				[success,x,y] = getGazeCoordinates();
				%And we successfully collect data...
				if (success)
					%And we're in the target...
					if ( x >= const.FixationTarg(1) && x < const.FixationTarg(3) && y >= const.FixationTarg(2) && y < const.FixationTarg(4) )
						%Accumulate fixation time
						cumulativeFixation = cumulativeFixation + (currentTime - timeLastFrame);
					end
				end

				%Only progress the lastFrameTime if we have new eye data
				timeLastFrame = currentTime;
			end
			
			%Escape if fixation duration is sufficient
			if ( cumulativeFixation > FixationDuration )
				currentState = STATE_DISPLAY;
				progressTime = toc + DisplayDuration;
				
				%Hide Fixation Cross
				Screen('CopyWindow', const.blankScreen, window, wRect, wRect);
				
				%Display new Image
				%Grab and clear the new trial condition type
				newTrialCondition = trialVector(1);
				trialVector(1) = [];
				
				%Grab and clear the new image name
				imageIndex = 1 + floor(rand() * length(IMAGE_FILE{newTrialCondition}));
				newImage = [IMAGE_DIRECTORY{newTrialCondition} IMAGE_FILE{newTrialCondition}{imageIndex}];
				IMAGE_FILE{newTrialCondition}(imageIndex) = [];
				
				%Display the image
				imload=imread(newImage,'jpg');
				Screen('PutImage',window,imload,const.ImageTarg);
				Screen('Flip', window);
				
				submitDataFlag(IMAGE_CATEGORY_NAMES{newTrialCondition});
				submitDataFlag(newImage);
			end
			
			%Progress to next state if timed out
            if ( toc > progressTime )
                currentState = STATE_ITI;
                progressTime = progressTime + ITIDuration;
				
                %Clear screen
				Screen('CopyWindow', const.blankScreen, window, wRect, wRect);
				Screen('Flip', window);
            end
        elseif (currentState==STATE_DISPLAY)
            %Simply wait
            
            %Progress to next state
            if ( toc > progressTime )
				if (length(trialVector) == 0)
					currentState = STATE_WAIT_FOR_END;
					progressTime = progressTime + FinalDelay;
				else
					currentState = STATE_ITI;
					progressTime = progressTime + ITIDuration;
				end
                
                %Clear screen
				Screen('CopyWindow', const.blankScreen, window, wRect, wRect);
				Screen('Flip', window);
                
				%Give Reward
				Reward(rewardChannel,rewardDuration);
				
				submitDataFlag('Reward');
            end
		elseif (currentState==STATE_WAIT_FOR_END)
			if ( toc > progressTime )
				break;
			end
        end
        
        cFlag = '';
    end
    ListenChar(0);
	
    shutdownEyelink(edfFile);
    %Clean Up Values and Connections
    cleanup();
catch myerr
    cleanup();
    commandwindow;
    myerr
    myerr.message
    myerr.stack.line
end
end

%Cleanup routine:
function cleanup()
	% finish up: stop recording eye-movements,
	% close graphics window, close data file and shut down tracker
	if (~useMouse)
		Eyelink('Shutdown');
	end

	%Clean up serial connection
	SerialCleanup();

	%Close window:
	sca;

	%Restore keyboard output to Matlab:
	ListenChar(0);
end

%Runs Eyelink Initialization procedures
function error = eyeTrackingInit(edfFile)
	error = false;
	
	if (useMouse)
		return;
	end
	
    %Initialization of the connection, and exit in case of failure
    dummymode=0;
	success = EyelinkInit(dummymode,1);
    if ~success
		error = true;
        return;
    end
	
    Eyelink('command', 'link_event_data = GAZE,GAZERES,HREF,AREA,VELOCITY');
    Eyelink('command', 'link_event_filter = LEFT,RIGHT,FIXATION,BLINK,SACCADE,BUTTON');
	
	
    Eyelink('Openfile', edfFile);
	
    %%%%%
    %Start Recording %
    %%%%%
    Eyelink('StartRecording');
end

%Send messages to the Eyelink
function submitDataFlag(dataFlag)
	if (useMouse)
		return;
	end
	Eyelink('Message',dataFlag);
end

%Returns a boolean indicating whether there is new gaze data available
function dataReady = newGazeDataReady()
	dataReady = true;
	if (useMouse)
		return;
	end
	dataReady = (Eyelink('NewFloatSampleAvailable') > 0);
end

%Returns the gaze coordinates and a bool indicating wether or not the query was successful
function [success, x, y] = getGazeCoordinates()
	persistent eye_used;
	persistent el;
	
	if (~useMouse && isempty(eye_used))
		eye_used = -1;
		
		%Grab default Eyelink values
		el = InitEyelinkDefaults();
	end
	
	success = false;
	x = 0;
	y = 0;
	if (useMouse)
		[x,y] = GetMouse();
		success = true;
		return;
	end
	
	event = Eyelink('NewestFloatSample');
	if (eye_used ~= -1 )
		%Get Eye Coordinates
		x = event.gx(eye_used+1);
		y = event.gx(eye_used+1);
		
		%If we have valid data
		if (x~=el.MISSING_DATA && y~=el.MISSING_DATA && event.pa(eye_used+1)>0)
			%then the data is valid
			success = true;
		end
	else
		eye_used = Eyelink('EyeAvailable');
		if (eye_used == el.BINOCULAR)
			eye_used = el.LEFT_EYE;
		end
		return;
	end
end

%Checks the status of the Eyelink Recording
function error = checkRecording()
	error = false;
	if (useMouse)
		return;
	end
	
	error = Eyelink('CheckRecording');
end

%Close down the Eyelink connection and download the file.
function shutdownEyelink(edfFile)
	if (useMouse)
		return;
	end

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
end

%Slide a matrix of points over by the shift vector
function newPoints = shiftPoints(points, shift)
	% Points should be N-by-2 for N points, shift should be 1-by-2
	%newPoints = bsxfun(@plus,points,shift);
	shift = repmat(shift, size(points,1), 1);
	newPoints = points + shift;
end
