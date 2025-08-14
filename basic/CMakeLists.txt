cmake_minimum_required(VERSION 3.8)
project(BASIC C)

set(BASIC_NUM_MODE "double" CACHE STRING "Numeric mode for BASIC: double, long-double, fixed64, msfp")
set_property(CACHE BASIC_NUM_MODE PROPERTY STRINGS double long-double fixed64 msfp)

set(BASIC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(BASIC_BASE_SRC
    src/basicc.c
    src/basic_runtime.c
    src/basic_pool.c
    src/arena.c
)

set(VENDOR_SRC)
set(BASICC_NAME "basicc")

if(BASIC_NUM_MODE STREQUAL "long-double")
  list(APPEND VENDOR_SRC
       src/vendor/libdfp/decContext.c
       src/vendor/libdfp/decNumber.c
       src/vendor/libdfp/decQuad.c
       src/vendor/ryu/d2s.c
       src/vendor/ryu/f2s.c
       src/vendor/ryu/generic_128.c
       src/vendor/ryu/ld2s.c
       src/vendor/kitty/kitty.c
       src/vendor/kitty/lodepng.c)
  set(BASICC_NAME "basicc-ld")
  add_compile_definitions(BASIC_USE_LONG_DOUBLE)
elseif(BASIC_NUM_MODE STREQUAL "fixed64")
  list(APPEND VENDOR_SRC src/vendor/fixed64/fixed64.c)
  set(BASICC_NAME "basicc-fix")
  add_compile_definitions(BASIC_USE_FIXED64)
elseif(BASIC_NUM_MODE STREQUAL "msfp")
  list(APPEND VENDOR_SRC
       src/vendor/libdfp/decContext.c
       src/vendor/libdfp/decNumber.c
       src/vendor/libdfp/decQuad.c
       src/vendor/ryu/d2s.c
       src/vendor/ryu/f2s.c
       src/vendor/kitty/kitty.c
       src/vendor/kitty/lodepng.c)
  set(BASICC_NAME "basicc-msfp")
  add_compile_definitions(BASIC_USE_MSFP)
else()
  list(APPEND VENDOR_SRC
       src/vendor/libdfp/decContext.c
       src/vendor/libdfp/decNumber.c
       src/vendor/libdfp/decQuad.c
       src/vendor/ryu/d2s.c
       src/vendor/ryu/f2s.c
       src/vendor/kitty/kitty.c
       src/vendor/kitty/lodepng.c)
  set(BASICC_NAME "basicc")
  add_compile_definitions(BASIC_USE_DOUBLE)
endif()

add_executable(${BASICC_NAME} ${BASIC_BASE_SRC} ${VENDOR_SRC})

target_include_directories(${BASICC_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/libdfp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/kitty
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/fixed64
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/ryu)

target_compile_definitions(${BASICC_NAME} PRIVATE BASIC_SRC_DIR=\"${CMAKE_SOURCE_DIR}\")

target_link_libraries(${BASICC_NAME} mir)
if(UNIX)
  target_link_libraries(${BASICC_NAME} m)
endif()

if(BUILD_TESTING)
  add_executable(basic_num_fixed64_test
    test/basic_num_fixed64_test.c
    src/vendor/fixed64/fixed64.c)
  target_compile_definitions(basic_num_fixed64_test PRIVATE BASIC_USE_FIXED64)
  target_include_directories(basic_num_fixed64_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/fixed64)
  if(UNIX)
    target_link_libraries(basic_num_fixed64_test m)
  endif()
  add_test(NAME basic_num_fixed64_test COMMAND basic_num_fixed64_test)

  add_executable(ld2s_test
    test/ld2s_test.c
    src/vendor/ryu/generic_128.c
    src/vendor/ryu/ld2s.c)
  target_include_directories(ld2s_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor)
  if(UNIX)
    target_link_libraries(ld2s_test m)
  endif()
  add_test(NAME ld2s_test COMMAND ld2s_test)
  add_test(NAME basic-tests COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/run-tests.sh $<TARGET_FILE:${BASICC_NAME}>)
endif()
