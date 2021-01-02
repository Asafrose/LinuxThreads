cmake_minimum_required(VERSION 3.16)
project(LinuxThreads C)

set(CMAKE_C_STANDARD 99)

find_library(mta_rand REQUIRED)
find_library(mta_crypt REQUIRED)
find_library(pthread REQUIRED)
add_executable(LinuxThreads main.c ConcurrentQueue.c ConcurrentQueue.h Encrypter.c Encrypter.h Decrypter.c Decrypter.h Common.c Common.h)
target_link_libraries(LinuxThreads mta_rand mta_crypt pthread)