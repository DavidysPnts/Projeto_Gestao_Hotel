cmake_minimum_required(VERSION 3.14)
project(hotel_system VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avisos de compilação
add_compile_options(-Wall -Wextra)

# Arquivos fonte
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(hotel_system ${SOURCES})
target_include_directories(hotel_system PRIVATE src)

# Catch2 pra testes
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(testes tests/test_tp2.cpp)
target_include_directories(testes PRIVATE src)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(testes)