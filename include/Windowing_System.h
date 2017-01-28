#ifndef WINDOWING_SYSTEM_H
#define WINDOWING_SYSTEM_H

#include "Virtuoso_Reality_Util.h"

class WindowingSystem{
public:
    WindowingSystem();
    Camera * camera;
    static GLFWwindow *window;
    
    GLFWwindow * getGLFWwindow();
    Camera * getCamera();
    
    static void error_callback(int error, const char * description);
    
    
    unsigned char getKey(unsigned char key);
    
    
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    
    int g_width = 1280;
    int g_height = 800;
    int g_last_width = 0;
    int g_last_height = 0;
    GLfloat g_eye_y = 0.0f; // 0.2f for stria, otherwise 0
    
    bool keys[1024];
    GLfloat lastX = 400, lastY = 300;
    bool firstMouse = true;
    
    int t=0;
    glm::mat4 view;
    
private:
protected:
};

#endif
