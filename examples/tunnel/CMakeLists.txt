add_executable(tunnel
    main.cpp
    argumentscollector.cpp
    directtunnel.cpp
    forwardtunnel.cpp)

target_link_libraries(tunnel PRIVATE QSsh Qt5::Core Qt5::Network ${BOTAN_LIB})

