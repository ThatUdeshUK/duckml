cmake_minimum_required(VERSION 3.5)

project(duckdb_llama_cpp CXX)
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Trying to find at: $ENV{LIBLLAMA_INSTALL_PREFIX}/bin")
find_library(LIBLLAMA_LIB llama HINTS "$ENV{LIBLLAMA_INSTALL_PREFIX}/bin")
message(STATUS "LLAMA_LIB Libs: ${LIBLLAMA_LIB}")

add_library(duckdb_llama_cpp STATIC duckdb_llama_cpp.cpp)

target_link_libraries(duckdb_llama_cpp PUBLIC ${LIBLLAMA_LIB})

target_include_directories(
  duckdb_llama_cpp
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(duckdb_llama_cpp PROPERTIES EXPORT_NAME duckdb_llama_cpp)

install(TARGETS duckdb_llama_cpp
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

disable_target_warnings(duckdb_llama_cpp)
