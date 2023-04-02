cmake_minimum_required(VERSION 3.16)
project(hpx_test CXX)

find_package(HPX REQUIRED)

add_executable(hpx_t main.cpp)
target_link_libraries(hpx_t HPX::hpx HPX::wrap_main HPX::iostreams_component)

