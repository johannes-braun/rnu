add_library(catch2 STATIC "${CMAKE_CURRENT_SOURCE_DIR}/catch_amalgamated.cpp")
target_compile_features(catch2 PRIVATE cxx_std_17)


add_executable(test_vectors "test_vectors.cpp")
target_link_libraries(test_vectors PRIVATE rnu catch2)

enable_testing()
add_test(NAME test_vectors COMMAND test_vectors)