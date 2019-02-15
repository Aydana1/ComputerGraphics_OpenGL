#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// GLFW
#include "GLFW/glfw3.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SOIL2/SOIL2.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;

GLint TextureFromFile( const char *path, string directory );

GLboolean textureExists = true;

class Model
{
public:
    // filepath to a 3D model
    Model( GLchar *path )
    {
        this->loadModel( path );
    }
    // Draws the model, and thus all its meshes
    void Draw( Shader shader )
    {
        
        if( !textureExists ) {

            glUniform1f( glGetUniformLocation( shader.Program, "materialOn" ), GL_FALSE );

            glUniform3f( glGetUniformLocation( shader.Program, "object.ambient" ), 0.1f, 0.1f, 0.1f );
            glUniform3f( glGetUniformLocation( shader.Program, "object.diffuse" ), 1.0f, 1.0f, 0.0f );
            glUniform3f( glGetUniformLocation( shader.Program, "object.specular" ), 1.0f, 1.0f, 0.0f );

        } else {
            glUniform1f( glGetUniformLocation( shader.Program, "materialOn" ), GL_TRUE );

        }
        
        for ( GLuint i = 0; i < this->meshes.size( ); i++ )
        {
            this->meshes[i].Draw( shader );
        }
    }
private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;
    
    // Functions
    // Loads a model with ASSIMP extensions from files and stores resulting meshes into meshes vector
    void loadModel( string path )
    {
        Assimp::Importer importer;
        
        const aiScene *scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs );
        
        // Check for errors
        if( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString( ) << endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr( 0, path.find_last_of( '/' ) );
        
        // Process ASSIMP's root node recursively
        this->processNode( scene->mRootNode, scene );
    };
    
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode( aiNode* node, const aiScene* scene )
    {
        // Process each mesh located at the current node
        for ( GLuint i = 0; i < node->mNumMeshes; i++ )
        {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            
            this->meshes.push_back( this->processMesh( mesh, scene ) );
        }
        
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for ( GLuint i = 0; i < node->mNumChildren; i++ )
        {
            this->processNode( node->mChildren[i], scene );
        }
    }
    
    Mesh processMesh( aiMesh *mesh, const aiScene *scene )
    {
        // DATA to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        
        // Retireve mesh vertices
        for( GLuint i = 0; i < mesh->mNumVertices; i++ )
        {
            Vertex vertex;
            glm::vec3 vector;
            
            // Position
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
          
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            
            // Textures
            if( mesh->mTextureCoords[0] )
            {
                glm::vec2 vec;
//                std::cout << "TEXTURE EXISTS!" << std::endl;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
    
//                vertex.TexCoords = glm::vec2( 0.0f, 0.0f );
                
                vertex.TexCoords = glm::vec2( vertex.Position.x*1.2f, vertex.Position.y*1.2f );

                TextureFromFile( "cat.png", "resources/images" );
                
                textureExists = false;

            }
            
            // Colors
            vector.x = 0.0000055f * i;
            vector.y = 0.0000029f * i;
            vector.z = 0.000099f * i;
            
            vertex.Color = vector;
            
            vertices.push_back( vertex );
        }
        
        // Retrieve mesh faces
        for( GLuint i = 0; i < mesh->mNumFaces; i++ )
        {
            aiFace face = mesh->mFaces[i];
            for( GLuint j = 0; j < face.mNumIndices; j++ )
            {
                indices.push_back( face.mIndices[j] );
            }
        }
        
        // Process materials if there exist ones
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            
            vector<Texture> diffuseMaps = loadMaterialTextures( material, aiTextureType_DIFFUSE, "texture_diffuse" );
            textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );
            
            vector<Texture> specularMaps = this->loadMaterialTextures( material, aiTextureType_SPECULAR, "texture_specular" );
            textures.insert( textures.end( ), specularMaps.begin( ), specularMaps.end( ) );
        }
        
        return Mesh( vertices, indices, textures );
    }
    
    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures( aiMaterial *mat, aiTextureType type, string typeName )
    {
        vector<Texture> textures;
//        std::cout << "TEXTURE COUNT...!"  << mat->GetTextureCount( type ) << std::endl;
        for ( GLuint i = 0; i < mat->GetTextureCount( type ); i++ )
        {
            aiString str;
            mat->GetTexture( type, i, &str );
            
           //  Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;

            for ( GLuint j = 0; j < textures_loaded.size( ); j++ )
            {
                if( textures_loaded[j].path == str )
                {
                    textures.push_back( textures_loaded[j] );
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)

                    break;
                }
            }
            
//            if( skip )
//            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile( str.C_Str( ), this->directory );
                std::cout << "TEXTURE ID: " << texture.id << std::endl;
                texture.type = typeName;
                texture.path = str;
                textures.push_back( texture );
                
                this->textures_loaded.push_back( texture );  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//            }
        }
        
        return textures;
    }
};

GLint TextureFromFile( const char *path, string directory )
{
    //Generate texture ID and load texture data
    string filename = string( path );  // filename here is an image name
    filename = "cat.png";
    std::cout << "PATH: " << filename << std::endl;
//    filename = directory + '/' + filename;
    filename = "resources/images/"  + filename;
    std::cout << "FILENAME: " << filename << std::endl;

    GLuint textureID;
    glGenTextures( 1, &textureID );
    
    int width, height;
    
    unsigned char *image = SOIL_load_image( filename.c_str( ), &width, &height, 0, SOIL_LOAD_RGB );
    
    // Assign texture to ID
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture( GL_TEXTURE_2D, 0 );
    SOIL_free_image_data( image );
    
    return textureID;
}

