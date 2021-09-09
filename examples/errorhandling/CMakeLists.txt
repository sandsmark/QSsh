add_executable(errorhandling
    main.cpp )

target_link_libraries(errorhandling PRIVATE QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${BOTAN_LIB})

