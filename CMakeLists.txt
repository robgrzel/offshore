cmake_minimum_required(VERSION 3.3)
project(UTILS)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)
INCLUDE(FindPkgConfig)


set(RUN_MAIN 1)
set(USE_MPI.OPENMPI 1)
set(USE_MPI.MPICH 0)
set(USE_MPI.OPENMPI.POSIX 0)
set(USE_CUDA 1)
set(USE_SDL 0)
set(USE_GL 0)
set(USE_FLTK 0)
set(USE_MATHGL 0)
set(USE_PTHREAD 0)
set(USE_FFTW 0)
set(USE_OMP 1) #crashed fft
#########################################################
# FIND CUDA
#########################################################
#sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9
#sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8
if (USE_CUDA)
    set(CMAKE_C_COMPILER /usr/bin/gcc-4.9)  # -> ADD THIS LINE <-
    set(CMAKE_CXX_COMPILER /usr/bin/g++-4.9)
    set(CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})

    INCLUDE(FindCUDA)

    find_package(CUDA REQUIRED)
    find_package(CUDA QUIET REQUIRED)

    include_directories(${CUDA_INCLUDE_DIRS})
    link_directories(${CUDA_LIBRARY_DIRS})
    message(libs ${CUDA_LIBRARY_DIRS})
    set(CUDA_SEPARABLE_COMPILATION ON)
    set(CUDA_ARCH_20 -gencode arch=compute_20,code=sm_21)
    set(CUDA_ARGS -pg -g -lineinfo --std=c++11 -O3 -use_fast_math)
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} ${CUDA_ARCH_20} ${CUDA_ARGS})
    #crash when --device-debug -G is on
    message(CUDA ${CUDA_LIBRARY_DIRS})

endif (USE_CUDA)

#########################################################
# FIND MPI
#########################################################

#if (MPI_FOUND)
if (USE_MPI.OPENMPI)
    message("USE_MPI.OPENMPI")
    include_directories(/usr/include/openmpi)
    SET(CMAKE_C_COMPILER /usr/bin/mpicc.openmpi)
    SET(CMAKE_CXX_COMPILER /usr/bin/mpicxx.openmpi)
elseif (USE_MPI.MPICH)
    message("USE_MPI.MPICH")
    find_package(MPI REQUIRED)
    include_directories(/usr/include/mpich)
    SET(CMAKE_C_COMPILER /usr/bin/mpicc.mpich)
    SET(CMAKE_CXX_COMPILER /usr/bin/mpicxx.mpich)
elseif (USE_MPI.OPENMPI.POSIX)
    message("USE_MPI.MPICH")
    find_package(MPI REQUIRED)
    include_directories(/opt/openmpi_posix/include)
    SET(CMAKE_C_COMPILER /opt/openmpi_posix/bin/mpicc)
    SET(CMAKE_CXX_COMPILER /opt/openmpi_posix/bin/mpicxx)
endif (USE_MPI.OPENMPI)
#endif (MPI_FOUND)

#option(enable-mpi ON)
#option(enable-pthr-widget ON)


#find_package(ROOT REQUIRED COMPONENTS RIO Net)

#qt5_use_modules(${PROJECT_NAME} Core Widgets Gui)

#find_package(OpenMP REQUIRED)


set(PATH MPI_CUDA) #.

set(SOURCE_FILES
        UtilSources/CUDA_DUMMY.h
        UtilSources/Math/const.h
        UtilSources/Math/real.h
        UtilSources/Math/FFT.cpp
        UtilSources/Math/FFT.h
        UtilSources/utils.h
        UtilSources/Math/matrixmath.h
        UtilSources/Math/matrixmath.cpp
        UtilSources/Math/math.h
        UtilSources/Math/math.cpp
        UtilSources/template_utils.cpp
        UtilSources/template_utils.h
        UtilSources/Containers/std_utils.h
        UtilSources/Containers/std_vector_3d.h
        UtilSources/Containers/std_vector_3d.cpp
        UtilSources/Containers/std_vector_2d.cpp
        UtilSources/Containers/std_vector_2d.h
        ${PATH}/MpiMaster.cpp
        ${PATH}/MpiMaster.h
        ${PATH}/MpiSlave.h
        ${PATH}/MpiSlave.cpp
        ${PATH}/OElemCylinder.h
        ${PATH}/OElemCylinder.cpp
        ${PATH}/VElem.h
        ${PATH}/VElem.cpp
        # ${PATH}/OSubSystemElems.h
        #${PATH}/OSubSystemElems.cpp //to do, create OSubSystem with option to choose for Waves and Elems
        ${PATH}/OSubSystemWaves.h
        ${PATH}/OSubSystemWaves.cpp
        ${PATH}/OTime.h
        ${PATH}/OTime.cpp
        ${PATH}/OWave.h
        ${PATH}/OWave.cpp
        ${PATH}/OWaves.h
        ${PATH}/OWaves.cpp
        ${PATH}/OWavenumber.h
        ${PATH}/OWavenumber.cpp
        ${PATH}/OParticle.h
        ${PATH}/OParticle.cpp
        #UtilSources/mem_utils.h
        #UtilSources/mem_utils.cpp
        ${PATH}/structs.h
        ${PATH}/structs.cpp
        ${PATH}/structs.cuh
        ${PATH}/structs.cu
        ${PATH}/main.cpp)

