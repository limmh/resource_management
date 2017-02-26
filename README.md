Resource Management in C++ using RAII
=====================================

Resource management is done manually in C, but can be done in C++ by using RAII, which stands for resource acquisition is initialization.

Each user-defined type or class in C++ allows us to define the behavior of its constructor and destructor.
This feature allows us to acquire resources in the constructor and release the resources in the destructor.

The advantages of RAII are:

- Resource leaks can be prevented.
- Application code is more exception safe as local resource objects are properly cleaned up in the event of a C++ exception.
- Application code can be shorter because the code to release resources manually need not be written.
- Programmers can be more productive as they need not deal with resource management in their application code.

## Resource Management Class Template

The header file `res_mgr_resource.hpp` contains a template of resource type.
It takes 4 template parameters.
The following is its declaration in the header file.
The type definition is placed under a namespace called `res_mgr`.

    template<typename T, typename S, S invalid_value, class ReleaseFuncType> class Resource;

- The first parameter is the resource type `T`.
- The second parameter is the invalid value type `S`.
- The third parameter is a value of the type represented by the second parameter.
- The fourth parameter is a function object that is used to perform the cleanup on resource type `T`.

Many resource types can be created by using the generic template.

## Member Functions

- close(): Releases the resource held by the object.
- get(): Returns the internal resource in its raw form. It can be passed to C API, but should not be released manually.
- is_valid(): Used to check whether the object holds a valid resource.
- swap(): Used to swap the internal resources between two resource objects.

## Usage

To create a resource class for C file pointer, the user-defined type is as follows.

    typedef res_mgr::Resource<FILE*, std::nullptr_t, nullptr, close_file> FileResource;

In the above example, `close_file` is a function object type. It can be written as follows.

    class close_file {
    public:
        void operator()(FILE* fp) {
            fclose(fp);
        }
    };

After the new resource type has been created, objects can be created using it.

    FileResource file;

The above code will create a resource object that holds a file pointer.
The size of a `FileResource` object is the same as that of a pointer to `FILE`.

To initialize the file resource object to hold a valid file pointer, write the following.

    FileResource file = fopen("<file path>", "rb");

Because the `operator=` is defined for the resource type `T`, the following code is valid.

    file = fopen("<file path>", "rb");

If `file` is holding a file pointer, the file pointer that it is holding will be closed first.
When the `file` object goes out of scope, its destructor will be invoked, which release the file resources it is holding.

In order to use the resource object with a C interface, the get() method can be used to obtain the raw resource, e.g.

    fread(buffer, sizeof(unsigned char), buffer_size, file.get());

## Limitations

- Programmers need to check whether the resource object holds a valid resource before using it.
- There is no prevention for manual deallocation of resources after the resource is obtained by using get(), e.g. fclose(file.get());. DO NOT do this.

## Examples

An example is now available in the `examples` folder.
The resource class is used to manage a C file pointer and a block of dynamically allocated memory.

## Compilation

A makefile for GCC is available in the `examples` folder.

On Windows, users can use Visual Studio 2015 to compile the code.

1. Create a Visual Studio solution.
2. Include the source files (*.cpp) in the `examples` folder in separate Visual Studio projects in the same solution.
3. Specify the include path that contains `res_mgr_resource.hpp` for each project.
4. Compile the project.

