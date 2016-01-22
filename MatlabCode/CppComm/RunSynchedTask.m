%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  RunSynchedTask.m
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function RunSynchedTask
	clear all;
	%%%%%%%
	%%IMPORTANT
	%%This should be true on one machine and false on the other, to
	%%coordinate
	%%%%%%
	master = true;
	
	%The filename will automatically get the Master or Slave suffix
	%Must be no more than 5 characters long
	%Blame Eyelink
	filename = 'Syn00';
	SynchedTask(master,filename);
end