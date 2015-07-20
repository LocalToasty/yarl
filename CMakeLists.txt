cmake_minimum_required(VERSION 3.1)

project(yarl)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
set(VERSION_IDENTIFIER develop)

option(USE_SDL "use SDL as backend of the IOManager" ON)

if(USE_SDL)
	message(STATUS "Using SDL as IOManager backend.")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR})
	set(EXTRA_LIBS ${SDL2_LIBRARY})
	set(IOM_SOURCE "src/iomanager/sdliomanager")
elseif(NOT USE_SDL)
	message(STATUS "Using Curses as IOManager backend.")
	find_package(Curses REQUIRED)
	include_directories(${CURSES_INCLUDE_DIR})
	set(EXTRA_LIBS ${CURSES_LIBRARIES})
	set(IOM_SOURCE "src/iomanager/cursesiomanager")
endif(USE_SDL)

configure_file("${PROJECT_SOURCE_DIR}/yarlconfig.h.in"
	"${PROJECT_SOURCE_DIR}/src/yarlconfig.h")

include_directories("${PROJECT_BINARY_DIR}")

include_directories("src/"
	"src/game/"
	"src/game/items/"
	"src/game/chars/"
	"src/iomanager/")

add_executable(yarl
	src/main.cpp
	src/yarl.h
	src/yarl.cpp
	src/command.h
	src/variable.h
	src/variable.cpp
	src/statusbar.h
	src/statusbar.cpp
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
	src/iomanager/iomanager.h
	src/iomanager/iomanager.cpp
	${IOM_SOURCE}.h
	${IOM_SOURCE}.cpp)

set_property(TARGET yarl PROPERTY CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_BUILD_TYPE EQUAL Debug)
	set_property(TARGET yarl APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
endif(CMAKE_COMPILER_IS_GNUCC AND CMAKE_BUILD_TYPE EQUAL Debug)


target_link_libraries(yarl ${EXTRA_LIBS})
