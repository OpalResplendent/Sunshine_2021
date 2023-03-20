typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

#define GL_ARRAY_BUFFER                   0x8892
#define GL_COMPILE_STATUS                 0x8B81
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
#define GL_STATIC_DRAW                    0x88E4
#define GL_VERTEX_SHADER                  0x8B31
#define GL_ARRAY_BUFFER				   0x8892
#define GL_STATIC_DRAW					0x88E4

#define GL_SAMPLE_BUFFERS_ARB                0x2041

#define MULTISAMPLE_ARB                      0x809D
#define SAMPLE_ALPHA_TO_COVERAGE_ARB         0x809E
#define SAMPLE_ALPHA_TO_ONE_ARB              0x809F
#define SAMPLE_COVERAGE_ARB                  0x80A0

#define MULTISAMPLE_BIT_ARB                  0x20000000

#define SAMPLE_BUFFERS_ARB                   0x80A8
#define SAMPLES_ARB                          0x80A9
#define SAMPLE_COVERAGE_VALUE_ARB            0x80AA
#define SAMPLE_COVERAGE_INVERT_ARB           0x80AB


typedef void _glAttachShader (GLuint program, GLuint shader);
typedef void _glBindBuffer (GLenum target, GLuint buffer);
typedef void _glBindVertexArray (GLuint array);
typedef void _glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef GLuint _glCreateProgram (void);
typedef GLuint _glCreateShader (GLuint index);
typedef void _glCompileShader (GLuint shader);
typedef void _glDeleteBuffers (GLsizei n, const GLuint *buffers);
typedef void _glDeleteProgram (GLuint program);
typedef void _glDeleteShader (GLuint shader);
typedef void _glDisableVertexAttribArray (GLuint index);
typedef void _glEnableVertexAttribArray(GLuint index);
typedef void _glGenBuffers (GLsizei n, GLuint *buffers);
typedef void _glGenVertexArrays (GLsizei n, GLuint *arrays);
typedef void _glGenerateMipmap(GLenum target);
typedef void _glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void _glGetProgramiv (GLuint program, GLenum pname, GLint *params);
typedef void _glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void _glGetShaderiv (GLuint shader, GLenum pname, GLint *params);
typedef GLint _glGetUniformLocation (GLuint program, const GLchar *name);
typedef void _glLinkProgram (GLuint program);
typedef void _glShaderSource (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void _glUniform1i (GLint location, GLint v0);
typedef void _glUniform1ui (GLint location, GLuint v0);
typedef void _glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void _glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void _glUseProgram (GLuint program);
typedef void _glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
//typedef void _glLoadMatrix
typedef void _glDeleteVertexArrays (GLsizei n, const GLuint *arrays);
typedef void _glUniform1f (GLint location, GLfloat v0);
typedef void _glUniform2f (GLint location, GLfloat v0, GLfloat v1);
typedef void _glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

_glAttachShader *glAttachShader = 0;
_glBindBuffer *glBindBuffer = 0;
_glBindVertexArray *glBindVertexArray = 0;
_glBufferData *glBufferData = 0;
_glCreateProgram *glCreateProgram = 0;
_glCreateShader *glCreateShader = 0;
_glCompileShader *glCompileShader = 0;
_glDeleteBuffers *glDeleteBuffers = 0;
_glDeleteProgram *glDeleteProgram = 0;
_glDeleteShader *glDeleteShader = 0;
_glDisableVertexAttribArray *glDisableVertexAttribArray = 0;
_glEnableVertexAttribArray *glEnableVertexAttribArray = 0;
_glGenBuffers *glGenBuffers = 0;
_glGenVertexArrays *glGenVertexArrays = 0;
_glGenerateMipmap *glGenerateMipmap = 0;
_glGetProgramInfoLog *glGetProgramInfoLog = 0;
_glGetProgramiv *glGetProgramiv = 0;
_glGetShaderInfoLog *glGetShaderInfoLog = 0;
_glGetShaderiv *glGetShaderiv = 0;
_glGetUniformLocation *glGetUniformLocation = 0;
_glLinkProgram *glLinkProgram = 0;
_glShaderSource *glShaderSource = 0;
_glUniform1i *glUniform1i = 0;
_glUniform1ui *glUniform1ui = 0;
_glUniform4f *glUniform4f = 0;
_glUniformMatrix4fv *glUniformMatrix4fv = 0;
_glUseProgram *glUseProgram = 0;
_glVertexAttribPointer *glVertexAttribPointer = 0;

_glDeleteVertexArrays *glDeleteVertexArrays = 0;
_glUniform1f *glUniform1f = 0;
_glUniform2f *glUniform2f = 0;
_glUniform3f *glUniform3f = 0;

int win_initOGL()
{
    glAttachShader = (_glAttachShader *)wglGetProcAddress("glAttachShader");
    glBindBuffer = (_glBindBuffer *)wglGetProcAddress("glBindBuffer");
    glBindVertexArray = (_glBindVertexArray *)wglGetProcAddress("glBindVertexArray");
    glBufferData = (_glBufferData *)wglGetProcAddress("glBufferData");
    glCreateProgram = (_glCreateProgram *)wglGetProcAddress("glCreateProgram");
    glCreateShader = (_glCreateShader *)wglGetProcAddress("glCreateShader");
    glCompileShader = (_glCompileShader *)wglGetProcAddress("glCompileShader");
    glDeleteBuffers = (_glDeleteBuffers *)wglGetProcAddress("glDeleteBuffers");
    glDeleteProgram = (_glDeleteProgram *)wglGetProcAddress("glDeleteProgram");
    glDeleteShader = (_glDeleteShader *)wglGetProcAddress("glDeleteShader");
    glDisableVertexAttribArray = (_glDisableVertexAttribArray *)wglGetProcAddress("glDisableVertexAttribArray");
    glEnableVertexAttribArray = (_glEnableVertexAttribArray *)wglGetProcAddress("glEnableVertexAttribArray");
    glGenBuffers = (_glGenBuffers *)wglGetProcAddress("glGenBuffers");
    glGenVertexArrays = (_glGenVertexArrays *)wglGetProcAddress("glGenVertexArrays");
    glGetProgramInfoLog = (_glGetProgramInfoLog *)wglGetProcAddress("glGetProgramInfoLog");
    glGetProgramiv = (_glGetProgramiv *)wglGetProcAddress("glGetProgramiv");
    glGetShaderInfoLog = (_glGetShaderInfoLog *)wglGetProcAddress("glGetShaderInfoLog");
    glGetShaderiv = (_glGetShaderiv *)wglGetProcAddress("glGetShaderiv");
    glGetUniformLocation = (_glGetUniformLocation *)wglGetProcAddress("glGetUniformLocation");
    glLinkProgram = (_glLinkProgram *)wglGetProcAddress("glLinkProgram");
    glShaderSource = (_glShaderSource *)wglGetProcAddress("glShaderSource");
    glUniform1i = (_glUniform1i *)wglGetProcAddress("glUniform1i");
    glUniform1ui = (_glUniform1ui *)wglGetProcAddress("glUniform1ui");
    glUniform4f = (_glUniform4f *)wglGetProcAddress("glUniform4f");
    glUniformMatrix4fv = (_glUniformMatrix4fv *)wglGetProcAddress("glUniformMatrix4fv");
    glUseProgram = (_glUseProgram *)wglGetProcAddress("glUseProgram");
    glVertexAttribPointer = (_glVertexAttribPointer *)wglGetProcAddress("glVertexAttribPointer");
    glGenVertexArrays = (_glGenVertexArrays *)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray *)wglGetProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (_glEnableVertexAttribArray *)wglGetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (_glDisableVertexAttribArray *)wglGetProcAddress("glDisableVertexAttribArray");
    glVertexAttribPointer = (_glVertexAttribPointer *)wglGetProcAddress("glVertexAttribPointer");
    glGenBuffers = (_glGenBuffers *)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (_glBindBuffer *)wglGetProcAddress("glBindBuffer");
    glBufferData = (_glBufferData *)wglGetProcAddress("glBufferData");
    glGenerateMipmap = (_glGenerateMipmap *)wglGetProcAddress("glGenerateMipmap");
    
    glDeleteVertexArrays = (_glDeleteVertexArrays *)wglGetProcAddress("glDeleteVertexArrays");
    glUniform1f= (_glUniform1f *)wglGetProcAddress("glUniform1f");
    glUniform2f= (_glUniform2f *)wglGetProcAddress("glUniform2f");
    glUniform3f= (_glUniform3f *)wglGetProcAddress("glUniform3f");
    
    return 0;
}