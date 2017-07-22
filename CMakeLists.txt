cmake_minimum_required(VERSION 3.6)
project(vfs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -static)
set(SOURCE_FILES main.cpp user.cpp
        format.cpp command.cpp dir.cpp
        Alloc_Free_data_block.cpp Alloc_Free_inode.cpp
        Loaddisk.cpp Savedisk.cpp filesys.h Open_Close_file.cpp ShowInfo.cpp)
add_executable(vfs ${SOURCE_FILES})