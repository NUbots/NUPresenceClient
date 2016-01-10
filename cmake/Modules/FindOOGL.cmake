#
#   Copyright 2013 Pixar
#
#   Licensed under the Apache License, Version 2.0 (the "Apache License")
#   with the following modification; you may not use this file except in
#   compliance with the Apache License and the following modification to it:
#   Section 6. Trademarks. is deleted and replaced with:
#
#   6. Trademarks. This License does not grant permission to use the trade
#      names, trademarks, service marks, or product names of the Licensor
#      and its affiliates, except as required to comply with Section 4(c) of
#      the License and to reproduce the content of the NOTICE file.
#
#   You may obtain a copy of the Apache License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the Apache License with the above modification is
#   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#   KIND, either express or implied. See the Apache License for the specific
#   language governing permissions and limitations under the Apache License.
#

# Try to find GLFW library and include path.
# Once done this will define
#
# OOGL_FOUND
# OOGL_INCLUDE_DIR
# OOGL_LIBRARIES
#

find_path( OOGL_INCLUDE_DIR 
    NAMES
        GL/OOGL.hpp
    HINTS
        "${OOGL_LOCATION}/include"
        "$ENV{OOGL_LOCATION}/include"
    PATHS
        "$ENV{PROGRAMFILES}/GLFW/include"
        "${OPENGL_INCLUDE_DIR}"
        /usr/openwin/share/include
        /usr/openwin/include
        /usr/X11R6/include
        /usr/include/X11
        /opt/graphics/OpenGL/include
        /usr/local/include
        /usr/include/GL
        /usr/include
    DOC 
        "The directory where GL/OOGL.hpp resides"
)


find_library( OOGL_LIBRARY 
    NAMES
        OOGL-d
    HINTS
        "${OOGL_LOCATION}/lib"
        "${OOGL_LOCATION}/lib/x64"
        "$ENV{OOGL_LOCATION}/lib"
    PATHS
        "${OPENGL_LIBRARY_DIR}"
        /usr/lib
        /usr/lib/w32api
        /usr/local/lib
        /usr/X11R6/lib
    DOC 
        "The OOGL library"
)

message("OOGL_LIBRARY = " ${OOGL_LIBRARY})
message("OOGL_INCLUDE_DIR = " ${OOGL_INCLUDE_DIR})

