# ATK needs to have uberenv setup with the following command in the asctoolkit dir:
#python scripts/uberenv/uberenv.py --spec=%clang@apple-mp
#
# Then modify the cmake file in the uberenv directory to include:
#set(ENABLE_MPI ON CACHE PATH "")
#set(MPI_C_COMPILER "mpicc-mpich-clang" CACHE PATH "")
#set(MPI_CXX_COMPILER "mpicxx-mpich-clang" CACHE PATH "")
#set(MPI_Fortran_COMPILER "mpifort-mpich-clang" CACHE PATH "")
#set(MPIEXEC "mpirun-mpich-clang" CACHE PATH "")

site_name(HOST_NAME)
message($ENV{HOME})

set(ATK_ROOT "$ENV{HOME}/Codes/asctoolkit" CACHE PATH "")
set(CONFIG_NAME "${HOST_NAME}-darwin-x86_64-clang@apple-mp" CACHE PATH "") 
set(ATK_DIR "${ATK_ROOT}/install-${CONFIG_NAME}-debug" CACHE PATH "")
set(RAJA_DIR "$ENV{HOME}/Codes/RAJA/install-clang-3.7.0-release" CACHE PATH "")

message("ATK_DIR=${ATK_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/hc-defaults.cmake")
include("${ATK_ROOT}/uberenv_libs/${CONFIG_NAME}.cmake")

set(GEOSX_LINK_PREPEND_FLAG "-Wl,-force_load" CACHE PATH "" FORCE)
set(GEOSX_LINK_POSTPEND_FLAG "" CACHE PATH "" FORCE)

#set(GEOSX_LINK_PREPEND_FLAG  "-Wl,--whole-archive"    CACHE PATH "" FORCE)
#set(GEOSX_LINK_POSTPEND_FLAG "-Wl,--no-whole-archive" CACHE PATH "" FORCE)
