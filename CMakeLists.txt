cmake_minimum_required (VERSION 3.1)

project (yarl)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 0)
set (VERSION_PATCH 0)

option (USE_SDL "use SDL as backend of the IOManager" ON)

if (USE_SDL)
	message (STATUS "Using SDL as IOManager backend.")
	include (FindPkgConfig)
	pkg_search_module (SDL2 REQUIRED sdl2)
	include_directories (${SDL2_INCLUDE_DIRS})
	set (EXTRA_LIBS ${SDL2_LIBRARIES})
	set (IOM_SOURCE "src/iomanager/sdliomanager.cpp")
else (USE_SDL)
	message (STATUS "Using Curses as IOManager backend.")
	find_package (Curses REQUIRED)
	include_directories (${CURSES_INCLUDE_DIR})
	set (EXTRA_LIBS ${CURSES_LIBRARIES})
	set (IOM_SOURCE "src/iomanager/cursesiomanager.cpp")
endif (USE_SDL)

configure_file ("${PROJECT_SOURCE_DIR}/yarlconfig.h.in"
				"${PROJECT_BINARY_DIR}/yarlconfig.h")

include_directories ("${PROJECT_BINARY_DIR}")

include_directories ("src/"
					 "src/gameplay/"
					 "src/gameplay/npcs"
					 "src/iomanager/")

add_executable (yarl
				src/main.cpp
				src/yarl.cpp
				src/command.h
				src/variable.cpp
				src/statusbar.cpp
				src/gameplay/world.cpp
				src/gameplay/entity.cpp
				src/gameplay/item.cpp
				src/gameplay/character.cpp
				src/gameplay/sector.cpp
				src/gameplay/tile.cpp
				src/gameplay/npcs/npc.cpp
				src/gameplay/npcs/dog.cpp
				src/iomanager/iomanager.cpp
				${IOM_SOURCE})

set_property (TARGET yarl PROPERTY CXX_STANDARD 11)

target_link_libraries(yarl ${EXTRA_LIBS})
