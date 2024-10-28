# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\DiplomaQt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DiplomaQt_autogen.dir\\ParseCache.txt"
  "DiplomaQt_autogen"
  )
endif()
