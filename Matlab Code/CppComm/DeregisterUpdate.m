%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  DeregisterUpdate( UpdateFunctionHandle )
%
%  Removes the function handle to the list of functions automatically called
%    every frame.
%
%  Global Variables:
%    Owned:
%    External:
%      Owner: RegisterUpdate
%        updateFunctions - A cell array of function handles to be called during
%          each frame's update.
%        updateFunctionNum - The number of currently registered update functions.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function DeregisterUpdate ( functionHandle )
	global updateFunctions;
    global updateFunctionNum;
	
    if ( isempty(updateFunctions) )
		return;
    end
	
	if ( (updateFunctionNum == 1) && (updateFunctions{1} == functionHandle) )
		updateFunctions = {};
		updateFunctionNum = 0;
		return;
	end
	
	for i=1:updateFunctionNum
		if (updateFunctions{i} == functionHandle)
			if ( i == 1 )
				updateFunctions = updateFunctions{2:updateFunctionNum};
			elseif ( i == updateFunctionNum )
				updateFunctions = updateFunctions{1:updateFunctionNum-1};
			else
				updateFunctions = updateFunctions{1:i-1;i+1:updateFunctionNum};
				updateFunctionNum = updateFunctionNum - 1;
			end
			break;
		end
	end
end