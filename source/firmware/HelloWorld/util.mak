# mkdir.exe: -p = --parents. No error if dir exists
#            -v = --verbose. print a message for each created directory
MKDIR = mkdir.exe -pv
# rm.exe: -r = --recursive. Remove the contents of dirs recursively
#         -v = --verbose. Explain what is being done
#         -f = --force.Ignore nonexistent files, never prompt
#         --no-preserve-root.
RM = rm.exe -rvf --no-preserve-root

export MKDIR RM
