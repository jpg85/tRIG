FILE(GLOB SRCS Source/*.*)
FILE(GLOB_RECURSE EXPORTS Export/*.h)
FILE(GLOB_RECURSE HDRS Interface/*.h)

add_library(RigTransportCppInterface INTERFACE ${HDRS})
target_include_directories(RigTransportCppInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Interface)

add_library(RigTransportCpp SHARED ${SRCS} ${EXPORTS})
target_link_libraries(RigTransportCpp
    PUBLIC
        RigTransportCppInterface
    PRIVATE
        RigTransportInProcess
)
generate_export_header(RigTransportCpp)
target_include_directories(RigTransportCpp
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Export
)

add_subdirectory(Libs)
add_subdirectory(Tests)