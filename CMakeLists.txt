CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(flowabs)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}" )
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

IF(MSVC)
    ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4)
SET(QT_USE_QTMAIN 1)
SET(QT_USE_QTOPENGL 1)
INCLUDE(${QT_USE_FILE})

MACRO(QT4_AUTO_WRAP outfiles)
    FOREACH(fileName ${ARGN})
        IF(fileName MATCHES "\\.h$")
            FILE(STRINGS ${fileName} lines REGEX Q_OBJECT)
            IF(lines)
                SET(moc_headers ${moc_headers} ${fileName})
                MESSAGE(STATUS "moc: ${fileName}")
            ENDIF()
        ENDIF()
        IF(fileName MATCHES "\\.ui$")
            SET(ui_files ${ui_files} ${fileName})
            MESSAGE(STATUS "uic: ${fileName}")
        ENDIF()
        IF(fileName MATCHES "\\.qrc$")
            SET(qrc_files ${qrc_files} ${fileName})
            MESSAGE(STATUS "qrc: ${fileName}")
        ENDIF()
    ENDFOREACH()
    QT4_WRAP_CPP(${outfiles} ${moc_headers})
    QT4_WRAP_UI(${outfiles} ${ui_files})
    QT4_ADD_RESOURCES(${outfiles} ${qrc_files})
ENDMACRO()

SET( sources
    stable.h
    stable.cpp
    glview.h
    mainwindow.h
    imageview.h
    glsl_program.h
    texture_2d.h
    GLee.h
    main.cpp
    mainwindow.cpp
    imageview.cpp
    glview.cpp
    glsl_program.cpp
    texture_2d.cpp
    GLee.c
    mainwindow.ui
    resources.qrc
    glsl/bf_fs.glsl
    glsl/color_quantization_fs.glsl
    glsl/dog_fs.glsl
    glsl/fdog0_fs.glsl
    glsl/fdog1_fs.glsl
    glsl/gauss3x3_fs.glsl
    glsl/gauss5x5_fs.glsl
    glsl/gauss_fs.glsl
    glsl/lab2rgb_fs.glsl
    glsl/lic_fs.glsl
    glsl/mix_fs.glsl
    glsl/overlay_fs.glsl
    glsl/rgb2lab_fs.glsl
    glsl/rgb2ycbcr_fs.glsl
    glsl/sst_fs.glsl
    glsl/tfm_fs.glsl
    glsl/ycbcr2rgb_fs.glsl )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} )

IF(WIN32)
    SET(sources ${sources} flowabs.rc)
ENDIF()
IF(APPLE)
    SET(sources ${sources} flowabs.icns)
    SET_SOURCE_FILES_PROPERTIES( cefabs.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
ENDIF()

QT4_AUTO_WRAP( sources ${sources} )
ADD_EXECUTABLE( flowabs WIN32 MACOSX_BUNDLE ${sources} )

SOURCE_GROUP(src REGULAR_EXPRESSION "c$|cpp$|hpp$|h$|ui$|qrc$|cu$")
SOURCE_GROUP(glsl REGULAR_EXPRESSION "glsl$")
SOURCE_GROUP(generated REGULAR_EXPRESSION "cxx$|ui_")

IF(MSVC)
    IF(MSVC_IDE)
        SET_TARGET_PROPERTIES( flowabs PROPERTIES COMPILE_FLAGS "/FIstable.h /Yustable.h" )
        SET_SOURCE_FILES_PROPERTIES( stable.cpp PROPERTIES COMPILE_FLAGS "/Ycstable.h" )
        SET_SOURCE_FILES_PROPERTIES( GLee.c PROPERTIES COMPILE_FLAGS "/Y-" )
    ELSE()
        SET_TARGET_PROPERTIES( flowabs PROPERTIES COMPILE_FLAGS "/FIstable.h" )
    ENDIF()
ELSE()
    SET_TARGET_PROPERTIES( flowab
        PROPERTIES
        COMPILE_FLAGS "-include stable.h" )
ENDIF()

IF(APPLE)
    SET_TARGET_PROPERTIES( flowabs PROPERTIES MACOSX_BUNDLE_ICON_FILE "flowabs.icns" )
ENDIF()

TARGET_LINK_LIBRARIES( flowabs ${QT_LIBRARIES} )
