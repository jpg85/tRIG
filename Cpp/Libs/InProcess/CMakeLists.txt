FILE(GLOB SRCS Source/*.cpp)
FILE(GLOB_RECURSE HDRS Include/*.h)

add_library(RigTransportInProcess STATIC ${SRCS} ${HDRS})
target_link_libraries(RigTransportInProcess
    PUBLIC
        RigTransportCppInterface
    PRIVATE
        RigTransportCommon
)
target_include_directories(RigTransportInProcess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)