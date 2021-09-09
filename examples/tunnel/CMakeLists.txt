add_executable(tunnel
    main.cpp
    argumentscollector.cpp
    directtunnel.cpp
    forwardtunnel.cpp)

target_link_libraries(tunnel PRIVATE QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${BOTAN_LIB})

