#include "Buffer.h"
#include "GL/glew.h"

Buffer::Buffer(const void* data, unsigned int size, bool dynamic){
    glGenBuffers(1, &_bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, _bufferId);

    if (!dynamic) {
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
    
}

Buffer::~Buffer(){
    glDeleteBuffers(1, &_bufferId);
}

void Buffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
}

void Buffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::setLayout(unsigned int index, int size, unsigned int stride, const void* pointer) {
    glEnableVertexArrayAttrib(_bufferId, index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}