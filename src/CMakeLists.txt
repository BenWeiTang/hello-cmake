# Generate compile_commands.json so clangd can reason about the project
# because I use neovim btw.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hello_cmake main.cpp)

add_library(print_hello STATIC print.cpp)

target_include_directories(print_hello PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(hello_cmake print_hello)
