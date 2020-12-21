file(GLOB_RECURSE hlib-src
    code/hlib/src/*.cpp)

add_library(hlib STATIC ${hlib-src})

target_include_directories(hlib PUBLIC code/hlib/include)
