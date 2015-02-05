%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  RegisterUpdate( UpdateFunctionHandle )
%
%  Adds the function handle to the list of functions automatically called every
%    frame.
%
%  Global Variables:
%    Owned:
%      updateFunctions - A cell array of function handles to be called during
%        each frame's update.
%      updateFunctionNum - The number of currently registered update functions.
%    External:
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function RegisterUpdate ( newUpdateFunction )
	global updateFunctions;
    global updateFunctionNum;
	
	%Since this global has limited scope, we handle initialization here.
    if ( isempty(updateFunctions) )
        updateFunctions = {};
        updateFunctionNum = 0;
    end
	
	for i=1:updateFunctionNum
		%Don't add the same function twice.
		if (strcmp(func2str(updateFunctions{i}),func2str(newUpdateFunction)))
			return;
		end
	end
	
    updateFunctionNum = updateFunctionNum + 1;
	updateFunctions{updateFunctionNum} = newUpdateFunction;
end