cmake_minimum_required(VERSION 3.14)
project(hotel_system VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avisos de compilação
add_compile_options(-Wall -Wextra)

include(FetchContent)

# nlohmann/json — usado na serialização
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Arquivos fonte do executável principal (não-recursivo: exclui src/gui/,
# que é compilada separadamente no target hotel_gui, com Qt linkado)
file(GLOB SOURCES "src/*.cpp")
add_executable(hotel_system ${SOURCES})
target_include_directories(hotel_system PRIVATE src)
target_link_libraries(hotel_system PRIVATE nlohmann_json::nlohmann_json)

# Catch2 pra testes
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(testes tests/test_tp2.cpp tests/test_tp3.cpp)
target_include_directories(testes PRIVATE src)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)

include(CTest)
include(Catch)
catch_discover_tests(testes)

# Só é compilada se o Qt6 estiver instalado
# caso contrário, o build principal segue normalmente sem a GUI.
find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    qt_standard_project_setup()
    qt_add_executable(hotel_gui
        src/gui/gui_main.cpp
        src/gui/main_window.hpp
        src/gui/main_window.cpp
    )
    target_include_directories(hotel_gui PRIVATE src)
    target_link_libraries(hotel_gui PRIVATE Qt6::Widgets nlohmann_json::nlohmann_json)
    message(STATUS "Qt6 encontrado — target 'hotel_gui' habilitado")
else()
    message(STATUS "Qt6 nao encontrado — pulando target 'hotel_gui' (opcional, Q6)")
endif()