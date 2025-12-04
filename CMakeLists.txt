#----------------------------------------------------------------------------
# Set up colors
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(iThembaData)

#----------------------------------------------------------------------------
# find the git package (needed to determine the version of the code)
find_package(Git REQUIRED)

#----------------------------------------------------------------------------
# find the root package (and add COMPONENTS)
#find_package(ROOT REQUIRED COMPONENTS Gui GuiHtml Minuit Spectrum)
find_package(ROOT OPTIONAL_COMPONENTS XMLParser)

if(${ROOT_xml_FOUND})
	add_compile_options(-DHAS_XML)
	set(CLING_OPTIONS "-DHAS_XML")
else()
	message("${Red}XML feature for ROOT not found (ROOT_xml_FOUND = ${ROOT_xml_FOUND})!${ColourReset}")
endif()

#----------------------------------------------------------------------------
# set c++ standard to the one used for root
if(DEFINED ROOT_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
else()
	set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED True)

#----------------------------------------------------------------------------
# configure the include directories and set other variables
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_INSTALL_BINDIR ${CMAKE_BINARY_DIR}/bin)

#----------------------------------------------------------------------------
# copy includes
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${CMAKE_BINARY_DIR})

#----------------------------------------------------------------------------
# define all libraries and root dictionaries
# for some we also create dependencies on other libraries to remove linking errors later on

add_library(TTdr SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdr/TTdrEvent.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdr/TTdrFile.cxx
	)
root_generate_dictionary(G__TTdr TTdrEvent.h TTdrFile.h MODULE TTdr LINKDEF ${PROJECT_SOURCE_DIR}/libraries/TTdr/LinkDef.h OPTIONS ${CLING_OPTIONS})
target_link_libraries(TTdr TTdrFormat ${ROOT_LIBRARIES})
add_dependencies(TTdr iThembaDataVersion)

add_library(TTdrFormat SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdrFormat/TTdrDetectorInformation.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrFormat/TTdrMnemonic.cxx
	)
root_generate_dictionary(G__TTdrFormat TTdrDetectorInformation.h TTdrMnemonic.h MODULE TTdrFormat LINKDEF ${PROJECT_SOURCE_DIR}/libraries/TTdrFormat/LinkDef.h)
target_link_libraries(TTdrFormat TTdrTigress TTdrClover TTdrPlastic TTdrSiLi)

add_library(TTdrDataParser SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdrDataParser/TTdrDataParser.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrDataParser/TTdrDataParserException.cxx
	)
target_link_libraries(TTdrDataParser TTdr)

add_library(TTdrTigress SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrTigress/TTdrTigress.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrTigress/TTdrTigressHit.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrTigress/TTdrTigressBgo.cxx
	)
root_generate_dictionary(G__TTdrTigress TTdrTigress.h TTdrTigressHit.h TTdrTigressBgo.h MODULE TTdrTigress LINKDEF ${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrTigress/LinkDef.h)
target_link_libraries(TTdrTigress ${GRSI_LIBRARIES})

add_library(TTdrClover SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrClover/TTdrClover.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrClover/TTdrCloverHit.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrClover/TTdrCloverBgo.cxx
	)
root_generate_dictionary(G__TTdrClover TTdrClover.h TTdrCloverHit.h TTdrCloverBgo.h MODULE TTdrClover LINKDEF ${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrClover/LinkDef.h)

add_library(TTdrPlastic SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrPlastic/TTdrPlastic.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrPlastic/TTdrPlasticHit.cxx
	)
root_generate_dictionary(G__TTdrPlastic TTdrPlastic.h TTdrPlasticHit.h MODULE TTdrPlastic LINKDEF ${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrPlastic/LinkDef.h)

add_library(TTdrSiLi SHARED
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrSiLi/TTdrSiLi.cxx
	${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrSiLi/TTdrSiLiHit.cxx
	)
root_generate_dictionary(G__TTdrSiLi TTdrSiLi.h TTdrSiLiHit.h MODULE TTdrSiLi LINKDEF ${PROJECT_SOURCE_DIR}/libraries/TTdrAnalysis/TTdrSiLi/LinkDef.h)

##----------------------------------------------------------------------------
# add the global library with the functions to create files and parsers
set(ITHEMBADATA_LIBRARIES TTdr TTdrFormat TTdrDataParser TTdrTigress TTdrClover TTdrPlastic TTdrSiLi)

add_library(iThembaData SHARED
	${PROJECT_SOURCE_DIR}/src/iThembaDataLibrary.cxx
	)
target_link_libraries(iThembaData ${ITHEMBADATA_LIBRARIES})
add_dependencies(iThembaData iThembaDataVersion)

##----------------------------------------------------------------------------
# custom target to generate the version header
# CMAKE_SOURCE_DIR is the GRSISort directory
# PROJECT_SOURCE_DIR is the _deps/grsidata-src directory
add_custom_target(iThembaDataVersion
	${CMAKE_COMMAND} -D SRC=${PROJECT_SOURCE_DIR}/include/iThembaDataVersion.h.in
	                 -D DST=${PROJECT_SOURCE_DIR}/include/iThembaDataVersion.h
                    -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                    -P ${CMAKE_SOURCE_DIR}/GenerateVersionHeader.cmake
  )

