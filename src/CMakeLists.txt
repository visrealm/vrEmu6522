

add_library(vrEmu6522 vrEmu6522.c)

if (WIN32)
  if (BUILD_SHARED_LIBS)
     add_definitions(-DVR_6522_EMU_COMPILING_DLL)
   endif()
endif()

target_include_directories (vrEmu6522 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
