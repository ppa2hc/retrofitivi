cmake_minimum_required(VERSION 3.16)

project(untitled2 VERSION 0.1 LANGUAGES CXX)

# Find Python3
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Include directories
include_directories(${Python3_INCLUDE_DIRS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-fpermissive")

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)


qt_add_executable(retrofitivi
    main.cpp 
    digitalauto/digitalauto.cpp
    aiassist/aiassist.cpp
)

qt_add_qml_module(retrofitivi
    URI untitled2
    VERSION 1.0
    QML_FILES 
        Main.qml
        digitalauto.qml
        aiassist.qml
    RESOURCES
        resource/trashbin2.png
        resource/editicon3.png
        resource/bot.png
        resource/bgswlogo.png
        resource/digitalautologo.png
        resource/digitalautologo.jpeg
        resource/etaslogo.jpeg
        resource/etaslogo.jpg
        resource/boschlogo.jpeg
        resource/boschlogo.png
        resource/logo1.png
        resource/logo2.png
        resource/logo3.png
        resource/logo4.png
)

set_target_properties(retrofitivi PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(retrofitivi
    PRIVATE Qt6::Quick
    ${Python3_LIBRARIES}
)

# target_link_libraries(main ${Python3_LIBRARIES})

install(TARGETS retrofitivi
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
