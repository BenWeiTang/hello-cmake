# Generate compile_commands.json so clangd can reason about the project
# because I use neovim btw.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hello_cmake main.cpp)
target_compile_features(hello_cmake PRIVATE cxx_std_17)
target_compile_definitions(hello_cmake PRIVATE $<$<CONFIG:Debug>:DEBUG_VERBOSE>)

add_library(print_hello STATIC print.cpp)
target_compile_features(print_hello PUBLIC cxx_std_17)
target_compile_definitions(print_hello PRIVATE $<$<CONFIG:Debug>:DEBUG_VERBOSE>)

target_include_directories(print_hello PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(hello_cmake print_hello)
