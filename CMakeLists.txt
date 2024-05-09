cmake_minimum_required(VERSION 3.26)
project(Simulacro_SO C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Simulacro_SO
        sintetico.c
        sintetico.h
        main.c
        lista.c
        lista.h
        eventos.c
        eventos.h
        listaCircular.c
        listaCircular.h)
