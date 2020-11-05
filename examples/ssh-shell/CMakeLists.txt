add_executable(ssh-hell
    argumentscollector.cpp  main.cpp  shell.cpp)

target_link_libraries(ssh-hell PRIVATE QSsh Qt5::Core Qt5::Network ${BOTAN_LIB})

