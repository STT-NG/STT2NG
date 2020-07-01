set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(ENABLE_GUI "Compile with GUI support." ON)
option(ENABLE_3D "Compile with support for the 3D viewer." OFF)
option(ENABLE_STTS "Compile with support for various STT configurations. Only PANDA is accepted by default." OFF)

if(ENABLE_GUI)
    set(CMAKE_AUTOUIC ON)
endif()

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ENABLE_GUI OR NOT ENABLE_STTS)
    list(APPEND HEADERS
        include/graphwriter.h
        include/geometryparameter.h
        include/parametermodel.h
    )
    list(APPEND SOURCES
        src/main.cpp

        src/graphwriter.cpp
        src/geometryparameter.cpp
        src/parametermodel.cpp
    )

    source_group("Source Files" FILES ${SOURCES})
    source_group("Header Files" FILES ${HEADERS})

    if (NOT ENABLE_STTS)
        list(REMOVE_ITEM SOURCES
            src/parametermodel.cpp
            src/geometryparameter.cpp
        )
        list(REMOVE_ITEM HEADERS
            include/parametermodel.h
            include/geometryparameter.h
        )
    endif()

    add_executable(STT2NG
      ${SOURCES}
      ${HEADERS}
    )

    target_link_libraries(STT2NG PRIVATE Qt5::Core GeomRel STTUtil)

    if (ENABLE_STTS)
        target_link_libraries(STT2NG PRIVATE Qt5::Widgets)
    endif()

else()
    file(GLOB HEADERS include/*.h)
    file(GLOB SOURCES src/*.cpp)

    source_group("Source Files" FILES ${SOURCES})
    source_group("Header Files" FILES ${HEADERS})

    if (NOT ENABLE_3D)
        list(REMOVE_ITEM SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/threedscene.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/threedview.cpp
        )
        list(REMOVE_ITEM HEADERS
            ${CMAKE_CURRENT_SOURCE_DIR}/include/threedscene.h
            ${CMAKE_CURRENT_SOURCE_DIR}/include/threedview.h
        )
    endif()

    add_executable(STT2NG
      ${SOURCES}
      ${HEADERS}
      src/mainwindow.ui
    )

    target_link_libraries(STT2NG PRIVATE Qt5::Core Qt5::Widgets Qt5::Charts Qt5::Svg GeomRel STTUtil)
    if(ENABLE_3D)
        target_link_libraries(STT2NG PRIVATE Qt5::3DCore Qt5::3DExtras)
    endif()
endif()

configure_file(config.h.in include/config.h @ONLY)

target_include_directories(STT2NG PRIVATE include/ ${CMAKE_CURRENT_BINARY_DIR}/include)


