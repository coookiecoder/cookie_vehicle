FROM muslcc/x86_64:aarch64-linux-musl

RUN apk update
RUN apk upgrade
RUN apk add cmake make git ncurses-dev ncurses-static

COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./src ./src
COPY ./class ./class

RUN mkdir /mnt/mmcblk0p1/

COPY ./vehicle.txt /mnt/mmcblk0p1/vehicle.txt

RUN cmake . -B build
RUN cmake --build build --target cookie
RUN cmake --build build --target cookie
RUN cmake --build build --target cookie_car
RUN cmake --build build --target cookie_truck
RUN cmake --build build --target cookie_central
