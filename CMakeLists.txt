cmake_minimum_required(VERSION 3.6)
project(hw12)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        hints/directory.h
        hints/pages.c
        hints/pages.h
        hints/slist.c
        hints/slist.h
        hints/util.h
        Makefile
        nufs.c
        report.txt
        storage.c
        storage.h
        test.t)

add_executable(hw12 ${SOURCE_FILES})