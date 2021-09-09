add_executable(SecureUploader
    main.cpp
    securefileuploader.cpp)

target_link_libraries(SecureUploader PRIVATE QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${BOTAN_LIB})

