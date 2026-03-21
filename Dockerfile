FROM muslcc/x86_64:armv6-linux-musleabihf

RUN apk update
RUN apk upgrade
RUN apk add cmake make git

COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./src ./src

RUN cmake . -B build
RUN cmake --build build
