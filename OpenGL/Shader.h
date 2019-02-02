#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader( const GLchar *vertexPath, const GLchar *fragmentPath )
    {
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;   // input file stream
        std::ifstream fShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions ( std::ifstream::badbit );
        fShaderFile.exceptions ( std::ifstream::badbit );
        try
        {
            // Open files
            vShaderFile.open( vertexPath );   // file stream accepts input file and opens it
            fShaderFile.open( fragmentPath );

            // Read file's buffer contents into streams
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf( );
            fShaderStream << fShaderFile.rdbuf( );
            
            // close file handlers
            vShaderFile.close( );
            fShaderFile.close( );
            
            // Convert stream into string
            vertexCode = vShaderStream.str( );
            std::cout << "VERTEX CODE: " << vertexCode << std::endl;
            fragmentCode = fShaderStream.str( );
            std::cout << "FRAGMENT CODE: " << fragmentCode << std::endl;
        }
        catch ( std::ifstream::failure e )
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        
        const GLchar *vShaderCode = vertexCode.c_str( );  // separate string into array of chars
        const GLchar *fShaderCode = fragmentCode.c_str( );
        
        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        
        // Vertex Shader
        vertex = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( vertex, 1, &vShaderCode, NULL );
        glCompileShader( vertex );
        
        // Print compile errors if any
        glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
        if ( !success )
        {
            glGetShaderInfoLog( vertex, 512, NULL, infoLog );
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        
        // Fragment Shader
        fragment = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( fragment, 1, &fShaderCode, NULL );
        glCompileShader( fragment );
        
        // Print compile errors if any
        glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
        if ( !success )
        {
            glGetShaderInfoLog( fragment, 512, NULL, infoLog );
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Shader Program
        this->Program = glCreateProgram( );
        glAttachShader( this->Program, vertex );
        glAttachShader( this->Program, fragment );
        glLinkProgram( this->Program );
        // Print linking errors if any
        glGetProgramiv( this->Program, GL_LINK_STATUS, &success );
        if (!success)
        {
            glGetProgramInfoLog( this->Program, 512, NULL, infoLog );
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader( vertex );
        glDeleteShader( fragment );
        
    }
    // Uses the current shader
    void Use( )
    {
        glUseProgram( this->Program );
    }
};

#endif