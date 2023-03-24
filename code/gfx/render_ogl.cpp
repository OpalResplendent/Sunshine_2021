
// TODO: get available multisampling options
// TODO: reversed-z
// TODO: pick OGL version to target

#define GLEW_STATIC
#include <glew/glew.h>
#include <glew/wglew.h>
//#include <glad/glad.h>
//#include <gl/gl.h>
//#include <gl/glext.h>

#include <stdio.h>

#include "render.h"

#ifndef __glew_h__
#include "win32_ogl_defs.cpp"
#endif

#include "render_ogl_shader.cpp"

#ifndef __glew_h__
_wglCreateContextAttribsARB *wglCreateContextAttribsARB = 0;
_wglChoosePixelFormatARB *wglChoosePixelFormatARB = 0;
_wglSwapIntervalEXT *wglSwapIntervalEXT = 0;
#endif

typedef HGLRC _wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL _wglChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef BOOL _wglSwapIntervalEXT(int interval);

#define WGL_SAMPLE_BUFFERS_ARB			      0x2041
#define WGL_SAMPLES_ARB                         0x2042

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// error
#define ERROR_INVALID_VERSION_ARB			   0x2095
#define ERROR_INVALID_PROFILE_ARB			   0x2096

// stupid shit:
#define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_DRAW_TO_BITMAP_ARB                  0x2002
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_NEED_PALETTE_ARB                    0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
#define WGL_SWAP_METHOD_ARB                     0x2007
#define WGL_NUMBER_OVERLAYS_ARB                 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                0x2009
#define WGL_TRANSPARENT_ARB                     0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
#define WGL_SHARE_DEPTH_ARB                     0x200C
#define WGL_SHARE_STENCIL_ARB                   0x200D
#define WGL_SHARE_ACCUM_ARB                     0x200E
#define WGL_SUPPORT_GDI_ARB                     0x200F
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_STEREO_ARB                          0x2012
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_RED_SHIFT_ARB                       0x2016
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_GREEN_SHIFT_ARB                     0x2018
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_BLUE_SHIFT_ARB                      0x201A
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_ALPHA_SHIFT_ARB                     0x201C
#define WGL_ACCUM_BITS_ARB                      0x201D
#define WGL_ACCUM_RED_BITS_ARB                  0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023
#define WGL_AUX_BUFFERS_ARB                     0x2024

#define WGL_NO_ACCELERATION_ARB                 0x2025
#define WGL_GENERIC_ACCELERATION_ARB            0x2026
#define WGL_FULL_ACCELERATION_ARB               0x2027

#define WGL_SWAP_EXCHANGE_ARB                   0x2028
#define WGL_SWAP_COPY_ARB                       0x2029
#define WGL_SWAP_UNDEFINED_ARB                  0x202A

#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_TYPE_COLORINDEX_ARB                 0x202C

// end stupid shit

#define ATTRIBUTE_LOCATION_POSITIONS 0x0
#define ATTRIBUTE_LOCATION_TEXTUREUV 0x1


/*
typedef void gl_draw_elements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices);
gl_draw_elements *glDrawElements = 0;


void glVertexAttribPointer(	GLuint index,
  GLint size,
  GLenum type,
  GLboolean normalized,
  GLsizei stride,
  const GLvoid * pointer);
*/

struct image_info {
	uint32 width, height;
	uint32 tilewidth, tileheight;
	uint32 tileswide, tilestall;
	uint32 offsetx, offsety;
};

#define MAX_IMAGES_LOADED 10
image_info Images[MAX_IMAGES_LOADED];
uint32 ImagesLoaded;

uint8 asciitooryx[128];

#if 0
uint32
LoadImageOGL(char* filename, image_info* io)
{
	int32 w = 0;
	int32 h = 0;
	int32 comp = 0;
    
	uint8* temptexture = 0;
	GLuint temptextureindex = 0;
    
	temptexture = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);
    
	glGenTextures(1, &temptextureindex);
	glBindTexture(GL_TEXTURE_2D, temptextureindex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, temptexture);
	free(temptexture);
    
	io->width = w;
	io->height = h;
	Images[ImagesLoaded] = *io;
	return ImagesLoaded++;
}
#endif

