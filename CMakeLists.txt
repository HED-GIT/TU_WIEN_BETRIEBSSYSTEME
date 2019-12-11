cmake_minimum_required(VERSION 3.8)

project(projects LANGUAGES C CXX)

SET(CMAKE_C_COMPILER gcc)

#ispalindrom c

add_executable(ispalindrom ispalindrom/ispalindrom.c)
target_compile_options(ispalindrom PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#mygrep c

add_executable(mygrep mygrep/mygrep.c)
target_compile_options(mygrep PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#mycompress c

add_executable(mycompress mycompress/mycompress.c)
target_compile_options(mycompress PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#ispalindrom c

add_executable(mydiff mydiff/mydiff.c)
target_compile_options(mydiff PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

#html c
add_executable(server html/requestHandling/sharedTools.c html/requestHandling/sharedTools.h html/server/server.c)
target_compile_options(server PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)

add_executable(client html/requestHandling/sharedTools.c html/requestHandling/sharedTools.h html/client/client.c)
target_compile_options(client PUBLIC -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L)