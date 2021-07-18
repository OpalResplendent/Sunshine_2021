
#include "platform.h"

// lower left xy, w, h
// we gonna draw a lotta squares, so make this not dumb later
void DrawSquare(glm::vec4 square, glm::vec3 color, Shader* shader)
{    
    float squarepoints[24] = {
        square.x, square.y, 0.1f, color.r, color.g, color.b, // ll
        square.x + square.z, square.y, 0.1f, color.r, color.g, color.b, // lr
        square.x, square.y + square.w, 0.1f, color.r, color.g, color.b, // ul
        square.x + square.z, square.y + square.w, 0.1f, color.r, color.g, color.b // ur
    };
    unsigned int elements[6]{
        0, 1, 2,
        1, 3, 2
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squarepoints), squarepoints, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void DrawAABB(AABB box, Color color, Shader* shader)
{
    float squarepoints[24] = {
        box.position.x, box.position.y, 0.1f,color.r, color.g, color.b, // ll
        box.position.x + box.size.x, box.position.y, 0.1f, color.r, color.g, color.b, // lr
        box.position.x, box.position.y + box.size.y, 0.1f, color.r, color.g, color.b, // ul
        box.position.x + box.size.x, box.position.y + box.size.y, 0.1f, color.r, color.g, color.b // ur
    };
    unsigned int elements[6]{
        0, 1, 2,
        1, 3, 2
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squarepoints), squarepoints, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

inline void
DrawElements(float* data, int datasize, unsigned int* elements, int elementssize, Shader* shader)
{
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * datasize, data, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elementssize, elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void FancyDrawAABB(AABB box, Color color, Shader* shader)
{
    // 

    float red = 0.3;
    float blue = 0.2;
    Color colorBL = Blend(color, Color{ lerp(0, red, box.position.x / 1280), 0,
                                        lerp(0, blue, box.position.y/720)});
    Color colorBR = Blend(color, Color{ lerp(0, red, (box.position.x + box.size.x) / 1280), 0,
                                        lerp(0, blue, box.position.y / 720) });
    Color colorUL = Blend(color, Color{ lerp(0, red, box.position.x/1280), 0,
                                        lerp(0, blue, (box.position.y + box.size.y) / 720) });
    Color colorUR = Blend(color, Color{ lerp(0, red, (box.position.x + box.size.x) / 1280), 0,
                                        lerp(0, blue, (box.position.y + box.size.y) / 720) });

    unsigned int datasize = 24;
    float data[24] = {
        box.position.x, box.position.y, 0.1f, colorBL.r, colorBL.g, colorBL.b, // ll
        box.position.x + box.size.x, box.position.y, 0.1f, colorBR.r, colorBR.g, colorBR.b, // lr
        box.position.x, box.position.y + box.size.y, 0.1f, colorUL.r, colorUL.g, colorUL.b, // ul
        box.position.x + box.size.x, box.position.y + box.size.y, 0.1f, colorUR.r, colorUR.g, colorUR.b // ur
    };
    unsigned int elementssize = 6;
    unsigned int elements[6]{
        0, 1, 2,
        1, 3, 2
    };

    DrawElements(data, datasize, elements, elementssize, shader);
}

