%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  ExampleTask.m
%
%  Fake task to demonstrate use of scripts
%
%  Global Variables:
%    Owned:
%      bQuit - An exposed variable to allow other scripts to declare the end of
%        the task.
%    External:
%      Owner: SerialInit
%        serialthing - A handle to the Serial object created to interface with
%          the Arduino.
%
%  Required Functions:
%    Init
%    SerialInit
%    RegisterUpdate
%    Looper
%    SendMessage
%    SerialCleanup
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
global bQuit;
global serialthing;


bQuit = false;

Init();

%These properties can be used to set the Arduino into Reflective mode and Paired Mode
%%Reflective Mode has the Arduino reflect messages back to the sender.
bReflective = false;
%%Paired Mode has the Arduino initiate communication with another Arduino.
bPaired = false;
%Initiate the Serial Device and pass along the Property values.
SerialInit(bReflective,bPaired);

RegisterUpdate ( @ButtonMonitor );
RegisterUpdate ( @RewardMonitor );

count = 0;

while (bQuit == false)
	Looper();
    count=count+1;
	
    if ( count == 50 )
        Reward ( 1, 100 );
    end
	
    if ( count > 10000 )
        bQuit = true;
    end
    
end

SerialCleanup();

