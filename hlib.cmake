file(GLOB_RECURSE kernel-src
    code/hlib/src/*.cpp)

add_library(hlib STATIC ${kernel-src})

target_include_directories(hlib PUBLIC code/hlib/include)
