add_executable(${PROJECT_NAME} WIN32)

target_sources(${PROJECT_NAME}
    PRIVATE
        MasterPaster.ico
        MasterPaster.rc
        clipboard.cpp
        io.cpp
        keyboard.cpp
        main.cpp
        utils.cpp
    PUBLIC
        clipboard.h
        io.h
        keyboard.h
        main.h
        resource.h
        utils.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE)