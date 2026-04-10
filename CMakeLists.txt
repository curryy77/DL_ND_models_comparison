cmake_minimum_required(VERSION 3.16)
project(spatial_models CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(spatial_models
        src/main.cpp
        src/bb_model.cpp
        src/nd_model.cpp
        src/dl_model.cpp
        src/nd_model.cpp
        src/dl_model.cpp
        src/main.cpp
)

target_include_directories(spatial_models PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# helpful warnings
if (MSVC)
    target_compile_options(spatial_models PRIVATE /W4)
else()
    target_compile_options(spatial_models PRIVATE -Wall -Wextra -Wpedantic)
endif()
