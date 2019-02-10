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

Camera camera( glm::vec3( 2.0f, -1.0f, 3.0f ) );

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
    Shader lamp2Shader( "resources/shaders/lamp2.vs", "resources/shaders/lamp2.frag" );
    Shader lamp3Shader( "resources/shaders/lamp3.vs", "resources/shaders/lamp3.frag" );
    
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
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    glm::vec3 translations[] = {
        glm::vec3( 1.5f,  -2.0f,  1.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f),
        
        glm::vec3( 1.8f,  2.5f, -1.5f),
        glm::vec3( 3.0f,  1.0f, -3.0f),
        glm::vec3( 0.0f,  -1.2f, 3.0f),
        glm::vec3( -2.0f,  1.8f, -3.0f),
        
        glm::vec3( -3.0f,  3.8f, -5.0f),
        glm::vec3( 2.2f,  -4.2f, -4.0f),
        glm::vec3( -3.0f,  1.8f, -1.0f),
        glm::vec3( 2.4f,  4.0f, -5.0f)
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
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) 0 );
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (3*sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
    
    // Texture
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (6*sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);
    
    glBindVertexArray( 0 );
    
    // VAO for LAMP
    GLuint lampVAO;
    glGenVertexArrays( 1, &lampVAO );
    glBindVertexArray( lampVAO );
    glBindBuffer( GL_ARRAY_BUFFER, VAO ); // bind to VAO since VBO already contains VAO data
    // fill lampVAO with cube vertices
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)0 );
    glEnableVertexAttribArray( 0 );
    
    glBindVertexArray( 0 );
    
    
    // CREATING TEXTURE
    GLuint diffuseMap;
    
    int width, height;
    
    glGenTextures( 1, &diffuseMap );
    glBindTexture( GL_TEXTURE_2D, diffuseMap );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    unsigned char *image = SOIL_load_image( "resources/images/container2.png", &width, &height, 0, SOIL_LOAD_RGB );
    if (image) {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
        glGenerateMipmap( GL_TEXTURE_2D );
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    SOIL_free_image_data( image );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    
    GLuint specularMap;
    
    glGenTextures( 1, &specularMap );
    glBindTexture( GL_TEXTURE_2D, specularMap );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    image = SOIL_load_image( "resources/images/emmision_letters.jpg", &width, &height, 0, SOIL_LOAD_RGB );
    if (image) {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
        glGenerateMipmap( GL_TEXTURE_2D );
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    SOIL_free_image_data( image );
    glBindTexture( GL_TEXTURE_2D, 0 );
    

    // active shader
    lightShader.Use();
    glUniform1i( glGetUniformLocation( lightShader.Program, "material.diffuse" ), 0 ); // diffuseMap is now at texture unit 0
    glUniform1i( glGetUniformLocation( lightShader.Program, "material.specular" ), 1 );

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
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // CHANGE LIGHT POS OVER TIME
//        lightPos.x = 1.0f + sin( glfwGetTime() ) * 2.0f;
//        lightPos.y = sin( glfwGetTime() ) / 2.0f;
//        lightPos.z = 1.5f * cos( glfwGetTime() );

        // LIGHT SHADER
        lightShader.Use();

        // Bind texture
        glActiveTexture( GL_TEXTURE0 ); // turning on texture unit location 0 in fragment shader before binding
        glBindTexture( GL_TEXTURE_2D, diffuseMap );  // bind texture1
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, specularMap );

        // View Position
        GLuint viewPosLoc = glGetUniformLocation( lightShader.Program, "viewPos");
        glUniform3f( viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z );
        GLuint shininessLoc = glGetUniformLocation( lightShader.Program, "material.shininess" );
        glUniform1f( shininessLoc, 32.0f );
        // MATERIAL COLORS
//        GLuint ambientLoc = glGetUniformLocation( lightShader.Program, "material.ambient" );
//        GLuint diffuseLoc = glGetUniformLocation( lightShader.Program, "material.diffuse" );
//        GLuint specularLoc = glGetUniformLocation( lightShader.Program, "material.specular" );
//        glUniform3f( ambientLoc, 0.24725f, 0.1995f, 0.0745f );
//        glUniform3f( diffuseLoc, 0.0f, 0.0f, 0.0f );
//        glUniform3f( specularLoc, 1.0f, 1.0f, 1.0f );
        
        // DIRECTIONAL LIGHT
        glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.ambient" ), 0.1f, 0.1f, 0.1f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.diffuse" ), 1.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.specular" ), 1.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.direction" ),  -0.2f, -1.0f, -0.3f );
    
        // POINT LIGHTS (LAMPS)
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].position" ), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].diffuse" ), 0.0f, 0.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].specular" ), 0.0f, 0.0f, 1.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[0].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[0].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[0].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].position" ), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].diffuse" ), 0.0f, 0.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].specular" ), 0.0f, 0.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[1].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[1].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[1].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].position" ), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].diffuse" ), 0.0f, 0.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].specular" ), 0.0f, 0.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[2].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[2].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[2].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].position" ), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].diffuse" ), 0.0f, 0.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].specular" ), 0.0f, 0.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[3].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[3].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[3].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[4].position" ), pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[4].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[4].diffuse" ), 1.0f, 1.0f, 0.0f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[4].specular" ), 1.0f, 1.0f, 0.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[4].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[4].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[4].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[5].position" ), pointLightPositions[5].x, pointLightPositions[5].y, pointLightPositions[5].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[5].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[5].diffuse" ), 1.0f, 1.0f, 0.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[5].specular" ), 1.0f, 1.0f, 0.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[5].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[5].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[5].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[6].position" ), pointLightPositions[6].x, pointLightPositions[6].y, pointLightPositions[6].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[6].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[6].diffuse" ), 1.0f, 1.0f, 0.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[6].specular" ), 1.0f, 1.0f, 0.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[6].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[6].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[6].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[7].position" ), pointLightPositions[7].x, pointLightPositions[7].y, pointLightPositions[7].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[7].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[7].diffuse" ), 1.0f, 1.0f, 0.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[7].specular" ), 1.0f, 1.0f, 0.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[7].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[7].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[7].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[8].position" ), pointLightPositions[8].x, pointLightPositions[8].y, pointLightPositions[8].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[8].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[8].diffuse" ), 1.0f, 0.0f, 1.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[8].specular" ), 1.0f, 0.0f, 1.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[8].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[8].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[8].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[9].position" ), pointLightPositions[9].x, pointLightPositions[9].y, pointLightPositions[9].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[9].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[9].diffuse" ), 1.0f, 0.0f, 1.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[9].specular" ), 1.0f, 0.0f, 1.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[9].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[9].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[9].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[10].position" ), pointLightPositions[10].x, pointLightPositions[10].y, pointLightPositions[10].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[10].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[10].diffuse" ), 1.0f, 0.0f, 1.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[10].specular" ), 1.0f, 0.0f, 1.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[10].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[10].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[10].quadratic" ), 0.032f );
        
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[11].position" ), pointLightPositions[11].x, pointLightPositions[11].y, pointLightPositions[11].z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[11].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[11].diffuse" ), 1.0f, 0.0f, 1.0f  );
        glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[11].specular" ), 1.0f, 0.0f, 1.0f  );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[11].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[11].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[11].quadratic" ), 0.032f );
        
        
        
        // SPOT (FLASH) LIGHT
        glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.position" ), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.direction" ), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z );
        glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.specular" ), 1.0f, 0.8f, 0.8f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.quadratic" ), 0.032f );
        glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.cutOff" ), glm::cos( glm::radians( 5.0f ) ) );
        glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.outerCutOff" ), glm::cos( glm::radians( 10.0f ) ) );
        
        
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        
        
        // CHANGE COLOR OVER TIME
