cmake_minimum_required(VERSION 2.8)

file(GLOB CONFIG_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.yaml
    )

install(FILES ${CONFIG_SRCS}
        DESTINATION bin
        )
