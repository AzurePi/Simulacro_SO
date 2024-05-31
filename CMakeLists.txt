cmake_minimum_required(VERSION 3.26)
project(Simulacro_SO C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_PROJECT_DESCRIPTION "Projeto de simulacro de um sistema operacional")

include_directories(.)

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

add_executable(Simulacro_SO ${SOURCES})

target_link_libraries(Simulacro_SO m)