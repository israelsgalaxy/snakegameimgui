cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(snakegame)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)

add_executable(${PROJECT_NAME} main.cpp gui.cpp)

find_library(DearImgui imgui REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/GLFW /usr/include/imgui /usr/include/imgui/backends /usr/include/stb)

target_link_libraries(${PROJECT_NAME} PRIVATE ${DearImgui} glfw stb)