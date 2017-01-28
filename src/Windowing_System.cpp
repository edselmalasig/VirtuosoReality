#include "Windowing_System.h"

GLFWwindow * WindowingSystem::window;

WindowingSystem::WindowingSystem(){
    
    camera = new Camera(0.0f, 3.5f * sin( g_eye_y ),
                                                3.5f * cos( g_eye_y ),
                                                0.0f,
                                                ( cos( g_eye_y ) < 0 ? -1.0f : 1.0f ),
                                                0.0f,
                        -90, 0.0);
    
    // Setup SDL
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#if __APPLE__
      //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    this->window = glfwCreateWindow(1280, 800, "Virtuoso Reality Techno Preview", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        cout << "Error: " << glewGetErrorString(err) << endl;
    }
    
    glViewport(0, 0, g_width, g_height);
}

Camera * WindowingSystem::getCamera(){
    return this->camera;
}


void WindowingSystem::error_callback(int error, const char* description){
    fprintf(stderr, "Error %d: %s\n", error, description);
}