// NVIDIA hack to use GPU instead of integrated graphics
extern "C"
{
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int
win_initOpenGL(HINSTANCE hInstance)
{
	// make dummy window
    WNDCLASSA dummywindowclass = {0};
    dummywindowclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    dummywindowclass.lpfnWndProc = DefWindowProcA;
    dummywindowclass.hInstance = GetModuleHandle(0);
    dummywindowclass.lpszClassName = "DummyWindowClass";
    
    if(!RegisterClass(&dummywindowclass))
    {
        MessageBox(NULL, "OGL Dummy Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }
    
    HWND dummyWindow = CreateWindowEx(0,
                                      dummywindowclass.lpszClassName,
                                      "Dummy Window Title",
                                      0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0,0,
                                      dummywindowclass.hInstance,
                                      0);
    
    HDC dummyDC = GetDC(dummyWindow);
    
    // desired pixel format for dummy context
	PIXELFORMATDESCRIPTOR dummyPF = {};
	dummyPF.nSize = sizeof(dummyPF);
	dummyPF.nVersion = 1;
	dummyPF.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	dummyPF.iPixelType = PFD_TYPE_RGBA;
	dummyPF.cColorBits = 32;
	dummyPF.cDepthBits = 24;
	dummyPF.cStencilBits = 8;
    dummyPF.iLayerType = PFD_MAIN_PLANE; // depricated?
    
    PIXELFORMATDESCRIPTOR dummySPF = {};
    
    // get pixelformat corresponding to our desired pixelformat
	int dummySPFI = ChoosePixelFormat(dummyDC, &dummyPF);
	DescribePixelFormat(dummyDC, dummySPFI,
                        sizeof(dummySPF), &dummySPF);
	SetPixelFormat(dummyDC, dummySPFI, &dummySPF);
    
	HGLRC dummyContext = wglCreateContext(dummyDC);
    
    if(dummyContext == 0)
    {
        MessageBox(NULL, "Dummy Context Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return(1);
    }
    
	if(wglMakeCurrent(dummyDC, dummyContext) == 0)
	{
        MessageBox(NULL, "Dummy MakeCurrent Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return(1);
    }
    
    
    // grab all the extensions
    // critical windows extensions
    wglCreateContextAttribsARB = (_wglCreateContextAttribsARB *)wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (_wglChoosePixelFormatARB *)wglGetProcAddress("wglChoosePixelFormatARB");
    
    // nice to have windows extensions
    wglSwapIntervalEXT = (_wglSwapIntervalEXT *)wglGetProcAddress("wglSwapIntervalEXT");
    
    // general ogl extensions
    //win_initOGL();
    
    // finally destroy the dummy context and window
    wglMakeCurrent(dummyDC, 0);
    wglDeleteContext(dummyContext);
    ReleaseDC(dummyWindow, dummyDC);
    DestroyWindow(dummyWindow);
    
    // finally start doing shit
    MainWindowHandle = win_createwindow(hInstance);
    HDC RealDC = GetDC(MainWindowHandle);
    
    GLint context_attributes[] = {
        //WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        //WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        //WGL_SAMPLE_BUFFERS_ARB, 1,
        //WGL_SAMPLES_ARB, 4,
        0
    };
    
    GLint attributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        //WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
        //WGL_SAMPLES_ARB, 4,        // Number of samples
        0
    };
    
    const int attribList[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0, // End
    };
    
    int pixelFormats[10] = {};
    UINT numFormats = 1;
    int maxFormats = 10;
    wglChoosePixelFormatARB(RealDC, attribList, 0, maxFormats, pixelFormats, &numFormats);
    
    PIXELFORMATDESCRIPTOR PixelFormatDescriptor = {};
    int faildescribe = DescribePixelFormat(RealDC, pixelFormats[0],
                                           sizeof(PixelFormatDescriptor), &PixelFormatDescriptor);
    int error = GetLastError();
	int failpixel = SetPixelFormat(RealDC, pixelFormats[0], &PixelFormatDescriptor);
    error = GetLastError();
    
    HGLRC RealContext = wglCreateContextAttribsARB(RealDC, 0, context_attributes);
    if(RealContext == 0)
    {
        int error = GetLastError();
        char *errormessage;
        int size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errormessage, 0,0);
        MessageBox(NULL, "Real Context Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return(1);
    }
    
    if(RealContext == 0 || wglMakeCurrent(RealDC, RealContext) == 0)
    {
        // real context failed
        int error = GetLastError();
        char *errormessage;
        int size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errormessage, 0,0);
        
        return(1);
    }
    
    if(wglSwapIntervalEXT != 0)
    {
        wglSwapIntervalEXT(1);
    }
    
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    
    //my_stbtt_initfont();
    
    strcpy(ShaderPrograms[0].vertexshader, "test.vert");
    strcpy(ShaderPrograms[0].fragmentshader, "test.frag");
    
    getfile(ShaderPrograms[0].vertexshader);
    getfile(ShaderPrograms[0].fragmentshader);
    
    LoadShader(&ShaderPrograms[0]);
    
    strcpy(ShaderPrograms[1].vertexshader, "test2.vert");
    strcpy(ShaderPrograms[1].fragmentshader, "test2.frag");
    
    getfile(ShaderPrograms[1].vertexshader);
    getfile(ShaderPrograms[1].fragmentshader);
    
    LoadShader(&ShaderPrograms[1]);
    
    strcpy(ShaderPrograms[2].vertexshader, "test3.vert");
    strcpy(ShaderPrograms[2].fragmentshader, "test3.frag");
    
    getfile(ShaderPrograms[2].vertexshader);
    getfile(ShaderPrograms[2].fragmentshader);
    
    LoadShader(&ShaderPrograms[2]);
    
    // load images here for now i guess
    image_info tempio = {16,16,1,1,0,0};
    //LoadImageOGL("C:/DEV/PROJ/gems/data/tiny_dungeon_font.png", &tempio);
    //LoadImage("C:/DEV/PROJ/zeldoII/data/guy.png",&tempio); // 0
    //LoadImage("C:/DEV/PROJ/zeldoII/data/badguy.png",&tempio); // 1
    tempio.width = 100;
    tempio.height = 50;
    //LoadImage("C:/DEV/PROJ/zeldoII/data/badplatform.png",&tempio); // 2
    
    // TODO: fallback for no extentions
    
    return(0);
}

void
DrawLine(RO_Line* line)
{
    glColor3ub(line->r, line->g, line->b);
    glBegin(GL_LINES);
    glVertex3f(line->x1, line->y1, 0.0);
    glVertex3f(line->x2, line->y2, 0.0);
    glEnd();
}

void
DrawBox(RO_Box* box)
{
    glColor3ub(box->r, box->g, box->b);
    
    glRectf(box->x, box->y, box->x + box->w, box->y + box->h);
}

void
DrawText(RO_Text* text)
{
    glColor3ub(text->r, text->g, text->b);
    // my_stbtt_print(text->x,text->y,text->text);
    // disabling this to compile,
#if 0
    uint32 textureindex = 3;
    image_info *image = &Images[textureindex];
    
    int w = image->tilewidth * text->scale;
    int h = image->tileheight * text->scale;
    int x = text->x;
    int y = text->y;
    
    float tilescalex = image->tilewidth/float(image->width);
    float tilescaley = image->tileheight/float(image->height);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0, 1.0, 1.0);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 4);
    
    glBegin(GL_TRIANGLES);
    
    // TODO: this looks unsafe as fuck
    size_t length = strlen(text->text);
    int xx = x;
    
    for(size_t ii = 0; ii < length; ++ii)
    {
        char c = text->text[ii];
        int number = -1;
#if 0
        if((c > 0x40 && c < 0x5B)
           || (c >0x60 && c < 0x7B))
        {
            number = (c & 0x1F) - 1;
        } else if(c == 0x40)
        {
            number = 37;
        } else
#endif
        if(c == 0x0A)
        {
            x = xx;
            y -= (image->tileheight) * text->scale;
        } else if(c == 0x20)
        {
            x += image->tilewidth * text->scale;
        } else if(c < 128) {
            number = asciitooryx[c];
        }
        
        if(number != -1)
        {
            float bot = ((number + image->tileswide * text->style * 2) / image->tileswide) * tilescaley;
            float top = (((number+ image->tileswide * text->style * 2) /  image->tileswide) + 1 ) * tilescaley;
            float left = (number % image->tileswide) * tilescalex;
            float right = ((number % image->tileswide) + 1) * tilescalex;
            
            x +=image->tilewidth * text->scale;
            
            // lower right
            glTexCoord2f(left, top);
            glVertex2i(x, y-h);
            glTexCoord2f(right, top);
            glVertex2i(x+w, y-h);
            glTexCoord2f(right, bot);
            glVertex2i(x+w, y);
            
            // upper left
            glTexCoord2f(left, top);
            glVertex2i(x, y-h);
            glTexCoord2f(right, bot);
            glVertex2i(x+w, y);
            glTexCoord2f(left, bot);
            glVertex2i(x, y);
        }
        
        
    }
    
    glEnd();
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
#endif
}

void
DrawTile(RO_Tile* tile)
{
    image_info *image = &Images[tile->textureindex];
    
    float w = image->tilewidth * tile->sizex;
    float h = image->tileheight * tile->sizey;
    float x = tile->x;
    float y = tile->y;
    
    float tilescalex = Images[tile->textureindex].tilewidth/float(Images[tile->textureindex].width);
    float tilescaley = Images[tile->textureindex].tileheight/float(Images[tile->textureindex].height);
    
    float bot = (tile->number / image->tileswide) * tilescaley;
    float top = ((tile->number /  image->tileswide) + 1 ) * tilescaley;
    float left = (tile->number % image->tileswide) * tilescalex;
    float right = ((tile->number % image->tileswide) + 1) * tilescalex;
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0, 1.0, 1.0);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, tile->textureindex+1);
    
    glBegin(GL_TRIANGLES);
    
    // lower right
    glTexCoord2f(left, top);
    glVertex2i(x, y-h);
    glTexCoord2f(right, top);
    glVertex2i(x+w, y-h);
    glTexCoord2f(right, bot);
    glVertex2i(x+w, y);
    
    // upper left
    glTexCoord2f(left, top);
    glVertex2i(x, y-h);
    glTexCoord2f(right, bot);
    glVertex2i(x+w, y);
    glTexCoord2f(left, bot);
    glVertex2i(x, y);
    
    glEnd();
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    
    //glBindTexture(GL_TEXTURE_2D, 0);
}

