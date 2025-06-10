#include "data.h"

#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

size_t to_gl(uint32_t fl_type)
{
	switch (fl_type) {
	case FL_FLOAT:
		return GL_FLOAT;
	case FL_UINT:
		return GL_UNSIGNED_INT;
	}

	return 0;
}

size_t sizeof_fl(uint32_t type)
{
	switch (type) {
	case FL_FLOAT:
		return sizeof(float);
	case FL_UINT:
		return sizeof(uint32_t);
	}

	return 0;
}

struct vao generate_vao(struct vbo *vertex_buffers, size_t vertex_buffers_size,
			struct ebo index_buffer)
{
	uint32_t vao_name;
	glCreateVertexArrays(1, &vao_name);

	glVertexArrayElementBuffer(vao_name, index_buffer.name);

	uint32_t binding_index = 0;
	uint32_t format_index = 0;

	for (size_t i = 0; i < vertex_buffers_size; i++) {
		struct vbo vertex_buffer = vertex_buffers[i];

		glVertexArrayVertexBuffer(vao_name, binding_index,
					  vertex_buffer.name, 0,
					  vertex_buffer.total_len);

		uint32_t offset = 0;
		for (size_t j = 0; j < vertex_buffer.format_len; j++) {
			uint32_t type = vertex_buffer.format[j * 2];
			uint32_t gl_type = to_gl(type);
			uint32_t size = vertex_buffer.format[j * 2 + 1];
			glEnableVertexArrayAttrib(vao_name, format_index);
			glVertexArrayAttribFormat(vao_name, format_index, size,
						  gl_type, GL_FALSE, offset);
			glVertexArrayAttribBinding(vao_name, format_index,
						   binding_index);
			offset += size * sizeof_fl(type);
			format_index++;
		}
		binding_index++;
	}

	return (struct vao){vao_name};
}

struct vbo generate_vbo(uint32_t *format, size_t format_size)
{
	uint32_t vbo_name;
	glCreateBuffers(1, &vbo_name);

	size_t total_len = 0;
	for (size_t i = 0; i < format_size; i++) {
		total_len += sizeof_fl(format[i * 2]) * format[i * 2 + 1];
	}

	return (struct vbo){vbo_name, format, format_size, total_len};
}

struct ebo generate_ebo()
{
	uint32_t ebo_name;
	glCreateBuffers(1, &ebo_name);

	return (struct ebo){ebo_name};
}

struct ssbo generate_ssbo(uint32_t binding)
{
	uint32_t ssbo_name;
	glCreateBuffers(1, &ssbo_name);

	return (struct ssbo){ssbo_name, binding};
}

void bind_vao(struct vao vertex_array)
{
	glBindVertexArray(vertex_array.name);
}

void bind_ssbo(struct ssbo shader_storage_buffer)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
			 shader_storage_buffer.binding,
			 shader_storage_buffer.name);
}

void allocate_buffer(uint32_t name, size_t size, void *data, uint32_t options)
{
	if (name == 0) {
		return;
	}

	if (options & AO_IMMUTABLE) {
		uint32_t flags = 0;
		if (options & AO_DYNAMIC) {
			flags |= GL_DYNAMIC_STORAGE_BIT;
		} else if (options & AO_READ) {
			flags |= GL_MAP_READ_BIT;
		} else if (options & AO_WRITE) {
			flags |= GL_MAP_WRITE_BIT;
		}
		glNamedBufferStorage(name, size, data, flags);
	} else {
		uint32_t usage = GL_STATIC_DRAW;
		if (options & AO_DYNAMIC) {
			usage = GL_DYNAMIC_DRAW;
		}
		glNamedBufferData(name, size, data, usage);
	}
}

void allocate_vbo(struct vbo vertex_buffer, size_t size, void *data,
		  uint32_t options)
{
	allocate_buffer(vertex_buffer.name, size, data, options);
}

void allocate_ebo(struct ebo index_buffer, size_t size, void *data,
		  uint32_t options)
{
	allocate_buffer(index_buffer.name, size, data, options);
}

void allocate_ssbo(struct ssbo shader_storage_buffer, size_t size, void *data,
		   uint32_t options)
{
	allocate_buffer(shader_storage_buffer.name, size, data, options);
}

void update_buffer(uint32_t name, size_t offset, size_t len, void *data)
{
	if (name == 0) {
		return;
	}

	glNamedBufferSubData(name, offset, len, data);
}

void update_vbo(struct vbo vertex_buffer, size_t offset, size_t size,
		void *data)
{
	update_buffer(vertex_buffer.name, offset, size, data);
}

void update_ebo(struct ebo index_buffer, size_t offset, size_t size, void *data)
{
	update_buffer(index_buffer.name, offset, size, data);
}

void update_ssbo(struct ssbo shader_storage_buffer, size_t offset, size_t size,
		 void *data)
{
	update_buffer(shader_storage_buffer.name, offset, size, data);
}
