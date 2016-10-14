linux-command-shell
==============

Linux command shell implementation in C
with custom commands as well as system 
commands using process forking in C, 
supports file redirection and pipelining.

Added Features:
	1. Parsing of command may contain white spaces at permitted 
        positions
			Can parse the commands with the use of trim and strtok 
			functions to remove extra spaces

	2. Supports multiple PIPEs and redirectors:   |,>,< 
			Can process commands like 
			cat < file | grep string | sort > file

	3. Supports execution in batch mode
		    If a file is specified as input argument then it executes the
		    file in batch mode, i.e. it prints all tha commands and their 
             corresponding outputs/errors in the screen

	4. Supports commands in mydir and bin folder 
        (preference->mydir->bin)
		    I have first searched in mydir for command implementation
             then the bin folder. If not present the corresponing error is printed.
