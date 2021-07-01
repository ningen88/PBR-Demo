#pragma once

class IndexBuffer {

private:
	unsigned int _indexBufferId;
	unsigned int _count;

public:
	IndexBuffer(const void* data, unsigned int count, bool dynamic);
	~IndexBuffer();
	void bind() const;
	void unbind() const;
	inline unsigned int getCount() { return _count; }
};