if (USE_OMP)
    find_package(OpenMP REQUIRED)
    if (OPENMP_FOUND)
        message(OPENMP_FOUND_C "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        message(OPENMP_FOUND_CXX "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif (OPENMP_FOUND)
endif (USE_OMP)

if (USE_FFTW)
    #find_package(FFTW REQUIRED)
    include_directories(/usr/include/fftw3.h /usr/include/fftw.h)
    link_directories(/usr/lib/libfftw.so)
endif (USE_FFTW)

if (USE_SDL)
    #########################################################
    # FIND SDL2
    #########################################################
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIR})
    link_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIR})
endif (USE_SDL)
if (USE_GL)
    #########################################################
    # FIND GLUT
    #########################################################
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})
    if (NOT GLUT_FOUND)
        message(ERROR " GLUT not found!")
    endif (NOT GLUT_FOUND)
    link_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIR})
    #########################################################
    # FIND GLEW
    #########################################################
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY_DIRS})
    add_definitions(${GLEW_DEFINITIONS})
    if (NOT GLEW_FOUND)
        message(ERROR " GLEW not found!")
    endif (NOT GLEW_FOUND)
    #########################################################
    # FIND OPENGL
    #########################################################
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    link_directories(${OpenGL_LIBRARY_DIRS})
    add_definitions(${OpenGL_DEFINITIONS})
    if (NOT OPENGL_FOUND)
        message(ERROR " OPENGL not found!")
    endif (NOT OPENGL_FOUND)
endif (USE_GL)
if (USE_FLTK)
    find_package(FLTK REQUIRED)

    #########################################################
    # FIND FLTK
    #########################################################
    include_directories(/usr/include/FL)
endif (USE_FLTK)
if (USE_PTHREADS)
    find_package(Threads REQUIRED)
    option(MGL_HAVE_PTHREAD ON)

    #########################################################
    # FIND PTHREADS
    #########################################################
    if (THREADS_HAVE_PTHREAD_ARG)
        target_compile_options(PUBLIC ${PROJECT_NAME} "-pthread")
    endif ()
endif (USE_PTHREADS)

#########################################################
# ADD EXECUTABLE
#########################################################
if (USE_CUDA)
    cuda_add_executable(${PROJECT_NAME} ${SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME} -lcuda -lcudart -lcurand)
else ()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif (USE_CUDA)
#########################################################
# LINK
#########################################################
if (USE_MATHGL)
    #########################################################
    # LINK QT/MATHGL
    #########################################################
    message("LINK MATHGL...")
    target_link_libraries(${PROJECT_NAME} mgl-mpi mgl-qt mgl-glut mgl-fltk mgl m rt)
endif (USE_MATHGL)
#########################################################
# LINK MPI/PTHREADS
#########################################################
if (MPI_FOUND)
    if (USE_MPI.OPENMPI)
        message("LINK OpenMPI...")
        target_link_libraries(${PROJECT_NAME} /usr/lib/openmpi/lib/libmpi.so)
    elseif (USE_MPI.MPICH)
        message("LINK MPICH...")
        target_link_libraries(${PROJECT_NAME} /usr/lib/mpich/lib/libmpich.so)
    endif (USE_MPI.OPENMPI)
endif (MPI_FOUND)
if (USE_OMP)
    target_link_libraries(${PROJECT_NAME} -fopenmp)
endif (USE_OMP)
if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
endif ()
#########################################################
# LINK FLTK/OPENGL/SDL2
#########################################################
if (USE_FLTK)
    message("LINK FLTK...")
    target_link_libraries(${PROJECT_NAME} fltk fltk_gl)
endif (USE_FLTK)
if (USE_GL)
    message("LINK OPENGL...")
    find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib64)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
endif (USE_GL)
if (USE_SDL)
    message("LINK SDL2...")
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
endif (USE_SDL)
#########################################################
# FFTW3
#########################################################
if (USE_FFTW)
    find_library(FFTW_LIBRARY fftw3)
    target_link_libraries(${PROJECT_NAME} -lfftw3 -lm)
endif (USE_FFTW)

#target_link_libraries(
#        ${PROJECT_NAME}
#        -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree
#        -lRint -lPostscript -lMatrix -lPhysics
#        -lMathCore -lThread -pthread -lm -ldl -rdynamic)

# Add a test:
#enable_testing()
#add_test(${PROJECT_NAME} ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})

