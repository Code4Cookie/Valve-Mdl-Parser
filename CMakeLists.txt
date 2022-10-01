cmake_minimum_required (VERSION 3.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project (ValveMDLParser)

file(GLOB sources src/*.cpp)
file(GLOB_RECURSE headers include/*.h)
add_library(ValveMDLParser ${sources} ${headers} ) # static by default

# Supress warnings generated from the contents within Source's studio header file.
add_compile_options(/wd4244) # 'conversion' conversion from 'type1' to 'type2', possible loss of data.
add_compile_options(/wd26495) # Variable '*parameter-name' is uninitialized. Always initialize a member variable.

target_include_directories(ValveMDLParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)