Run the commands
	make -- to compile.
	./shell -- to run the shell

The built in commands like ls etc. can be run.

External commands that can be run are:
	cd
	pwd
	echo

Some user-defined commands can be run:
	pinfo : prints the process related info of your shell

	pinfo <pid> : prints the process info about given pid.
	
	jobs : prints a list of all currently running jobs along with their pid, particularly background jobs, in order of their creation.
	
	kjob <jobNumber> <signalNumber>: takes the job id of a running job and sends a signal value to that process
	
	fg <jobNumber>: brings background job with given job number to foreground.
	overkill: kills all background process at once.
	
	quit : exits the shell. It doesnot quit on any other signals from user like : CTRL-D, CTRL-C, SIGINT, SIGCHLD etc.
	
	CTRL-Z : It changes the status of currently running job to stop, and push it in background process.



It has some features like:
	displaying appropriate messages based on the exit status.
	input-output redirection functionality has been implemented.
	command redirections using '|'
	Using both '|' and '>/<'
	A process can be run in background by using "&" argument.


