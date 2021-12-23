##############
# Unit Tests
# See the official site of GoogleTest supported by CMake for detailed information:
#    https://cmake.org/cmake/help/latest/module/GoogleTest.html
##############
CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)
include(GoogleTest)

file(GLOB TEST_SRC_FILES *.cpp utils/*.cpp raster/*.cpp)
IF (MONGOC_FOUND)
   file(GLOB TEST_MONGODB_SRC_FILES db/*.cpp)
   list(APPEND TEST_SRC_FILES ${TEST_MONGODB_SRC_FILES})
ENDIF ()
set(APPNAME unittest)
add_executable(${APPNAME} ${TEST_SRC_FILES})
SET_TARGET_PROPERTIES(${APPNAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

IF (GTEST_FOUND)
    target_link_libraries(${APPNAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main ${CCGLNAME})
ELSE ()
    target_link_libraries(${APPNAME} PRIVATE gtest gtest_main gmock gmock_main ${CCGLNAME})
ENDIF ()

IF (XCODE)
    # Xcode failed with gtest_discover_tests()
    # Stay tuned https://github.com/google/googletest/issues/3475#issuecomment-873690957
    gtest_add_tests(TARGET      ${APPNAME}
                    TEST_SUFFIX .noArgs
                    TEST_LIST   noArgsTests
                   )
    set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 10)
ELSE ()
    gtest_discover_tests(${APPNAME})
ENDIF ()

IF(CV_CLANG AND LLVM_VERSION_MAJOR)
    TARGET_LINK_LIBRARIES(${APPNAME} PRIVATE ${OpenMP_LIBRARY})
ENDIF ()

install(TARGETS ${APPNAME}
        DESTINATION ${INSTALL_DIR})

if ((CV_GCC OR CV_CLANG) AND CODE_COVERAGE)
#    include(code-coverage)
    target_code_coverage(${CCGLNAME})
    target_code_coverage(${APPNAME} EXCLUDE ${TEST_DIR}/*)
endif ()

# Previously implementation based on CodeCoverage.cmake by Lars Bilke
# But failed with Clang and llvm-cov. So I switched to code-coverage.cmake
#   written by George Cave.  2021-12-23 LJ.
# if (CODE_COVERAGE)
#     # https://github.com/bilke/cmake-modules/issues/40
#     include(CodeCoverage)
#     append_coverage_compiler_flags()
#     setup_target_for_coverage_lcov(
#         NAME coverage
#         EXECUTABLE ctest -C ${CMAKE_BUILD_TYPE}
#         )
# endif ()
# SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" PARENT_SCOPE)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
