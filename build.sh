echo "Starting CMake"
cmake . && echo "Starting Makefile" &&
make && {
    #echo "Open the built program? [y/N] "
    #read choice
    #if [ $choice = "y" ] || [ $choice = "Y" ]
    #then
        chmod +x exec
        ./exec
    #fi
}