cmake_minimum_required(VERSION 3.26)
project(Simulacro_SO C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Simulacro_SO
        sintetico.c
        sintetico.h
        eventos.c
        eventos.h
        memoria.c
        memoria.h
        semaforo.c
        semaforo.h
        main.c
        interface.c
        interface.h
)
