cmake_minimum_required(VERSION 3.7)
project(Exam)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES

        OWave.cpp
        OWave.h
        OWavenumber.cpp
        OWavenumber.h
        OWaves.cpp
        OWaves.h
        OParticle.cpp
        OParticle.h
        OCylinderFloating.cpp
        OCylinderFloating.h
        UtilSources/Math/real.h
        UtilSources/Math/scatmech.h
        UtilSources/Math/matrixmath.cpp
        UtilSources/Math/matrixmath.h
        UtilSources/Math/const.h
        UtilSources/Math/math.h
        UtilSources/Math/math.cpp
        UtilSources/Containers/std_vector_2d.h
        UtilSources/Containers/std_vector_2d.cpp
        UtilSources/Containers/std_utils.h
        UtilSources/template_utils.h
        UtilSources/template_utils.cpp
        UtilSources/Math/FFT.h
        UtilSources/Math/FFT.cpp
        task_3_4.cpp
        task_1_2.cpp
        task_1_2.h task_3_4.h
        OMorisonEquation.cpp OMorisonEquation.h

        main.cpp

        OSysObjEnv.cpp OSysObjEnv.h OSeas.cpp OSeas.h Cases.h AResults.h UtilSources/Math/phys.h UtilSources/Math/trigonometry.h UtilSources/Math/env.h UtilSources/Math/env.cpp OVec.cpp OVec.h)


add_executable(Exam ${SOURCE_FILES})