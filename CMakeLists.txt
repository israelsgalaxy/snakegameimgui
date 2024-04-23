cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(snakegameimgui)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)

add_executable(${PROJECT_NAME} main.cpp gui.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_subdirectory(libsnakegame)

target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/GLFW /usr/include/imgui /usr/include/imgui/backends /usr/include/stb ${PROJECT_SOURCE_DIR}/libsnakegame)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui glfw stb snakegame)