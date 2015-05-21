################################### userdistd #####################################

###################################################################################
 run from the project directory:
  1. $ make
  2. $ userdistd
  3. $ source jsimplement/autopach.sh
###################################################################################

###################################################################################
 options:
 	-u [name] - select a custom user for the configuration file
 	-p [port] - allows to select a custom port in which the daemon
 	            can receives requests
 	-t        - enables the test mode to show the frame analysis
###################################################################################

###################################################################################
 dependencies:
  - opencv
  - libtbb
  - libGL
  - libc
###################################################################################



