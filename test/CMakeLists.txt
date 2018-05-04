##############
# Unit Tests
##############
enable_testing()

file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
add_executable(UnitTests_CCGL ${TEST_SRC_FILES})
target_link_libraries(UnitTests_CCGL gtest gtest_main ${CCGLNAME})

install(TARGETS UnitTests_CCGL DESTINATION ${INSTALL_DIR})
###
## This is so you can do 'make test' to see all your tests run, instead of
## manually running the executable runUnitTests to see those specific tests.
add_test(NAME UnitTests_CCGL
         COMMAND UnitTests_CCGL)

if (RUNCOV STREQUAL 1)
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    set(COVERAGE_EXCLUDES 'test/*' '*test/*')
    setup_target_for_coverage(NAME UnitTestCoverage
                              EXECUTABLE UnitTests_CCGL
                              DEPENDENCIES UnitTests_CCGL)
endif ()
