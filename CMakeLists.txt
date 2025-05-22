cmake_minimum_required(VERSION 3.16)
project(password_manager C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(OPENSSL REQUIRED openssl)

include_directories(
    ${GTK4_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

link_directories(
    ${GTK4_LIBRARY_DIRS}
    ${OPENSSL_LIBRARY_DIRS}
)

set(SOURCES
    src/main.c
    src/gui.c
    src/storage.c
    src/crypto.c
)

add_executable(password-manager ${SOURCES})


target_link_libraries(password-manager
    ${GTK4_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)