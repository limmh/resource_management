/*

The MIT License (MIT)

Copyright (c) 2017 MH Lim

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

#include <res_mgr_resource.hpp>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This program opens a list of files and displays printable characters

class close_file_s
{
public:
	int operator()(FILE *fp) {
		int rc = fclose(fp);
		// Uncomment the following line for debugging
		//printf(">> Debug message: File closed\n");
		return rc;
	}
};

class release_memory_s
{
public:
	void operator()(void *mem) {
		free(mem);
		// Uncomment the following line for debugging
		//printf(">> Debug message: Memory released\n");
	}
};

// Alternative for C++98
//typedef res_mgr::Resource<FILE*, int, 0, close_file_s> FileResource;
//typedef res_mgr::Resource<void*, int, 0, release_memory_s> Buffer;

typedef res_mgr::Resource<FILE*, std::nullptr_t, nullptr, close_file_s> FileResource;
typedef res_mgr::Resource<void*, std::nullptr_t, nullptr, release_memory_s> Buffer;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		errno = 0;
		FileResource file = fopen(argv[i], "rb");
		if (!file.is_valid()) {
			printf("%s: ", argv[i]);
			if (errno != 0)
				printf("%s\n", strerror(errno));
			else
				printf("Cannot open file\n");
			continue;
		}

		// get file size
		size_t file_size = 0;
		int c = fgetc(file.get());
		while (EOF != c) {
			file_size++;
			c = fgetc(file.get());
		}

		rewind(file.get());
		Buffer buffer = malloc(file_size * sizeof(unsigned char));
		if (!buffer.is_valid()) {
			printf("%s: Out of memory.\n", argv[i]);
			continue;
		}

		fread(buffer.get(), sizeof(unsigned char), file_size, file.get());
		unsigned char *p = reinterpret_cast<unsigned char*>(buffer.get());
		printf("%s: %zd byte%s\n", argv[i], file_size, (file_size > 1) ? "s" : "");
		for (size_t n = 0; n < file_size; n++)
			if (isprint(p[n]) || isspace(p[n]))
				fputc(p[n], stdout);
		printf("\n");
	}

	return 0;
}
