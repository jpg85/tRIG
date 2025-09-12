FILE(GLOB SRCS *.cpp)
add_executable(RigTransportCppTests ${SRCS})
target_link_libraries(RigTransportCppTests
    PRIVATE
        RigTransportCpp
        Catch2::Catch2WithMain
)

catch_discover_tests(RigTransportCppTests)
