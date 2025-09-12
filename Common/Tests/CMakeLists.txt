FILE(GLOB SRCS *.cpp)
add_executable(RigTransportCommonTests ${SRCS})
target_link_libraries(RigTransportCommonTests
    PRIVATE
        RigTransportCommon
        RigTransportCppInterface
        Catch2::Catch2WithMain
)

catch_discover_tests(RigTransportCommonTests)
