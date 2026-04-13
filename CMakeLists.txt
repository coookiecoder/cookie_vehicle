cmake_minimum_required(VERSION 3.14)

project(cookie_vehicle VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

message(STATUS "Finding Curses...")
find_package(Curses REQUIRED)

message(STATUS "Fetching Dependencies...")
include(FetchContent)

# --- cookie library ---
FetchContent_Declare(cookie GIT_REPOSITORY https://gitlab.cookie-coder.com/cookiecoder/cookie.git)
FetchContent_MakeAvailable(cookie)

set(SOURCES_CAR src/car.cpp)
set(HEADERS_CAR)

set(SOURCES_TRUCK src/truck.cpp)
set(HEADERS_TRUCK)

set(VEHICLE_DEP class/vehicle.cpp class/vehicle.hpp)

set(SOURCES_CENTRAL src/central.cpp)
set(HEADERS_CENTRAL)

set(SOURCES_CENTRAL_COMPUTER src/central_computer.cpp)
set(HEADERS_CENTRAL_COMPUTER)

add_executable(cookie_car ${SOURCES_CAR} ${HEADERS_CAR} ${VEHICLE_DEP})
add_executable(cookie_truck ${SOURCES_TRUCK} ${HEADERS_TRUCK} ${VEHICLE_DEP})
add_executable(cookie_central ${SOURCES_CENTRAL} ${HEADERS_CENTRAL})
add_executable(cookie_central_computer ${SOURCES_CENTRAL_COMPUTER} ${HEADERS_CENTRAL_COMPUTER})

target_link_libraries(cookie_car PRIVATE cookie -static-libstdc++ -static-libgcc)
target_compile_options(cookie_car PRIVATE -Os)

target_link_libraries(cookie_truck PRIVATE cookie -static-libstdc++ -static-libgcc)
target_compile_options(cookie_truck PRIVATE -Os)

target_link_libraries(cookie_central PRIVATE cookie -static-libstdc++ -static-libgcc)
target_compile_options(cookie_central PRIVATE -Os)

target_link_libraries(cookie_central_computer PRIVATE cookie -static-libstdc++ -static-libgcc)
target_compile_options(cookie_central_computer PRIVATE -Os)
target_include_directories(cookie_central_computer PUBLIC include ${CURSES_INCLUDE_DIRS})
target_link_libraries(cookie_central_computer PRIVATE ${CURSES_LIBRARIES})

set(DOCKER_IMAGE_NAME "cookie_vehicle")
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/docker_output")

add_custom_target(docker-build-armv6
    # 1. Build the image
    COMMAND docker build -t ${DOCKER_IMAGE_NAME} -f ./armv6/Dockerfile .

    # 2. Create the output directory on the host if it doesn't exist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}

    # 3. Create a temporary container, copy the files, and remove the container
    # We assume your binaries are built in '/app/build' inside the container
    COMMAND docker run --name cookie_temp ${DOCKER_IMAGE_NAME} /bin/true
    COMMAND docker cp cookie_temp:./build/. ${OUTPUT_DIR}
    COMMAND docker rm cookie_temp

    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building inside docker and extracting artifacts to docker_output"
)

add_custom_target(docker-build-aarch64
    # 1. Build the image
    COMMAND docker build -t ${DOCKER_IMAGE_NAME} -f ./aarch64/Dockerfile .

    # 2. Create the output directory on the host if it doesn't exist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}

    # 3. Create a temporary container, copy the files, and remove the container
    # We assume your binaries are built in '/app/build' inside the container
    COMMAND docker run --name cookie_temp ${DOCKER_IMAGE_NAME} /bin/true
    COMMAND docker cp cookie_temp:./build/. ${OUTPUT_DIR}
    COMMAND docker rm cookie_temp

    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building inside docker and extracting artifacts to docker_output"
)