//        glm::vec3 lightColor;
//        lightColor.x = sin( glfwGetTime() * 1.3f );
//        lightColor.y = sin( glfwGetTime() * 0.7f );
//        lightColor.z = sin( glfwGetTime() * 1.3f );
//
//        glm::vec3 diffuseColor = lightColor * glm::vec3( 0.5f );
//        glm::vec3 ambientColor = diffuseColor * glm::vec3( 0.2f );
        
        GLuint modelLoc = glGetUniformLocation( lightShader.Program, "model" );
        GLuint viewLoc = glGetUniformLocation( lightShader.Program, "view" );
        GLuint projLoc = glGetUniformLocation( lightShader.Program, "projection" );
        
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( VAO );
        
        // MODEL MATRIX
        for(GLuint i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate( model, translations[i] );
            GLfloat angle = glfwGetTime() * i;
            model = glm::rotate( model, angle / 5.0f, glm::vec3( 1.0f, 0.3f, 0.5f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            // DRAW THE CONTAINER
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        
        glBindVertexArray( 0 );
        
        
        // LAMP SHADER
        lampShader.Use();

        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate( model, lightPos );
        model = glm::scale( model, glm::vec3( 0.2f ) );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
         //MOVE LAMP OVER TIME
//        GLfloat timeValue = glfwGetTime();
//        model = glm::rotate( model, (GLfloat) sin( timeValue ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
//        model = glm::translate( model, glm::vec3( 10.0f, 0.0f, 0.0f ) );
   
        glBindVertexArray( lampVAO );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        // DRAW THE LAMP
        glBindVertexArray( lampVAO );
        
        for(GLuint i = 0; i < 4; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate( model, pointLightPositions[i] );
            model = glm::scale( model, glm::vec3( 0.2f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        
        glBindVertexArray( 0 );
        
        
        // YELLOW LAMPS
        lamp2Shader.Use();
        
        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        model = glm::mat4(1.0f);
        model = glm::translate( model, lightPos * 0.5f );
        model = glm::scale( model, glm::vec3( 0.2f ) );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        
//        MOVE LAMP OVER TIME
//        GLfloat timeValue = glfwGetTime();
//        model = glm::rotate( model, (GLfloat) sin( timeValue ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
//        model = glm::translate( model, glm::vec3( 10.0f, 0.0f, 0.0f ) );
        
        glBindVertexArray( lampVAO );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        // DRAW THE LAMP
        glBindVertexArray( lampVAO );
        
        for(GLuint i = 4; i < 8; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate( model, pointLightPositions[i] );
            model = glm::scale( model, glm::vec3( 0.2f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        
        glBindVertexArray( 0 );
        
        // PINK LAMPS
        lamp3Shader.Use();
        
        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        model = glm::mat4(1.0f);
        model = glm::translate( model, lightPos * 0.2f );
        model = glm::scale( model, glm::vec3( 0.2f ) );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        
        //        MOVE LAMP OVER TIME
        //        GLfloat timeValue = glfwGetTime();
        //        model = glm::rotate( model, (GLfloat) sin( timeValue ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
        //        model = glm::translate( model, glm::vec3( 10.0f, 0.0f, 0.0f ) );
        
        glBindVertexArray( lampVAO );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        // DRAW THE LAMP
        glBindVertexArray( lampVAO );
        
        for(GLuint i = 8; i < 12; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate( model, pointLightPositions[i] );
            model = glm::scale( model, glm::vec3( 0.2f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        
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
