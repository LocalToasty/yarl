cmake_minimum_required(VERSION 3.1)

project(yarl)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)
set(VERSION_IDENTIFIER )

option(USE_SDL "use SDL as backend of the view" ON)

if(USE_SDL)
  message(STATUS "Using SDL as view backend.")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIR})
  set(EXTRA_LIBS ${SDL2_LIBRARY})
  set(VIEW_SOURCE "sdlyarlview")
elseif(NOT USE_SDL)
  message(STATUS "Using Curses as view backend.")
  find_package(Curses REQUIRED)
  include_directories(${CURSES_INCLUDE_DIR})
  set(EXTRA_LIBS ${CURSES_LIBRARIES})
  set(VIEW_SOURCE "cursesyarlview")
endif(USE_SDL)

configure_file("${PROJECT_SOURCE_DIR}/yarlconfig.h.in"
  "${PROJECT_SOURCE_DIR}/src/yarlconfig.h")

include_directories("${PROJECT_BINARY_DIR}")

include_directories(
  "src/"
  "src/view/"
  "src/view/consoleview"
  "src/game/"
  "src/game/items/"
  "src/game/chars/"
  "src/game/events/"
)

add_executable(
  yarl
  src/main.cpp
  src/yarlcontroller.h
  src/yarlcontroller.cpp
  src/command.h
  src/vec.hpp
  src/view/yarlview.h
  src/view/yarlview.cpp
  src/view/yarlviewfactory.h
  src/view/yarlviewfactory.cpp
  src/view/consoleview/consoleyarlview.h
  src/view/consoleview/consoleyarlview.cpp
  src/view/consoleview/${VIEW_SOURCE}.h
  src/view/consoleview/${VIEW_SOURCE}.cpp
  src/view/statusbar.h
  src/view/statusbar.cpp
  src/game/world.h
  src/game/world.cpp
  src/game/entity.h
  src/game/entity.cpp
  src/game/sector.h
  src/game/sector.cpp
  src/game/tile.h
  src/game/tile.cpp
  src/game/attack.h
  src/game/attack.cpp
  src/game/items/item.h
  src/game/items/item.cpp
  src/game/items/armor.h
  src/game/items/armor.cpp
  src/game/items/weapon.h
  src/game/items/weapon.cpp
  src/game/chars/character.h
  src/game/chars/character.cpp
  src/game/chars/humanoid.h
  src/game/chars/humanoid.h
  src/game/chars/humanoid.cpp
  src/game/chars/player.h
  src/game/chars/player.cpp
  src/game/chars/npc.h
  src/game/chars/npc.cpp
  src/game/chars/companion.h
  src/game/chars/companion.cpp
  src/game/events/event.h
  src/game/events/attackevent.h
  src/game/events/deathevent.h
  src/game/events/dropevent.h
)

set_property(TARGET yarl PROPERTY CXX_STANDARD 14)

install(TARGETS yarl RUNTIME DESTINATION bin)

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_BUILD_TYPE EQUAL Debug)
  set_property(TARGET yarl APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
endif(CMAKE_COMPILER_IS_GNUCC AND CMAKE_BUILD_TYPE EQUAL Debug)

target_link_libraries(yarl ${EXTRA_LIBS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")
