find_program(HOSTNAME_CMD NAMES hostname)
exec_program(${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
set(CTEST_SITE              "${HOSTNAME}")

find_program(UNAME NAMES uname)
macro(getuname name flag)
  exec_program("${UNAME}" ARGS "${flag}" OUTPUT_VARIABLE "${name}")
endmacro(getuname)

getuname(osname -s)
getuname(osrel  -r)
getuname(cpu    -m)
set(CTEST_BUILD_NAME        "Linux-c++")
set(CTEST_COMPILER_NAME     "/usr/bin/c++")

find_program(CTEST_SVN_COMMAND NAMES svn)
find_program(MAKE NAMES make)

SET(CTEST_SOURCE_DIRECTORY "..")
SET(CTEST_BINARY_DIRECTORY ".")

#set(CTEST_GIT_COMMAND "/usr/bin/git" )
#SET(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")#${CTEST_GIT_COMMAND}
#SET(CTEST_UPDATE_OPTIONS "${GIT_UPDATE_OPTIONS}")
#set(CMake_SOURCE_DIR “${CTEST_SOURCE_DIRECTORY}”)

set(CTEST_CONFIGURE_COMMAND "cmake ${CTEST_SOURCE_DIRECTORY}")
set(CTEST_BUILD_COMMAND     "make -i")
SET(CTEST_CMAKE_GENERATOR "Unix Makefiles")
SET(CTEST_BUILD_CONFIGURATION "$ENV{CMAKE_CONFIG_TYPE}")

ctest_start(Experimental)
#ctest_update(SOURCE "${CTEST_SOURCE_DIRECTORY}")
#ctest_submit(PARTS Update Notes)

# to get CTEST_PROJECT_SUBPROJECTS definition:
include("${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake")

foreach(subproject ${CTEST_PROJECT_SUBPROJECTS})
  set_property(GLOBAL PROPERTY SubProject ${subproject})
  set_property (GLOBAL PROPERTY Label ${subproject})

  ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
  #ctest_submit(PARTS Configure)

  set(CTEST_BUILD_TARGET "${subproject}")
  ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
   # builds target ${CTEST_BUILD_TARGET}

  ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}"
   INCLUDE_LABEL "${subproject}" RETURN_VALUE res
  )
   # runs only tests that have a LABELS property
#matching ("${subproject}")
  ctest_submit(RETURN_VALUE res)
endforeach()

