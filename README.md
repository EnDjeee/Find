# Find
A simplified version of bash's find tool written in C
# Compile
- Position yourself in the src directory
- make
# Run
## some examples:
```
- ./1 . -print  //prints every files in current directory
- ./1 . -delete //deletes every files in current directory
- ./1 . -type -d -print //prints every directories in current directory
- ./1 . -type -f -delete //deletes every regular files in current directory
```
