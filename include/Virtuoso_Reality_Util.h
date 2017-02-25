#ifndef VIRTUOSO_REALITY_UTIL_H
#define VIRTUOSO_REALITY_UTIL_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <iostream>
//Windows
#ifdef _WIN32
#include <windows.h>
#include <map>
#endif

// process related
#if defined(__OS_WINDOWS__)
#include <process.h>
#endif
// usleep
#ifndef usleep1
#define usleep(x) //Sleep( (x/1000 <= 0 ? 1 : x/1000) )
#else
#include <unistd.h>
#endif

//GL Utils
// OpenGL
#if defined(__APPLE__)
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>

#include <GL/glut.h>
#include <GL/glu.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"

//imgui
#include <imgui.h>
#include "imgui_impl_glfw.h"

#include "imguifilesystem.h"
#include "ImGuiUtils.h"

// libsndfile
/*
 #if defined(_USE_SNDFILE_NATIVE_)
 #include <sndfile.h>
 #else
 #include "util_sndfile.h"
 #endif
 */

//#include "util_sndfile.h"

// STK
#include "Thread.h"

//RTAudio
//PortAudio
//#include "RtAudio.h"
#include "portaudio.h"
//PortAudio add here

// libaudiodecoder
#include "audiodecoder.h"

// FFT
#include "chuck_fft.h"

// Marsyas
#include "Centroid.h"
#include "DownSampler.h"
#include "Flux.h"
#include "LPC.h"
#include "MFCC.h"
#include "RMS.h"
#include "Rolloff.h"

#endif
