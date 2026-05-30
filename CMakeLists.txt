cmake_minimum_required(VERSION 3.14)
project(robot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(HTTPLIB_REQUIRE_OPENSSL OFF)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.3
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(httplib nlohmann_json)

if (WIN32)
    add_compile_definitions(
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
        _WIN32_WINNT_WIN10=0x0A00
    )
endif()

# Добавляем оба исходника: C++ и C
add_executable(robot
    main.cpp
    robot_gpio.c
)

target_link_libraries(robot PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    pthread
)
target_link_options(robot PRIVATE -static)