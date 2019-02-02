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

const GLuint WIDTH = 800, HEIGHT = 600;

GLfloat mixVal = 0.2f;  // default

void processInput(GLFWwindow *window);

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
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // build and compile shader program
    Shader ourShader( "resources/shaders/lighting.vs", "resources/shaders/lighting.frag" );
    
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
        0.5f, 0.0f, 0.0f,       0.25f, 0.5f, 0.62f,     2.0f, 0.0f,
        0.25f, 0.5f, 0.0f,      0.95f, 0.5f, 0.62f,     1.0f, 2.0f,
        0.0f, 0.0f, 0.0f,       0.25f, 0.5f, 0.62f,     0.0f, 0.0f,
        
        0.0f, 0.0f, 0.0f,       0.95f, 0.5f, 0.62f,     2.0f, 0.0f,
        -0.25f, 0.5f, 0.0f,     0.75f, 0.85f, 0.62f,    1.0f, 2.0f,
        -0.5f, 0.0f, 0.0f,      0.95f, 0.5f, 0.62f,     0.0f, 0.0f,
        
        0.25f, -0.5f, 0.0f,     0.75f, 0.85f, 0.62f,    2.0f, 0.0f,
        0.0f, 0.0f, 0.0f,       0.25f, 0.5f, 0.62f,     1.0f, 2.0f,
        -0.25f, -0.5f, 0.0f,    0.75f, 0.85f, 0.62f,    0.0f, 0.0f,
    };
    
    // CREATE VERTEX ARRAY OBJECT
    GLuint VAO, VBO,EBO;
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &EBO );
    
    glBindVertexArray( VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );
    
    // Position
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)0 );
    glEnableVertexAttribArray(0);
    
    // Color
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (3*sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
    
    // Texture
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *) (6*sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);
    
    glBindVertexArray( 0 );
    
    
    // CREATING TEXTURE
    GLuint texture1, texture2;
    
    int width, height;
    
    glGenTextures( 1, &texture1 );
    glBindTexture( GL_TEXTURE_2D, texture1 );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    unsigned char *image = SOIL_load_image( "resources/images/brick.jpg", &width, &height, 0, SOIL_LOAD_RGB );
    if (image) {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
        glGenerateMipmap( GL_TEXTURE_2D );
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    SOIL_free_image_data( image );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    
    glGenTextures( 1, &texture2 );
    glBindTexture( GL_TEXTURE_2D, texture2 );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    image = SOIL_load_image( "resources/images/cat.png", &width, &height, 0, SOIL_LOAD_RGBA );
    if (image) {
        // passing image data to GL_TEXTURE_2D that is texture2 that was bound above
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    SOIL_free_image_data( image );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    // active shader
    ourShader.Use();
    glUniform1i( glGetUniformLocation( ourShader.Program, "texture1" ), 0 ); // texture1 is now at texture unit 0
    glUniform1i( glGetUniformLocation( ourShader.Program, "texture2" ), 1 );
    
    while( !glfwWindowShouldClose( window ) ) {
        
        // check if any events are enabled such as key, mouse movements
        glfwPollEvents();
        
        processInput( window );
        
        // Render
        // clear color buffer
        glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        
        // DRAW the rectangle
        ourShader.Use();
        
        // Bind texture
        glActiveTexture( GL_TEXTURE0 ); // turning on texture unit location 0 in fragment shader before binding
        glBindTexture( GL_TEXTURE_2D, texture1 );  // bind texture1
        
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, texture2 );
        
        // ANIMATED COLORING
        GLfloat timeValue = glfwGetTime();
        //std::cout << "TIME VALUE: " << timeValue << "\n";
        GLfloat r = sin(timeValue) / 2.0f + 0.5f;
        GLfloat g = cos(timeValue) / 2.0f + 0.5f;
        GLfloat b = sin(timeValue) / 2.0f + 0.8f;

        GLuint colorLoc = glGetUniformLocation( ourShader.Program, "newColor" );
        glUniform4f( colorLoc, r, g, b, 1.0f );  // setting values of newColor vector that is located in fragment shader
        
        // CHANGHING MIXVALUE FOR TEXTURES
        GLuint mixLoc = glGetUniformLocation( ourShader.Program, "mixValue" );
        glUniform1f( mixLoc, mixVal );
        
        // Passing time to frag shader
        GLuint timeLoc = glGetUniformLocation( ourShader.Program, "time" );
        glUniform1f( timeLoc, timeValue );
        
        
        glBindVertexArray( VAO );
        //glDrawArrays( GL_TRIANGLES, 0, 9 );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
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
