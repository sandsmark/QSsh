add_executable(errorhandling
    main.cpp )

target_link_libraries(errorhandling PRIVATE QSsh Qt5::Core Qt5::Network ${BOTAN_LIB})

