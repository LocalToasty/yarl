cmake_minimum_required(VERSION 3.1)

project(yarl)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 5)
set(VERSION_IDENTIFIER develop)

option(USE_SDL "use SDL as backend of the IOManager" ON)

if(USE_SDL)
	message(STATUS "Using SDL as IOManager backend.")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR})
	set(EXTRA_LIBS ${SDL2_LIBRARY})
	set(IOM_SOURCE "src/iomanager/sdliomanager.cpp")
else(USE_SDL)
	message(STATUS "Using Curses as IOManager backend.")
	find_package(Curses REQUIRED)
	include_directories(${CURSES_INCLUDE_DIR})
	set(EXTRA_LIBS ${CURSES_LIBRARIES})
	set(IOM_SOURCE "src/iomanager/cursesiomanager.cpp")
endif(USE_SDL)

configure_file("${PROJECT_SOURCE_DIR}/yarlconfig.h.in"
			   "${PROJECT_BINARY_DIR}/yarlconfig.h")

include_directories("${PROJECT_BINARY_DIR}")

include_directories("src/"
					"src/game/"
					"src/game/items/"
					"src/game/chars/"
					"src/iomanager/")

add_executable(yarl
			   src/main.cpp
			   src/yarl.cpp
			   src/command.h
			   src/variable.cpp
			   src/statusbar.cpp
			   src/game/world.cpp
			   src/game/entity.cpp
			   src/game/sector.cpp
			   src/game/tile.cpp
			   src/game/attack.cpp
			   src/game/items/item.cpp
			   src/game/items/armor.cpp
			   src/game/items/weapon.cpp
			   src/game/chars/character.cpp
			   src/game/chars/humanoid.cpp
			   src/game/chars/player.cpp
			   src/game/chars/npc.cpp
			   src/game/chars/companion.cpp
			   src/iomanager/iomanager.cpp
			   ${IOM_SOURCE})

set_property(TARGET yarl PROPERTY CXX_STANDARD 11)

#if(CMAKE_COMPILER_IS_GNUCC)
#	set_property(TARGET yarl APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
#endif(CMAKE_COMPILER_IS_GNUCC)


target_link_libraries(yarl ${EXTRA_LIBS})