#if 0

int w;
int h;
int comp;


struct
renderdata{
    v2 av,bv,cv,dv,ev,fv;
    v2 at,bt,ct,dt,et,ft;
};

renderdata
BoxTov2(tempbox* box)
{
    renderdata result = {};
    v2 a,b,c,d;
    a.x = box->x - box->w;
    a.y = box->y + box->h;
    b.x = box->x + box->w;
    b.y = box->y + box->h;
    c.x = box->x - box->w;
    c.y = box->y - box->h;
    d.x = box->x + box->w;
    d.y = box->y - box->h;
    
    v2 u,v,w,x;
    u = {0,0};
    v = {1,0};
    w = {0,1};
    x = {1,1};
    
    result.av = a;
    result.bv = b;
    result.cv = c;
    
    result.dv = c;
    result.ev = b;
    result.fv = d;
    
    result.at = u;
    result.bt = v;
    result.ct = w;
    
    result.dt = w;
    result.et = v;
    result.ft = x;
    
    return(result);
}

void
TestOpenGL(int number, tempbox* boxes)
{
    // HORRIBLE HACKY THING! TESTING ONLY
    if(temptexture == NULL)
    {
        
        temptexture = stbi_load("../data/herotest.png", &w, &h, &comp, STBI_rgb_alpha);
        
        //LoadBMP:
        glGenTextures(1, &temptextureindex);
        glBindTexture(GL_TEXTURE_2D, temptextureindex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, temptexture);
        free(temptexture);
    }
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0, 1.0, 1.0);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, temptextureindex);
    
    
    // interesting bit
    renderdata platforms[4];
    
    GLfloat *vaoboxes = (GLfloat *)platforms;
    
    
    GLfloat *vaotextureuv = (GLfloat *)platforms;
    /*
     GLfloat vaoboxes[12][2] = {
      {500,200}, {500,300},{600,300},
      {500,200}, {600,200},{600,300},
      {-200,-200}, {-100,-200},{-100,-100},
      {-200,-200}, {-200,-100},{-100,-100},
     };
     
     GLfloat vaotextureuv[12][2] = {
      {0,0},{0,1},{1,1},
      {0,0},{1,0},{1,1},
      {1,1},{0,1},{0,0},
      {1,1},{1,0},{0,0},
     };
    */
    GLuint VAOindex;
    GLuint VBOindex[2];
    
    glGenVertexArrays(1, &VAOindex);
    glBindVertexArray(VAOindex);
    glEnableVertexAttribArray(VAOindex);
    
    glGenBuffers(2, VBOindex);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBOindex[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*2*4, platforms, GL_STATIC_DRAW);
    glEnableVertexAttribArray(ATTRIBUTE_LOCATION_POSITIONS);
    glVertexAttribPointer(ATTRIBUTE_LOCATION_POSITIONS, 2, GL_FLOAT, GL_FALSE,
                          0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    
    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
    glBindBuffer(GL_ARRAY_BUFFER, VBOindex[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*2*4, platforms, GL_STATIC_DRAW);
    glEnableVertexAttribArray(ATTRIBUTE_LOCATION_TEXTUREUV);
    glVertexAttribPointer(ATTRIBUTE_LOCATION_TEXTUREUV, 2, GL_FLOAT, GL_FALSE,
                          0, platforms+3*2*4);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    
    //glEnableVertexAttribArray( ATTRIBUTE_LOCATION_POSITIONS );
    //glVertexAttribPointer(ATTRIBUTE_LOCATION_POSITIONS, 3, GL_FLOAT, 32, 0);
    
    //glEnableVertexAttribArray( ATTRIBUTE_LOCATION_TEXTUREUV );
    //glVertexAttribPointer(ATTRIBUTE_LOCATION_TEXTUREUV, 2, GL_FLOAT, 32, 0);
    
    // fix this
    glDrawArrays(GL_TRIANGLES, 0, 12);
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    
#if 0
    
    glColor3f(1,0,0);
    GLuint triangleVBO;
    float data[] = {12.0, 61.0, 5.0,
        2.0, 15.0, -1.0,
        -10.0, 5.0, 6.0};
    glGenBuffers(1, &triangleVBO);
    //Make the new VBO active
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    //Upload vertex data to the video device
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    //Make the new VBO active. Repeat here incase changed since initialisation
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    
    //Draw Triangle from VBO - do each time window, view point or data changes
    //Establish its 3 coordinates per vertex with zero stride in this array; necessary here
    glVertexPointer(2, GL_FLOAT, 0, NULL);
    
    //Establish array contains vertices (not normals, colours, texture coords etc)
    glEnableClientState(GL_VERTEX_ARRAY);
#endif
    //Actually draw the triangle, giving the number of vertices provided
    
    
}
#endif

void
debug_justdrawtext(char* text)
{
#if 0
    RO_Text ro = {0xFFFFFFFF,16,32,2,0,text};
    
    // easyfont
    char buffer[99999]; // ~500 chars
    unsigned char color[4] = {0xFF,0xFF,0xFF,0xFF};
    //int num_quads;
    //stb_easy_font_print(x,y,color,buffer,size);
    int numquads = stb_easy_font_print(100.0,100.0,text,color,buffer,sizeof(buffer));
    
    glColor3f(0xFF,0xFF,0xFF);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, numquads*4);
    glDisableClientState(GL_VERTEX_ARRAY);
#endif
}
