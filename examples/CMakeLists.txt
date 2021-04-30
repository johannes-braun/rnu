add_executable(ex0 ex0.cpp "vec.hpp" "mat.hpp" "traits.hpp" "math.hpp" "cx_fun.hpp" "quat.hpp" "quat_type.hpp" "quat_type.inl.hpp")
target_link_libraries(ex0 PUBLIC rnu::rnu)
