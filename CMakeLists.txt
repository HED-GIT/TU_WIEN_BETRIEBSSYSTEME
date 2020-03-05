cmake_minimum_required(VERSION 3.10.2)

project(projects LANGUAGES C CXX)

SET(CMAKE_C_COMPILER gcc)

#ispalindrom c

add_executable(ispalindrom 1a/ispalindrom/ispalindrom.c)
target_compile_options(ispalindrom PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#mygrep c

add_executable(mygrep 1a/mygrep/mygrep.c)
target_compile_options(mygrep PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#mycompress c

add_executable(mycompress 1a/mycompress/mycompress.c)
target_compile_options(mycompress PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#ispalindrom c

add_executable(mydiff 1a/mydiff/mydiff.c)
target_compile_options(mydiff PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#myexpand c

add_executable(myexpand 1a/myexpand/myexpand.c)
target_compile_options(myexpand PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#html c
add_executable(server 1b/html/requestHandling/sharedTools.c 1b/html/requestHandling/sharedTools.h 1b/html/server/server.c)
target_compile_options(server PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

add_executable(client 1b/html/requestHandling/sharedTools.c 1b/html/requestHandling/sharedTools.h 1b/html/client/client.c)
target_compile_options(client PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#intmul c

add_executable(intmul 2/intmul/intmul.c)
target_compile_options(intmul PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#forkFFT c

add_executable(forkFFT 2/forkFFT/forkFFT.c)
target_compile_options(forkFFT PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)
target_link_libraries(forkFFT m)

#forkSort c

add_executable(forkSort 2/forkSort/forkSort.c)
target_compile_options(forkSort PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#fb_arc_set c

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fb_arc_set)
add_executable(fb_generator 3/fb_arc_set/generator.c 3/fb_arc_set/circularBuffer.c)
target_link_libraries(fb_generator pthread rt)
target_compile_options(fb_generator PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

add_executable(fb_supervisor 3/fb_arc_set/supervisor.c 3/fb_arc_set/circularBuffer.c)
target_link_libraries(fb_supervisor pthread rt)
target_compile_options(fb_supervisor PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#3color c

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/3color)
add_executable(3color_generator 3/3color/generator.c 3/3color/circularBuffer.c)
target_link_libraries(3color_generator pthread rt)
target_compile_options(3color_generator PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

add_executable(3color_supervisor 3/3color/supervisor.c 3/3color/circularBuffer.c)
target_link_libraries(3color_supervisor pthread rt)
target_compile_options(3color_supervisor PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)
