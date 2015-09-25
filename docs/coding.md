_Note: Code samples on this page do not represent any valid code present in the project's sources, it is all made up._

General Naming Conventions
==========================

* Prefer camelCase wherever possible: `helloThisIsACamel`
* When incorporating an abbreviation in a name, force it to be camelCase: `JsonLoader` instead of `JSONLoader`
* Prefix names with the global namespace prefix if necessary (e.g. preprocessor #defines => `KR_MY_MACRO`)

Platforms
=========

Currently the project is 64 bit only. Support for 32 bit Architectures is not planned.

| OS                                    | Support         |
| ------------------------------------- | --------------- |
| :checkered_flag: Windows 10           | *Main Platform* |
| :checkered_flag: Windows 8.1          | Planned         |
| :checkered_flag: Windows 8            | Planned         |
| :checkered_flag: Windows 7            | Planned         |
| :checkered_flag: Windows Vista        | Planned         |
| :apple: OSX                           | Planned         |
| :penguin: Several Linux Distributions | Planned         |
| Android                               | Not Planned     |
| iOS                                   | Not Planned     |

At first, we're going to target Windows 10 only. Windows Vista and later will probably just work.  Support for several Linux distributions as well as OSX is planned.

Project Structure
===================

~~~
+- code                // Contains all source code.
  +- app               // Some project.
    +- PCH.h           // Precompiled header file.
    +- something.h     // Header file.
    +- implementation  // Contains implementation details.
      +- something.cpp // The main translation unit including something.h.
  +- engine            // Another project.
    +- ...             // More files...
+- docs                // Contains misc documents, documentation, ideas, etc.
  +- readme.md         // Simple read-me.
+- tests               // Code directory for all tests. Don't put tests in ":/code"!
  +- engineTests       //
    +- CMakeLists.txt  // Build system descriptors.
    +- main.cpp        // Source files...
    +- test_somethingImportant.cpp

~~~

---

C++ Source Code
===============

All source code must reside in a subdirectory below `code/`. Any kind of source file located directly inside the `code/` folder is forbidden.

Include Directives
------------------

They're used to include other files, typically header files (.h or .hpp).

Whenever you `#include` a file that is not in the same directory as the current file, use angle brackets `<>` instead of quotes `""` for the file path. See the rules for how the two different forms of `#include` behave for different compilers:

* msvc: See remarks in https://msdn.microsoft.com/en-us/library/36k2cdd4.aspx
* gcc: See 2.1 in https://gcc.gnu.org/onlinedocs/gcc-3.0.2/cpp_2.html
* clang: Info here is a bit hidden, but it's still there in http://clang.llvm.org/docs/UsersManual.html#id23

**TL;DR**: The quoted form searches locally first, then in all directories specified by some `/I` or `-I` option to the compiler. Since we basically never refer to our included files in the local form, we might as well just use the angle-bracket form.

**I.e. this is the recommended way:**
~~~.cpp
#include <krEngine/renderin/window.h> // Use this form for all regular headers.
#include "localFile.inl"              // Use this form for special files only.
~~~

### Include Guards

Include guards guard against multiple inclusions of the same file for each compilation unit (.cpp).

All our target compilers support `pragma once` so we'll use that. There is no need for the old `#ifdef ABC #define ABC #endif` convention used in ancient C and C++. All header files should usually begin with `#pragma once`.

~~~.cpp
#pragma once

namespace kr
{
  struct Something {};
}
~~~

Precompiled Headers
-------------------

We use a manually maintained precompiled header located at `code/krEngine/pch.h`.

~~~.cpp
// Include precompiled header
#include <krEngine/stdafx.h>

// Include other headers.
#include <krEngine/rendering/window.h>
// ... more includes ...

kr::Window::Window()
{
}
// ... more code ...
~~~

Bracing & Indentation
---------------------

We are using a variant of the [Allman Style](http://en.wikipedia.org/wiki/Indent_style#Allman_style) for indentation and bracing. In this style, every opening brace stands on a new line for itself, aligining properly with its closing couterpart.
Indentation is done with 2 whitespaces, not tabs. `if`, `while` and `for` statements are allways followed by braces, even if only one statement is executed.
Always prefer clearity and security of code over length of code.

~~~{.cpp}

static char * concat (char *s1, char *s2)
{
  while (x == y)
  {
    something();
    somethingelse();
  }

  if (theThing())
  {
    finalthing();
  }
}
~~~

Namespaces
----------

The Krepel Namespace is called `kr`. This is the main, global namespace and all code should reside in there.
~~~{.cpp}
int main(int argc, const char *argv[])
{
    kr::String message("Hello world.");
    KR_assert(message.GetData() != nullptr, "...");
    kr::Log("Message: %s, %d", message.GetData(), KR_COMPILETIME_MIN(1, 2));
    return 0;
}
~~~
Other namespaces may only be defined within the `kr` namespace:
~~~{.cpp}
// Good:
namespace kr {
  namespace internal {
    struct ScopeExit { /* ... */ }; // some code that is not supposed to be used directly by the user.
  }
}
// Bad:
namespace kr {}
namespace internal {} // Should be inside `kr`
~~~

Variables
---------

Even though the ezEngine uses a light-weight version of the _Hungarian Notation_, we do not use it. Variable names are always in lower-camelCase.

~~~{.cpp}
void doSomething(const ezVec3& startPosition) // start is lower-case
{
  ezVec3 endPosition(0, 1, 0);
  auto rayDirection = endPosition- startPosition;
}
~~~

Use descriptive names for your variables, such as `direction` instead of `d`.

Documentation
-------------

In order to be able to generate a source code documentation later on, we are documenting stuff in header files using doxygen commands, which `///` for each line of documentation.

~~~{.cpp}
/// \brief Calculates the factorial of \a n
/// \example factorial(5); // yields 720
int factorial(int n);
~~~

---

C/C++ Preprocessor
==================

**ALL** preprocessor #defines, _with no exceptions_, are given the prefix `KR_` and are continued with UPPER_CASE (not camelCase or PascalCase).

~~~{.cpp}
#define KR_EMPTY_PUBLIC_CONSTRUCTOR(TheTypeName) public: TheTypeName() {}
#define KR_VERSION_MAJOR 0
#define KR_VERSION_MINOR 1
~~~

Indentation
-----------

Preprocessor statements are formatted like source code, thus nested statements are indented with two spaces:

~~~{.cpp}
#ifdef WIN32
  #ifdef KR_EXPORT
    #define KR_API __declspec(dllexport)
  #else
    #define KR_API __declspec(dllimport)
  #endif
#else
  #define KR_EXPORT
#endif
~~~

Multi-Line Statements
---------------------

Multi-line statements are formatted like normal source code but whith the trailing backslashes aligned on the same column:
~~~{.cpp}
// 2: align backslashes in one column
#define KR_UNIT_TEST(TheGroup, TheName)    \
class TheGroup##TheName : public IUnitTest \
{                                          \
public:                                    \
  void run() override final;               \
};
~~~

CMake
=====

* [CMake homepage](http://www.cmake.org/)
* [CMake documentation](http://cmake.org/cmake/help/latest/)

New Projects
------------

*TODO*
