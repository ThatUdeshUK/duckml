# Header-only library — nothing to compile, but register the include path
# so that targets depending on this can find optimal_selector.hpp.
add_library(duckdb_model_selection INTERFACE)
target_include_directories(duckdb_model_selection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})