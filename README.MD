Remote File Share using C and TPS socket connections

- To compile the program please use make on shell to compile all necessary header functions
- To run the server please use ./server
- The client is named after rfs and takes in these command lines ./rfs COMMAND ARG1 ARG2
- COMMAND = WRITE, GET, RM ARG1 and ARG2 are the file directories

- To get the most out of the test cases use the bash scripts provided in the tar
- Please use chmod +x "scripts.sh" and replace scripts with the script files 
- Run the scripts in the following order: writeClient, getServer, writeVersion, removeVersion, sequentialCommand
  - writeClient will write files from data to folder
  - getServer will get files from folder and store it in getFolder
  - writeVersions will write multiple versions of 1 file to versions
  - removeVersions will get rid of all versions in version
  - sequentialCommand will test all commands sequentially
  - Please run make clean after testing all scripts and you can test the command lines manually
  - Please do not remove localfoo, testingOne, testingTwo as they are used for testing purposes for the scripts
