#ifndef BIRPLOT_DATA_H
#define BIRPLOT_DATA_H

#include <stddef.h>
#include <stdint.h>

// used in vbo formatting
#define FL_FLOAT 0x1
#define FL_UINT 0x2

// used for allocation options
#define AO_IMMUTABLE 1
#define AO_DYNAMIC 2
#define AO_READ 4
#define AO_WRITE 8

struct vao {
	uint32_t name;
};

struct vbo {
	uint32_t name;
	uint32_t *format;
	size_t format_len;
	size_t total_len;
};

struct ebo {
	uint32_t name;
};

struct ssbo {
	uint32_t name;
	uint32_t binding;
};

struct vao generate_vao(struct vbo *vertex_buffers, size_t vertex_buffers_size,
			struct ebo index_buffer);
struct vbo generate_vbo(uint32_t *format, size_t format_size);
struct ebo generate_ebo();
struct ssbo generate_ssbo(uint32_t binding);

void bind_vao(struct vao vertex_array);
void bind_ssbo(struct ssbo shader_storage_buffer);

void allocate_vbo(struct vbo vertex_buffer, size_t size, void *data,
		  uint32_t options);
void allocate_ebo(struct ebo index_buffer, size_t size, void *data,
		  uint32_t options);
void allocate_ssbo(struct ssbo shader_storage_buffer, size_t size, void *data,
		   uint32_t options);

void update_vbo(struct vbo vertex_buffer, size_t offset, size_t size,
		void *data);
void update_ebo(struct ebo index_buffer, size_t offset, size_t size,
		void *data);
void update_ssbo(struct ssbo shader_storage_buffer, size_t offset, size_t size,
		 void *data);

#endif
