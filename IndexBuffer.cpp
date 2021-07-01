#include "IndexBuffer.h"
#include "GL/glew.h"

IndexBuffer::IndexBuffer(const void* data, unsigned int count, bool dynamic) : _count(count) {
	glGenBuffers(1, &_indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);

	if (!dynamic) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}
	
}

IndexBuffer::~IndexBuffer(){
	glDeleteBuffers(1, &_indexBufferId);
}

void IndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
}

void IndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
