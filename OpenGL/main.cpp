#include <string>

#include <iostream>
#include <cmath>
// GLEW
#define GLEW_STATIC
#include "GL/glew.h"

// GLFW
#include "GLFW/glfw3.h"

// SOIL  - image loader
#include "SOIL2/SOIL2.h"

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint WIDTH = 800, HEIGHT = 600;

GLfloat mixVal = 0.2f;  // default

void processInput(GLFWwindow *window);
// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

Camera camera( glm::vec3( 1.0f, -1.0f, 3.0f ) );


GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos( 1.2f, 1.0f, 2.0f );

int main() {

    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", nullptr, nullptr);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);  //read screen wdith and height


    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    glfwSetScrollCallback( window, ScrollCallback );

    // Options, removes the mouse cursor for a more immersive experience
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );


    //std::cout << "SCREEN DIMENSION: " << screenWidth*screenHeight << "\n";

    if( window == nullptr ) {
        std::cout << "Failed to create GLFW window!" << "\n";
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if( glewInit() != GLEW_OK ) {
        std::cout << "Failed to initialize GLEW!" << "\n";
        return EXIT_FAILURE;
    }

    glViewport(0, 0, screenWidth, screenHeight);

    // enable depth test
    glEnable( GL_DEPTH_TEST );

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shader program
//    Shader lightShader( "resources/shaders/lighting.vs", "resources/shaders/lighting.frag" );
//    Shader lampShader( "resources/shaders/lamp.vs", "resources/shaders/lamp.frag" );
//    Shader lamp2Shader( "resources/shaders/lamp2.vs", "resources/shaders/lamp2.frag" );
//    Shader lamp3Shader( "resources/shaders/lamp3.vs", "resources/shaders/lamp3.frag" );
    
    Shader shader( "resources/shaders/model.vs", "resources/shaders/model.frag" );
    Model ourModel( "resources/models/nanosuit.obj" );
    
    glm::mat4 projection;
    projection = glm::perspective( camera.GetZoom(), (GLfloat) screenWidth / (GLfloat) screenHeight, 0.1f, 100.f );

    while( !glfwWindowShouldClose( window ) ) {

        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // check if any events are enabled such as key, mouse movements
        glfwPollEvents();
        DoMovement();

        processInput( window );

        // Render
        // clear color buffer
        glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader.Use();
        
//        // COLORS
//
//        GLuint fColorLoc = glGetUniformLocation( shader.Program, "color" );
//        glUniform3f( fColorLoc, 0.75f, 0.67f, 0.25f );
        
        
        GLuint viewPosLoc = glGetUniformLocation( shader.Program, "viewPos");
        glUniform3f( viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z );
        
        // DIRECTIONAL LIGHT
        glUniform3f( glGetUniformLocation( shader.Program, "dirLight.ambient" ), 0.1f, 0.1f, 0.1f );
        glUniform3f( glGetUniformLocation( shader.Program, "dirLight.diffuse" ),0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( shader.Program, "dirLight.specular" ), 1.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( shader.Program, "dirLight.direction" ),  -0.2f, -1.0f, -0.3f );
        
    
        glUniform3f( glGetUniformLocation( shader.Program, "spotLight.position" ), camera.GetPosition().x,  camera.GetPosition().y, camera.GetPosition().z );
        glUniform3f( glGetUniformLocation( shader.Program, "spotLight.direction" ), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z );
        glUniform3f( glGetUniformLocation( shader.Program, "spotLight.ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( shader.Program, "spotLight.diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( shader.Program, "spotLight.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( shader.Program, "spotLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( shader.Program, "spotLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( shader.Program, "spotLight.quadratic" ), 0.032f );
        glUniform1f( glGetUniformLocation( shader.Program, "spotLight.cutOff" ), glm::cos( glm::radians( 5.0f ) ) );
        glUniform1f( glGetUniformLocation( shader.Program, "spotLight.outerCutOff" ), glm::cos( glm::radians( 10.0f ) ) );
        
        glm::mat4 view = camera.GetViewMatrix( );
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        
        // Draw the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate( model, (GLfloat) glfwGetTime(), glm::vec3( 0.0f, 0.0f, 1.0f ) );
        model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
        model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) );    // It's a bit too big for our scene, so scale it down
        
        
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
   
        ourModel.Draw( shader );
        
        // swap screen buffers
        glfwSwapBuffers( window );

    }

    glfwTerminate();

    return EXIT_SUCCESS;

}

void processInput( GLFWwindow *window ) {

    if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwWindowShouldClose( window );
    }

    if( glfwGetKey( window, GLFW_KEY_UP) == GLFW_PRESS ) {
        mixVal += 0.01f;
        if( mixVal >= 1.0f ) {
            mixVal = 1.0f;
        }
    }

    if( glfwGetKey( window, GLFW_KEY_DOWN) == GLFW_PRESS ) {
        mixVal -= 0.01f;
        if( mixVal <= 0.0f ) {
            mixVal = 0.0f;
        }
    }
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }

    if( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }

    if( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }

    if( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if ( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement( xOffset, yOffset );
}


void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset )
{
    camera.ProcessMouseScroll( yOffset );
}


