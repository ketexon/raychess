cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
