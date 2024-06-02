cmake_minimum_required(VERSION 3.22.1)
project(Simulacro_SO C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_PROJECT_DESCRIPTION "Projeto de simulador de um sistema operacional")

include_directories(.)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

set(SOURCE_DIR "src")

set(SOURCES
        ${SOURCE_DIR}/cpu.c
        ${SOURCE_DIR}/globals.c
        ${SOURCE_DIR}/interface.c
        ${SOURCE_DIR}/main.c
        ${SOURCE_DIR}/memoria.c
        ${SOURCE_DIR}/processo.c
        ${SOURCE_DIR}/semaforo.c
        ${SOURCE_DIR}/syscalls.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} m)
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})