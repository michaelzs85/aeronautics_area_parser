cmake_minimum_required(VERSION 3.1)

project(geo)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()



set(headers
    geotypes.h
    geoparser.h
    geovincenty.h
    geoutils.h
    geoast.h
    geoast_adapted.h
    geoparser_config.h
    geolocation.h
    geolocation_def.h
    )

set(sources
    geovincenty.cpp
    geoutils.cpp
    geolocation.cpp
    )

add_executable(${PROJECT_NAME} ${headers} ${sources} "main.cpp")
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

add_executable(spirittest ${headers} ${sources} "spirittest.cpp")
target_link_libraries(spirittest ${CONAN_LIBS})

add_executable(fileparser ${headers} ${sources} "fileparser.cpp")
target_link_libraries(fileparser ${CONAN_LIBS})
