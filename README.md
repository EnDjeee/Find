# Find
A simplified version of bash's find tool written in C
# Compile
1. Position yourself in the src directory
1. make
# Run
## some examples:
```shell-script
- ./1 . -print  
- ./1 . -delete 
- ./1 . -type d -print 
- ./1 . -type f -delete 
- ./1 . -maxdepth 2 -print
- ./1 . -maxdepth 3 -type f -delete
- ./1 . -maxdepth 1 -gtsize 1000 -type d
```
