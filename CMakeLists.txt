cmake_minimum_required(VERSION 3.16)
project(LastEcho)

set(CMAKE_CXX_STANDARD 17)

set(SFML_STATIC_LIBRARIES FALSE)
set(SFML_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(LastEcho
        src/main.cpp
        src/player.cpp
        src/MainMenu.cpp
        src/settings.h
        src/settings.cpp
        "src/audio.h"
        "src/audio.cpp"
        src/GameMap.cpp
        src/GameMap.h
        src/game.h
        src/game.cpp
        src/healthbar.cpp   # <--- ضيف السطر ده هنا
        src/healthbar.h
        src/XPbar.cpp
        src/XPBar.h
        src/inventory.cpp
        src/inventory.h
        src/NPC.h
        src/NPC.cpp
        src/enemies.cpp
        src/enemies.h
        src/DialogueManager.h
        src/DialogueManager.cpp     # ويفضل تضيف الهيدر كمان للتنظيم
        src/world.h
        src/world.cpp
        src/phase.h
        src/phase.cpp
        src/BTDminigame.cpp
        src/BTDminigame.h
        # ويفضل تضيف الهيدر كمان للتنظيم

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

# Copy assets
add_custom_command(TARGET LastEcho POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:LastEcho>/assets"
)