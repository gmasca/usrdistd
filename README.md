################################### userdistd #####################################

###################################################################################
 run from the project directory:
  1. $ make
  2. $ userdistd
  3. $ source jsimplement/autopach.sh
###################################################################################

###################################################################################
Usage: usrdistd [options]
Options:
  -u {user}         Set {user}.conf file to use.
  -m {mode}         Select frame mode: 0(default) -frame, 1 - video.
  -t                Debug mode, (no http server only video).
  -p {port}         Select custom port (9600 default) for transmission.
  -h                Show this help message.

  start             Start the program as daemon.
  stop              Interrupt eventually running instance.
###################################################################################

###################################################################################
 dependencies:
  - opencv
  - libtbb
  - libGL
  - libc
###################################################################################