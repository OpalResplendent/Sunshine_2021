#include <fstream>

struct shader_program{
	GLuint id;
	char vertexshader[MAX_PATH];
	FILETIME vertexwritetime;
	char fragmentshader[MAX_PATH];
	FILETIME fragmentwritetime;
};

shader_program ShaderPrograms[10];

//GL_VERTEX_SHADER

// LOADING AND RUNNING A SHADER:
//	* glCreateProgram();
//		returns GLuint
//	* glCreateShader(GLenum shaderType);
//		returns GLuint for shader
//	* glShaderSource(GLuint shader, GLsizei count, GLchar** string, GLint length);
//		length can be 0 if count is 1... or something
//	* glCompileShader(GLuint shader);
//	* glGetShaderiv(Gluint shader, GL_COMPILE_STATUS, GLint &return);
//		gets whether or not it compiled
//	* glAttachShader(GLuint program, glUint shader);
//	* glLinkProgram(GLuint program);
//	* glGetProgramiv(GLuint program, GL_LINK_STATUS, GLint &return)
//	* glUseProgram(GLuint program);

// REMOVING SHIT
//	* glUseProgram(NULL);

void DrawText(RO_Text* text);

void
ShaderError(char* errorlog, char* errortype)
{
	if(errorlog[0] == '\0')
	{
		strcpy(errorlog, "Unknown Error: ");
		strcat(errorlog, errortype);
	}
	RO_Text rot = {10, 54,0xFF00FFFF, 1.0, 0, errorlog};
	DrawText(&rot);
	rot = {10.0f-BACKBUFFER_WIDTH, 54, 0xFF00FFFF, 1.0, 0, errorlog};
	DrawText(&rot);
	rot = {10.0f-(BACKBUFFER_WIDTH*2), 54, 0xFF00FFFF, 1.0, 0, errorlog};
	DrawText(&rot);
    
}

GLuint
loadShaderFromFile(std::string path, GLenum shaderType)
{
	//Open file
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile;
	sourceFile.open(path.c_str());
    
	//Source file loaded
	if(sourceFile)
	{
	    //Get shader source
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
        
		//Create shader ID
		shaderID = glCreateShader( shaderType );
        
        //Set shader source
        const GLchar* shaderSource = shaderString.c_str();
        glShaderSource( shaderID, 1, (const GLchar**)&shaderSource, NULL );
        
        //Compile shader source
        glCompileShader(shaderID);
        
        //Check shader for errors
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv( shaderID, GL_COMPILE_STATUS, &shaderCompiled );
        if( shaderCompiled != GL_TRUE )
        {
        	char errorlog[5000] = {'\0'};
			glGetShaderInfoLog(shaderID,5000,0,errorlog);
            
			if(shaderType == GL_VERTEX_SHADER){
				ShaderError(errorlog, "Vertex Shader");
			} else if(shaderType == GL_FRAGMENT_SHADER){
				ShaderError(errorlog, "Fragment Shader");
			} else {
				ShaderError(errorlog, "Incorrect Shader Type");
			}
            
            glDeleteShader( shaderID );
            shaderID = 0;
        }
	}
    else
    {
        printf( "Unable to open file %s\n", path.c_str() );
    }
    
	return shaderID;
}

bool
LoadShader(shader_program* sp)
{
	WIN32_FILE_ATTRIBUTE_DATA vertexfiletime;
	WIN32_FILE_ATTRIBUTE_DATA fragmentfiletime;
    
	GetFileAttributesEx(sp->vertexshader, GetFileExInfoStandard, &vertexfiletime);
	FILETIME newvertextime = vertexfiletime.ftLastWriteTime;
    
	GetFileAttributesEx(sp->fragmentshader, GetFileExInfoStandard, &fragmentfiletime);
	FILETIME newfragmenttime = fragmentfiletime.ftLastWriteTime;
    
	// shouldn't bother to check for id ????????
	if(sp->id == 0 || CompareFileTime(&newvertextime, &sp->vertexwritetime) != 0
       || CompareFileTime(&newfragmenttime, &sp->fragmentwritetime) != 0)
	{
		glDeleteProgram(sp->id);
        
		GLuint vertexID = loadShaderFromFile(sp->vertexshader, GL_VERTEX_SHADER);
        GLuint fragmentID = loadShaderFromFile(sp->fragmentshader, GL_FRAGMENT_SHADER);
        
		GLuint programID = glCreateProgram();
		glAttachShader(programID, vertexID);
		glAttachShader(programID, fragmentID);
        
		glLinkProgram(programID);
        
		GLint linksuccess = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &linksuccess);
		if(!linksuccess && vertexID > 0 && fragmentID > 0)
		{
			char errorlog[5000] = {'\0'};
			glGetProgramInfoLog(programID,5000,0,errorlog);
			ShaderError(errorlog, "Shader Program Linker");
		}
		if(linksuccess && vertexID > 0 && fragmentID > 0){
			glDeleteShader(vertexID);
			glDeleteShader(fragmentID);
		}
        
		sp->id = programID;
        
		//char text[256];
		//sprintf(text, "ID: %u",(unsigned int)sp->id);
		//RO_Text rot = {10, 54, 0xFF00FFFF, 1, 0, text};
        //DrawText(&rot);
        
		return true;
	}
	return false;
}
