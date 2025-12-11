cmake_minimum_required(VERSION 3.10)
project(sid-jsoncpp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Include directories
include_directories(include)
include_directories(include/sid)

# Source files
set(SOURCES
    src/sid/jsoncpp/utils.cpp
    src/sid/jsoncpp/format.cpp
    src/sid/jsoncpp/parser.cpp
    src/sid/jsoncpp/parser_stats.cpp
    src/sid/jsoncpp/time_calc.cpp
    src/sid/jsoncpp/value.cpp
    src/sid/jsoncpp/schema.cpp
)

# Header files
set(HEADERS
    include/sid/jsoncpp/format.h
    include/sid/jsoncpp/json.h
    include/sid/jsoncpp/parser_control.h
    include/sid/jsoncpp/parser_stats.h
    include/sid/jsoncpp/schema.h
    include/sid/jsoncpp/value.h
)

# Create the library
add_library(sid-jsoncpp ${SOURCES} ${HEADERS})

add_executable(sid-jsoncpp-client
  src/sid/jsoncpp-client/main.cpp
  $<TARGET_OBJECTS:sid-jsoncpp>
)
# Set library properties
set_target_properties(sid-jsoncpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Install targets
install(TARGETS sid-jsoncpp
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/sid/jsoncpp
    DESTINATION include
)