cmake_minimum_required (VERSION 3.1)

project (yarl)

set (YARL_VERSION_MAJOR 0)
set (YARL_VERSION_MINOR 0)

option (USE_SDL "use SDL as backend of the IOManager" ON)

if (USE_SDL)
	message (STATUS "Using SDL as IOManager backend.")
	include (FindPkgConfig)
	pkg_search_module (SDL2 REQUIRED sdl2)
	include_directories (${SDL2_INCLUDE_DIRS})
	set (EXTRA_LIBS ${SDL2_LIBRARIES})
	set (IOM_SOURCE "src/iomanager/sdliom.cpp")
else (USE_SDL)
	message (STATUS "Using Curses as IOManager backend.")
	find_package (Curses REQUIRED)
	include_directories (${CURSES_INCLUDE_DIR})
	set (EXTRA_LIBS ${CURSES_LIBRARIES})
	set (IOM_SOURCE "src/iomanager/cursesiom.cpp")
endif (USE_SDL)

configure_file ("${PROJECT_SOURCE_DIR}/yarlconfig.h.in"
				"${PROJECT_BINARY_DIR}/yarlconfig.h")

include_directories ("${PROJECT_BINARY_DIR}")

add_executable (yarl
				src/command.h
				src/iomanager/iomanager.cpp
				${IOM_SOURCE}
				src/main.cpp
				src/yarl.cpp
				src/entity.cpp
				src/item.cpp
				src/character.cpp
				src/sector.cpp
				src/tile.cpp
				src/variable.cpp
				src/statusBar.cpp
				src/generator.cpp)

set_property (TARGET yarl PROPERTY CXX_STANDARD 11)

target_link_libraries(yarl ${EXTRA_LIBS})
