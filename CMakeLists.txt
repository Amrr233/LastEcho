cmake_minimum_required(VERSION 3.16)
project(LastEcho)

set(CMAKE_CXX_STANDARD 17)

set(SFML_STATIC_LIBRARIES FALSE)
set(SFML_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(LastEcho
        src/main.cpp
        src/player.cpp
        src/player.h
        src/Data.h
)

target_include_directories(LastEcho PRIVATE
        src/
        include/
)

target_link_libraries(LastEcho
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

if(WIN32)
    add_custom_command(TARGET LastEcho POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/lib/bin"
            $<TARGET_FILE_DIR:LastEcho>
    )
endif()