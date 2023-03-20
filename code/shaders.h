#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>

class Shader
{
    public:
    unsigned int ID;
    
    Shader(const char* vertexPath, const char* fragmentPath);
    
    void use();
    
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    
    
    private:
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
