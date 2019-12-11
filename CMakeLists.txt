cmake_minimum_required(VERSION 3.8)

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

#html c
add_executable(server 1b/html/requestHandling/sharedTools.c 1b/html/requestHandling/sharedTools.h 1b/html/server/server.c)
target_compile_options(server PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

add_executable(client 1b/html/requestHandling/sharedTools.c 1b/html/requestHandling/sharedTools.h 1b/html/client/client.c)
target_compile_options(client PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)