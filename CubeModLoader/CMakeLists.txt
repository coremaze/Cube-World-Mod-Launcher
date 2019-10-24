cmake_minimum_required (VERSION 3.8)
project(project_CubeModLoader)
add_library (CubeModLoader SHARED 
    crc.cpp
    DLL.cpp
    main.cpp
    mutex.cpp)