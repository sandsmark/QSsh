set(LIBTYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(LIBTYPE SHARED)
endif()

add_library(QSsh ${LIBTYPE}
    sshsendfacility.cpp
    sshremoteprocess.cpp
    sshpacketparser.cpp
    sshpacket.cpp
    sshoutgoingpacket.cpp
    sshkeygenerator.cpp
    sshkeyexchange.cpp
    sshincomingpacket.cpp
    sshcryptofacility.cpp
    sshconnection.cpp
    sshchannelmanager.cpp
    sshchannel.cpp
    sshcapabilities.cpp
    sftppacket.cpp
    sftpoutgoingpacket.cpp
    sftpoperation.cpp
    sftpincomingpacket.cpp
    sftpdefs.cpp
    sftpchannel.cpp
    sshremoteprocessrunner.cpp
    sshconnectionmanager.cpp
    sshkeypasswordretriever.cpp
    sftpfilesystemmodel.cpp
    sshdirecttcpiptunnel.cpp
    sshhostkeydatabase.cpp
    sshlogging.cpp
    sshtcpipforwardserver.cpp
    sshtcpiptunnel.cpp
    sshforwardedtcpiptunnel.cpp
    sshagent.cpp
    sshx11channel.cpp
    sshx11inforetriever.cpp
    opensshkeyfilereader.cpp
    qssh.qrc)

get_property(BOTAN_LIB GLOBAL PROPERTY BOTAN_LIB)
target_link_libraries(QSsh Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Widgets ${BOTAN_LIB})

# state that anybody linking to us needs to include the current source dir
target_include_directories(QSsh
          INTERFACE
		  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
		  $<INSTALL_INTERFACE:include>
          $<INSTALL_INTERFACE:include/qssh>
          )
		  
#INSTALL RULES
install(
  DIRECTORY .
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qssh
  COMPONENT headers
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(TARGETS QSsh DESTINATION ${CMAKE_INSTALL_LIBDIR} EXPORT QSsh-targets)
