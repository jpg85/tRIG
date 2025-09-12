FILE(GLOB SRCS Source/*.cpp)
FILE(GLOB_RECURSE EXPORTS Export/*.h)

add_library(RigTransportCommon SHARED ${SRCS} ${EXPORTS})
generate_export_header(RigTransportCommon)
target_include_directories(RigTransportCommon
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Export
)
target_link_libraries(RigTransportCommon
    PRIVATE
        RigTransportCppInterface
)
add_subdirectory(Tests)
