%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Function:  Looper()
%
%  Meant to be called every frame, looper calls all registered update functions
%    and passes in the frame's deltaT as an argument.
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
function Looper()
	global updateFunctions;
    global updateFunctionNum;
	
	persistent lastFrameTime;
	if (isempty(lastFrameTime))
	  lastFrameTime=toc*1000;
	end
	
	thisFrameTime = toc*1000;
	deltaT = thisFrameTime - lastFrameTime;
    
	if (updateFunctionNum>0)
		for i=1:updateFunctionNum
            feval(updateFunctions{i},deltaT);
		end
	end
	
	lastFrameTime=thisFrameTime;
end
