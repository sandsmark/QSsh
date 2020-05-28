add_subdirectory(qssh)

#INSTALL RULES
install(
  DIRECTORY .
  DESTINATION include
  COMPONENT headers
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)