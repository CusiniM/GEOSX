
#
# Specify all headers
#
set(math_headers
  	interpolation/Interpolation.hpp
   )

#
# Specify all sources
#
#set(geosx_core_sources
#  ${geosx_core_sources}
#    PARENT_SCOPE )


blt_add_library( NAME                  math
                 SOURCES               
                 HEADERS               ${math_headers}
                 DEPENDS_ON            
                 SHARED FALSE
               )
               
target_include_directories( math INTERFACE ${CMAKE_SOURCE_DIR}/coreComponents)

geosx_add_code_checks(PREFIX math )
