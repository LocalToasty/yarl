cmake_minimum_required (VERSION 3.1)

project (yarl)

find_package (Curses REQUIRED)
include_directories (${CURSES_INCLUDE_DIR})

add_executable (yarl
				src/main.cpp
				src/globals.h
				src/yarl.h
				src/yarl.cpp
				src/entity.h
				src/entity.cpp
				src/character.h
				src/character.cpp
				src/sector.h
				src/sector.cpp
				src/tile.h
				src/tile.cpp)

set_property (TARGET yarl PROPERTY CXX_STANDARD 11)

target_link_libraries(yarl ${CURSES_LIBRARIES})
