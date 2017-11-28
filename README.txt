README
  FAT12 disk utilities developed for CSC360 - Operating Systems. 

INSTALLATION 
  Program requires Linux with 'make' installed. 
  Compile the code by typing 'make' into the command line in the  
  root directory of the project. 

USAGE 
  diskinfo -- displays information about file system 
    usage: diskinfo <disk image> 
  disklist -- displays content of the root directory in the file system 
    usage: disklist <disk image> 
  diskget -- copies file from file system to current directory in Linux 
    usage: diskget <disk image> <disk file> 
  diskput -- copies file from current Linux directory into root directory 
    usage: diskput <disk image> <file> 
