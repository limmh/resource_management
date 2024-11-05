/*

The MIT License (MIT)

Copyright (c) 2017 - 2024 MH Lim

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "res_mgr_resource.hpp"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This program opens a list of files and print their contents as binary data and ASCII characters

#if __cplusplus < 201103L
#define nullptr NULL
#endif

struct FileFunctor
{
	static FILE* open_file_in_binary_read_mode(const char* path) {
		assert(path != nullptr);
		return fopen(path, "rb");
	}

	void operator()(FILE *fp) {
		assert(fp != nullptr);
		const int status = fclose(fp);
		// Comment the following line so that a debug message will not be printed
		printf(">> Debug message: File at %p closed (status code: %d)\n", fp, status);
	}

	bool operator()(FILE* fp, FILE* invalid_file) { return (fp != invalid_file); }
};

struct DynamicMemoryFunctor
{
	static void* allocate(size_t number_of_bytes) {
		return calloc(number_of_bytes, sizeof(unsigned char));
	}

	void operator()(void *memory) {
		free(memory);
		// Comment the following line so that a debug message will not be printed
		printf(">> Debug message: Memory at %p released\n", memory);
	}

	bool operator()(void* memory_address, void* invalid_address) { return (memory_address != invalid_address); }
};

typedef res_mgr::Resource<FILE*, nullptr, FileFunctor> File;
typedef res_mgr::Resource<void*, nullptr, DynamicMemoryFunctor> DynamicMemory;

size_t get_file_size(FILE* file)
{
	assert(file != nullptr);
	size_t file_size = 0U;
	rewind(file);
	int c = fgetc(file);
	while (c != EOF) {
		++file_size;
		c = fgetc(file);
	}
	rewind(file);
	return file_size;
}

void print_binary_data(FILE *file, const unsigned char* data, size_t number_of_bytes)
{
	const size_t bytes_per_line = 32U;
	const size_t number_of_lines = number_of_bytes / bytes_per_line;
	const size_t remaining_bytes = number_of_bytes % bytes_per_line;

	for (size_t i = 0U; i < number_of_lines; ++i) {
		const size_t offset = i * bytes_per_line;
		for (size_t j = 0U; j < bytes_per_line; ++j) {
			const size_t index = offset + j;
			fprintf(file, "%02X ", data[index]);
		}
		fprintf(file, "   ");
		for (size_t j = 0U; j < bytes_per_line; ++j) {
			const size_t index = offset + j;
			unsigned char c = data[index];
			if (isspace(c)) {
				c = ' ';
			} else if (!isprint(c)) {
				c = '?';
			}
			fputc(c, file);
		}
		if ((i + 1) < number_of_lines || remaining_bytes > 0U)
			fputc('\n', file);
	}

	if (remaining_bytes > 0U) {
		const size_t offset = bytes_per_line * number_of_lines;
		for (size_t i = 0U; i < remaining_bytes; ++i) {
			const size_t index = offset + i;
			fprintf(file, "%02X ", data[index]);
		}
		for (size_t i = remaining_bytes; i < bytes_per_line; ++i) {
			fprintf(file, "   ");
		}
		fprintf(file, "   ");
		for (size_t i = 0U; i < remaining_bytes; ++i) {
			const size_t index = offset + i;
			unsigned char c = data[index];
			if (isspace(c)) {
				c = ' ';
			}
			else if (!isprint(c)) {
				c = '?';
			}
			fputc(c, file);
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 0;
	}

	for (int i = 1; i < argc; ++i) {
		errno = 0;
		size_t file_size = 0U;
		DynamicMemory dyn_mem;
		{
			const File file = FileFunctor::open_file_in_binary_read_mode(argv[i]);
			if (!file.is_valid()) {
				const int error_code = errno;
				printf("%s: %s\n", argv[i], ((error_code != 0) ? strerror(error_code) : "Cannot open file."));
				continue;
			}

			file_size = get_file_size(file.get());
			dyn_mem = DynamicMemoryFunctor::allocate(file_size);
			if (!dyn_mem.is_valid()) {
				const int error_code = errno;
				printf("%s: %s\n", argv[i], ((error_code != 0) ? strerror(error_code): "Out of memory."));
				continue;
			}

			fread(dyn_mem.get(), sizeof(unsigned char), file_size, file.get());
		}
		printf("%s: %lu byte%s\n", argv[i], static_cast<unsigned long>(file_size), ((file_size > 1U) ? "s" : ""));
		print_binary_data(stdout, static_cast<const unsigned char*>(dyn_mem.get()), file_size);
		printf("\n");
		DynamicMemory dyn_mem2;
		dyn_mem2.swap(dyn_mem);
	}

	return 0;
}
