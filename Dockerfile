# ignore this file, i use it to test everything on centos on a non centos computer

FROM centos:7

RUN yum -y install cmake make gcc

WORKDIR /os
COPY . .

WORKDIR /os/build
RUN cmake ..
RUN make
