#
# Specify list of tests
#

set(geosx_fileio_tests
   testHDFFile.cpp
   )

set( dependencyList gtest geosx_core )

if ( ENABLE_MPI )
  set ( dependencyList ${dependencyList} mpi )
endif()

if( ENABLE_OPENMP )
  set( dependencyList ${dependencyList} openmp )
endif()

if ( ENABLE_CUDA )
  set( dependencyList ${dependencyList} cuda )
endif()

#
# Add gtest C++ based tests
#
foreach(test ${geosx_fileio_tests})
    get_filename_component( test_name ${test} NAME_WE )
    blt_add_executable( NAME ${test_name}
                        SOURCES ${test}
                        OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                        DEPENDS_ON ${dependencyList}
                        )

    blt_add_test( NAME ${test_name}
                  COMMAND ${test_name}
                  )
endforeach()

if ( ENABLE_MPI )

  set(nranks 2)

  set( geosx_fileio_parallel_tests
       testHDFParallelFile.cpp )
  foreach(test ${geosx_fileio_parallel_tests})
     get_filename_component( test_name ${test} NAME_WE )
     blt_add_executable( NAME ${test_name}
                          SOURCES ${test}
                          OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                          DEPENDS_ON ${dependencyList}
                          )

      blt_add_test( NAME ${test_name}
                    COMMAND ${test_name}
                    NUM_MPI_TASKS ${nranks}
                    )
  endforeach()
endif()
