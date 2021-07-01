#pragma once

class Buffer {

private:
	unsigned int _bufferId;


public:
	Buffer(const void* data, unsigned int size, bool dynamic);
	~Buffer();
	void bind() const;
	void unbind() const;
	void setLayout(unsigned int index, int size, unsigned int stride, const void* pointer);
};
