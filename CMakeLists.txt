cmake_minimum_required (VERSION 3.1)

project (yarl)

find_package (Curses REQUIRED)
include_directories (${CURSES_INCLUDE_DIR})

include_directories ("src/"
					 "src/gameplay/"
					 "src/iomanager/")

add_executable (yarl
				src/main.cpp
				src/yarl.cpp
				src/gameplay/entity.cpp
				src/gameplay/item.cpp
				src/gameplay/character.cpp
				src/gameplay/sector.cpp
				src/gameplay/tile.cpp
				src/command.h
				src/iomanager/iomanager.cpp
				src/iomanager/cursesiomanager.cpp
				src/variable.cpp
				src/statusBar.cpp
				src/generator.cpp)

set_property (TARGET yarl PROPERTY CXX_STANDARD 11)

target_link_libraries(yarl ${CURSES_LIBRARIES})
