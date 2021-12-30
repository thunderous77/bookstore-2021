cmake_minimum_required(VERSION 3.16)
project(1075)

set(CMAKE_CXX_STANDARD 20)

add_executable(code main.cpp TokenScanner.h FileAccess.h Accounts.h Books.h Diary.h Error.h Express.h)
