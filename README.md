# TU_WIEN_BETRIEBSSYSTEME

## solutions

- 1a: file read-write
  - binary-digits
  - ispalindrom
  - mycompress
  - mydiff
  - myexpand
  - mygrep
- 1b: sockets
  - html
- 2: fork/execlp
  - forkFFT
  - forkSort
  - intmul
- 3: shared memory
  - 3color
  - fb_arc_set

## todos

- memory leaks
  - check for memory leaks in 1b
    - client
    - server
  - check for memory leaks in 2
    - intmul
  - check for memory leaks in 3
    - 3color
      - generator
      - supervisor
    - fb_arc_set
      - generator
      - supervisor
- other
  - forkfft
    - check why so many frees are used
    - most likely cause of getline, get away from it and pass input without
  - mydiff switch away from getline
  - check for minor bugs
