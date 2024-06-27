cmake_minimum_required(VERSION 3.25)
project(Simulacro_SO C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_PROJECT_DESCRIPTION "Projeto de simulacro de um sistema operacional")

include_directories(.)

set(SOURCE_DIR "src")
set(HEADERS_DIR "include")

set(SOURCES
        ${SOURCE_DIR}/cpu.c
        ${SOURCE_DIR}/globals.c
        ${SOURCE_DIR}/interface.c
        ${SOURCE_DIR}/main.c
        ${SOURCE_DIR}/memoria.c
        ${SOURCE_DIR}/processo.c
        ${SOURCE_DIR}/semaforo.c
        ${SOURCE_DIR}/syscalls.c
        ${SOURCE_DIR}/disco.c
        ${SOURCE_DIR}/tela.c
)

set(HEADERS
        ${HEADERS_DIR}/cpu.h
        ${HEADERS_DIR}/globals.h
        ${HEADERS_DIR}/interface.h
        ${HEADERS_DIR}/memoria.h
        ${HEADERS_DIR}/processo.h
        ${HEADERS_DIR}/semaforo.h
        ${HEADERS_DIR}/syscalls.h
        ${HEADERS_DIR}/disco.h
        ${HEADERS_DIR}/tela.h
)

add_executable(Simulacro_SO ${SOURCES} ${HEADERS})

target_link_libraries(Simulacro_SO m)