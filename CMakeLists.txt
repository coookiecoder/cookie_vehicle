cmake_minimum_required(VERSION 3.14)

project(cookie_vehicle VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

message(STATUS "Fetching Dependencies...")

# --- cookie library ---
FetchContent_Declare(cookie GIT_REPOSITORY https://gitlab.cookie-coder.com/cookiecoder/cookie.git)
FetchContent_MakeAvailable(cookie)

set(SOURCES_CAR src/car.cpp)
set(HEADERS_CAR)

set(SOURCES_TRUCK src/truck.cpp)
set(HEADERS_TRUCK)

set(VEHICLE_DEP class/vehicle.cpp class/vehicle.hpp)

add_executable(cookie_car ${SOURCES_CAR} ${HEADERS_CAR} ${VEHICLE_DEP})
add_executable(cookie_truck ${SOURCES_TRUCK} ${HEADERS_TRUCK} ${VEHICLE_DEP})

target_link_libraries(cookie_car PRIVATE cookie -static-libstdc++ -static-libgcc)
target_compile_options(cookie_car PRIVATE -Os)

target_link_libraries(cookie_truck PRIVATE cookie -static-libstdc++ -static-libgcc)
target_compile_options(cookie_truck PRIVATE -Os)

set(DOCKER_IMAGE_NAME "cookie_vehicle")
add_custom_target(docker-build COMMAND docker build -t ${DOCKER_IMAGE_NAME} ${CMAKE_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMENT "Building inside docker using alpine ARMHF target")
