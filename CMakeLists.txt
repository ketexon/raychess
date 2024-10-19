cmake_minimum_required(VERSION 3.25)

project(RaylibChess)

include(FetchContent)
FetchContent_Declare(
	Raylib
	URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)

FetchContent_MakeAvailable(Raylib)

add_executable(
	RaylibChess
	src/Main.c
	src/Const.h
	src/Chess.h src/Chess.c
	src/Log.h
)

target_link_libraries(
	RaylibChess
	raylib
)

set_target_properties(
	RaylibChess
	PROPERTIES
	EXPORT_COMPILE_COMMANDS ON
)
