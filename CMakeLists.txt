# ignore this file, it exists so that i can build all programs from one file instead of using a different make for every program

cmake_minimum_required(VERSION 3.10.2)

project(projects LANGUAGES C CXX)

SET(CMAKE_C_COMPILER gcc)
add_compile_options(-std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#ispalindrom c

add_executable(ispalindrom 1a/ispalindrom/ispalindrom.c)

#mygrep c

add_executable(mygrep 1a/mygrep/mygrep.c)

#mycompress c

add_executable(mycompress 1a/mycompress/mycompress.c)

#ispalindrom c

add_executable(mydiff 1a/mydiff/mydiff.c)

#myexpand c

add_executable(myexpand 1a/myexpand/myexpand.c)

#binary-digits c

add_executable(binary-digits 1a/binary-digits/binary-digits.c)

#html c
add_executable(server 1b/html/sharedTools.c 1b/html/server.c)
	
add_executable(client 1b/html/sharedTools.c 1b/html/client.c)

#intmul c

add_executable(intmul 2/intmul/intmul.c)

#forkFFT c

add_executable(forkFFT 2/forkFFT/forkFFT.c)
target_link_libraries(forkFFT m)

#forkSort c

add_executable(forkSort 2/forkSort/forkSort.c)

#fb_arc_set c

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fb_arc_set)
add_executable(fb_generator 3/fb_arc_set/generator.c 3/fb_arc_set/circularBuffer.c)
target_link_libraries(fb_generator pthread rt)

add_executable(fb_supervisor 3/fb_arc_set/supervisor.c 3/fb_arc_set/circularBuffer.c)
target_link_libraries(fb_supervisor pthread rt)

#3color c

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/3color)
add_executable(3color_generator 3/3color/generator.c 3/3color/circularBuffer.c)
target_link_libraries(3color_generator pthread rt)

add_executable(3color_supervisor 3/3color/supervisor.c 3/3color/circularBuffer.c)
target_link_libraries(3color_supervisor pthread rt)
