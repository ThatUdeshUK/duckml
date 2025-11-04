cmake_minimum_required(VERSION 3.5)

project(duckdb_onnx CXX)
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)

find_library(ONNX_RUNTIME_LIB onnxruntime HINTS "$ENV{ONNX_INSTALL_PREFIX}/lib")
message(STATUS "ORT Include Libs: ${ONNX_RUNTIME_SESSION_INCLUDE_DIRS}")

include_directories(include)
add_library(duckdb_onnx STATIC duckdb_onnx.cpp tokenizer.cpp)

target_link_libraries(duckdb_onnx PUBLIC ${ONNX_RUNTIME_LIB})
target_include_directories(duckdb_onnx PUBLIC ${ONNX_RUNTIME_SESSION_INCLUDE_DIRS})

set_target_properties(duckdb_onnx PROPERTIES EXPORT_NAME duckdb_onnx)

install(TARGETS duckdb_onnx
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

disable_target_warnings(duckdb_onnx)
