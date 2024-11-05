Resource Management in C++ using RAII
=====================================

Resource management is done manually in C, but can be done in C++ by using RAII, which stands for resource acquisition is initialization.
RAII is also known as scope bound resource management (SBRM).

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

    template<typename ResourceType, ResourceType invalid_value, class ResourceFunctor> class Resource;

- The first parameter is the resource type.
- The second parameter is the invalid value.
- The third parameter is a functor.

Many resource types can be created by using the generic template.

## Member Functions

- release(): Releases the resource held by the object.
- get(): Returns the internal resource in its raw form. It can be passed to C API, but should not be released manually.
- is_valid(): Used to check whether the object holds a valid resource.
- swap(): Used to swap the internal resources between two resource objects.

## Usage

To create a resource class for C file pointer, the user-defined type is as follows.

    typedef res_mgr::Resource<FILE*, nullptr, FileFunctor> FileResource;

In the above example, `FileFunctor` is a function object type. It should be written as follows.

    class FileFunctor {
    public:
        void operator()(FILE* fp) {
            fclose(fp);
        }

        bool operator()(FILE *fp, FILE *invalid_value) {
            return (fp != invalid_value);
        }
    };

After the new resource type has been created, objects can be created using it.

    FileResource file;

The above code will create a resource object that holds a file pointer.
The size of a `FileResource` object is the same as that of a pointer to `FILE`.

To initialize the file resource object to hold a valid file pointer, write the following.

    FileResource file = fopen("<file path>", "rb");

Since the constructor, `Resource(ResourceType)` is defined for the resource type, the code above is valid.

The copy assignment operator, `Resource& operator=(ResourceType)` is defined, so the following code is valid.

    file = fopen("<file path>", "rb");

If `file` is holding a valid file pointer, the file pointer that it is holding will be closed first.
When the `file` object goes out of scope, its destructor will be invoked, which release the file it is holding.

In order to use the resource object with a C interface, the get() method can be used to obtain the raw resource, e.g.

    fread(buffer, sizeof(unsigned char), buffer_size, file.get());

## Limitations

- Programmers should check whether the resource object holds a valid resource before using it.
- There is no prevention for manual deallocation of resources after the resource is obtained by using get(), e.g. fclose(file.get());. DO NOT do this.

## Examples

Examples are available in the `examples` folder.
The resource class is used to manage a C file pointer and a block of dynamically allocated memory.

## Compilation

A makefile for GCC and a CMakeLists.txt file are available in the `examples` folder.
