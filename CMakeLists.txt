cmake_minimum_required (VERSION 3.1)

project (yarl)

find_package (Curses REQUIRED)
include_directories (${CURSES_INCLUDE_DIR})

add_executable (yarl
				src/command.h
				src/iomanager/iomanager.cpp
				src/iomanager/cursesiom.cpp
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

target_link_libraries(yarl ${CURSES_LIBRARIES})
