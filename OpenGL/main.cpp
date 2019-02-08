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

Camera camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );

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
    Shader lightShader( "resources/shaders/lighting.vs", "resources/shaders/lighting.frag" );
    Shader lampShader( "resources/shaders/lamp.vs", "resources/shaders/lamp.frag" );
    
    GLfloat vertices[] = {
        0.5f, 0.5f, 0.0f,       0.25f, 0.5f, 0.62f,     1.0f, 1.0f, // Top right
        0.5f, -0.5f, 0.0f,      0.75f, 0.85f, 0.62f,    1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, 0.0f,     0.25f, 0.95f, 0.62f,    0.0f, 0.0f, // Bottom left
        -0.5f, 0.5f, 0.0f,      0.95f, 0.5f, 0.62f,     0.0f, 1.0f  // Top left
    };
    
    GLuint indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3
    };
    
    GLfloat triangles[] = {
//        0.5f, 0.0f, 0.0f,       0.25f, 0.5f, 0.62f,     2.0f, 0.0f,
//        0.25f, 0.5f, 0.0f,      0.95f, 0.5f, 0.62f,     1.0f, 2.0f,
//        0.0f, 0.0f, 0.0f,       0.25f, 0.5f, 0.62f,     0.0f, 0.0f,
//
        0.0f, 0.0f, 0.0f,       0.95f, 0.5f, 0.62f,     2.0f, 0.0f,
        -0.25f, 0.5f, 0.0f,     0.75f, 0.85f, 0.62f,    1.0f, 2.0f,
        -0.5f, 0.0f, 0.0f,      0.95f, 0.5f, 0.62f,     0.0f, 0.0f,
//
//        0.25f, -0.5f, 0.0f,     0.75f, 0.85f, 0.62f,    2.0f, 0.0f,
//        0.0f, 0.0f, 0.0f,       0.25f, 0.5f, 0.62f,     1.0f, 2.0f,
//        -0.25f, -0.5f, 0.0f,    0.75f, 0.85f, 0.62f,    0.0f, 0.0f,
    };
    
    GLfloat cube_vertices[] = {
        // FRONT
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        // BACK
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
        // LEFT
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        
        // RIGHT
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        // BOTTOM
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        // TOP
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    

    // CREATE VERTEX ARRAY OBJECT
    GLuint VAO, VBO, EBO;
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &EBO );
    
    glBindVertexArray( VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW );
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );
    
    // Position
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid *) 0 );
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid *) (3*sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
    
    // Texture
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid *) (3*sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);
    
    glBindVertexArray( 0 );
    
    // VAO for LAMP
    GLuint lampVAO;
    glGenVertexArrays( 1, &lampVAO );
    glBindVertexArray( lampVAO );
    glBindBuffer( GL_ARRAY_BUFFER, VAO ); // bind to VAO since VBO already contains VAO data
    // fill lampVAO with cube vertices
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid *)0 );
    glEnableVertexAttribArray( 0 );
    
    glBindVertexArray( 0 );

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
        
        // CHANGE LIGHT POS OVER TIME
        lightPos.x = 1.0f + sin( glfwGetTime() ) * 2.0f;
        lightPos.y = sin( glfwGetTime() ) / 2.0f;
        lightPos.z = 1.5f * cos( glfwGetTime() );
        
        // LIGHT SHADER
        lightShader.Use();
        GLuint viewPosLoc = glGetUniformLocation( lightShader.Program, "viewPos");
        glUniform3f( viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z );
        
        // MATERIAL COLORS
        GLuint ambientLoc = glGetUniformLocation( lightShader.Program, "material.ambient" );
        GLuint diffuseLoc = glGetUniformLocation( lightShader.Program, "material.diffuse" );
        GLuint specularLoc = glGetUniformLocation( lightShader.Program, "material.specular" );
        GLuint shininessLoc = glGetUniformLocation( lightShader.Program, "material.shininess" );
        
        glUniform3f( ambientLoc, 0.19225f, 0.19225f, 0.19225f );
        glUniform3f( diffuseLoc, 0.50754f, 0.50754f, 0.50754f );
        glUniform3f( specularLoc, 0.508273f, 0.508273f, 0.508273f );
        glUniform1f( shininessLoc, 0.4f * 128.0f );
        
        // LIGHT INTENSITIES
        GLuint ambientLightLoc = glGetUniformLocation( lightShader.Program, "light.ambient" );
        GLuint diffuseLightLoc = glGetUniformLocation( lightShader.Program, "light.diffuse" );
        GLuint specularLightLoc = glGetUniformLocation( lightShader.Program, "light.specular" );
        GLuint lightPosLoc = glGetUniformLocation( lightShader.Program, "light.position" );
        
        // CHANGE COLOR OVER TIME
//        glm::vec3 lightColor;
//        lightColor.x = sin( glfwGetTime() * 1.3f );
//        lightColor.y = sin( glfwGetTime() * 0.7f );
//        lightColor.z = sin( glfwGetTime() * 1.3f );
//
//        glm::vec3 diffuseColor = lightColor * glm::vec3( 0.5f );
//        glm::vec3 ambientColor = diffuseColor * glm::vec3( 0.2f );
//
        glUniform3f( ambientLightLoc, 1.0f, 1.0f, 1.0f );
        glUniform3f( diffuseLightLoc, 1.0f, 1.0f, 1.0f );
        glUniform3f( specularLightLoc, 1.0f, 1.0f, 1.0f );  // we need shiny spot so full intensity
        glUniform3f( lightPosLoc, lightPos.x, lightPos.y, lightPos.z );
        
        // CAMERA VIEW SPACE
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        
        GLuint modelLoc = glGetUniformLocation( lightShader.Program, "model" );
        GLuint viewLoc = glGetUniformLocation( lightShader.Program, "view" );
        GLuint projLoc = glGetUniformLocation( lightShader.Program, "projection" );
        
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( VAO );
        // MODEL MATRIX
        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::scale( model, glm::vec3( 0.5f, 1.5f, 2.0f ) );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        // DRAW THE CONTAINER
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
    
    
        // LAMP SHADER
        lampShader.Use();
        
        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        
        // MODIFY MODEL MATRIX FOR LAMP CONFIGURATION
        model = glm::mat4(1.0f);
        model = glm::translate( model, lightPos );
        model = glm::scale( model, glm::vec3( 0.2f ) );
        
        // MOVE LAMP OVER TIME
//        GLfloat timeValue = glfwGetTime();
//        model = glm::rotate( model, (GLfloat) sin( timeValue ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
//        model = glm::translate( model, glm::vec3( 10.0f, 0.0f, 0.0f ) );
        
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( lampVAO );
        // DRAW THE LAMP
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        // swap screen buffers
        glfwSwapBuffers( window );

    }
    
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glDeleteBuffers( 1, &EBO );
    
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
