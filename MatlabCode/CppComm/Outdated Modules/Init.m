%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  Init()
%
%  Declares and initializes a number of variables.
%
%  Global Variables:
%    Owned:
%      ButtonStates - An array of the current state of each button connected
%        to the Arduino
%      ButtonReverseLookup - A hashtable of the button messages mapped onto an
%        array containing the corresponding button number and state.
%      rewardOn - The signals to send to initiate rewards
%      rewardOff - The signals to send to end rewards
%      bCallback - A boolean to disable the callback (when handling messages
%        manually)
%      bConnected - A boolean to indicate when the arduino is connected.
%      wSerialInitChar - The character sent to the Arduino to start the program
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Init()
	global ButtonStates;
	ButtonStates = zeros(1,6);
	
	global rewardOn;
	global rewardOff;
	rewardOn = ['1','2','3'];
	rewardOff = ['4','5', '6'];
    
    global bConnected;
    bConnected = false;
	
	%ButtonReverseLookup is a Hash Table
	% The incoming character value is mapped onto a vector of [UpOrDown,ButtonNumber]
	% Examples:
	%  ButtonReverseLookup('A') returns [1,1]
	%  ButtonReverseLookup('a') returns [0,1]
	%  ButtonReverseLookup('D') returns [1,4]
	% Using a Hash Table for this one-way lookup (we will always receive the messages
	%   and then need to find out what it means, not the reverse) is much more efficient
	%   than searching both vectors for it.
	% You store objects in a Hash Table, but rather than accessing them by an index,
	%   like a normal array or matrix, you access the objects by a "key", which is a
	%   string you pass in.
	% Technically what it's actually doing is generating a unique number from the string
	%   in a consistent way, and mapping that onto memory.
	global ButtonReverseLookup;
	upStates = ['a', 'b', 'c', 'd', 'e', 'f'];
	downStates = ['A', 'B', 'C', 'D', 'E', 'F'];
	keys=num2cell([upStates,downStates]);
	values=num2cell([[zeros(6,1);ones(6,1)] [1:6 1:6]'],2);
	ButtonReverseLookup = containers.Map(keys,values);
	
	global bCallback;
	bCallback = false;
	
	%Update this value with non-backwards-compatible version upgrades
	global wSerialInitChar;
	wSerialInitChar='C';
	
	%Number of milliseconds for a "WaitForMessage" call to timeout
	global iWaitTimeOut;
    iWaitTimeOut = 5000;
	
	%Default size of Serial read buffer.
	global iReadBufferSize;
    iReadBufferSize = 100;
	
	%Minimum delay between reward pulses (in milliseconds)
	global rewardPause;
	rewardPause = 1000;
	
	%Kick off the timer.
	tic;
end