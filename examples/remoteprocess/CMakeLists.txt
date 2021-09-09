add_executable(remoteprocess
    main.cpp remoteprocesstest.cpp argumentscollector.cpp)

target_link_libraries(remoteprocess PRIVATE QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${BOTAN_LIB})

