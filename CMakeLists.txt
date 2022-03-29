cmake_minimum_required(VERSION 3.21)

project(convert_assoc VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REPO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PLINK_SOURCE_DIR ${REPO_ROOT_DIR}/plink_1.9)
set(PLINK_BINARY_DIR ${PROJECT_BINARY}/plink)
set(CONVERT_SOURCE_DIR ${REPO_ROOT_DIR}/convert)
set(TEST_SOURCE_DIR ${REPO_ROOT_DIR}/test-vsproj)
set(GUI_SOURCE_DIR ${REPO_ROOT_DIR}/convert-assoc-qt/convert_assoc)
find_package(QT NAMES Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
add_subdirectory(${PLINK_SOURCE_DIR})
set(TS_FILES convert_assoc_en_US.ts)
list(TRANSFORM TS_FILES PREPEND ${GUI_SOURCE_DIR}/)
set(GUI_SOURCES
    main.cpp
    genotype_map_select.cpp
    phenotype_select.cpp
    phenotype_file_select.cpp
    genotype_file_select.cpp
    exception_dialog.cpp
    info_dialog.cpp
    genotype_main.cpp
    plink_monitor.cpp
    genotype_map_select.h
    phenotype_select.h
    phenotype_file_select.h
    genotype_file_select.h
    exception_dialog.h
    info_dialog.h
    genotype_main.h
    plink_monitor.h
    genotype_map_select.ui
    phenotype_select.ui
    phenotype_file_select.ui
    genotype_file_select.ui
    exception_dialog.ui
    info_dialog.ui
    genotype_main.ui
    plink_monitor.ui
)
list(TRANSFORM GUI_SOURCES PREPEND ${GUI_SOURCE_DIR}/)
set(CONVERT_SOURCES
    genotype_convert.cpp
    phenotype_convert.cpp
    genotype_convert.h
    phenotype_convert.h
)
list(TRANSFORM CONVERT_SOURCES PREPEND ${CONVERT_SOURCE_DIR}/)
set(PROJECT_SOURCES
    ${GUI_SOURCES}
    ${CONVERT_SOURCES}
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(convert_assoc
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET convert_assoc APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(convert_assoc SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(convert_assoc
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(convert_assoc PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(convert_assoc PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(convert_assoc)
endif()
