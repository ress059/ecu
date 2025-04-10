.. _introduction:

Introduction
###############################################

.. raw:: html

   <hr>


What Is This Project?
"""""""""""""""""""""""""""""""""""""""""""""""
:term:`ECU` (Embedded C Utilities) is a hardware-independent collection of data structures and 
algorithms targeted for embedded platforms.


Key Features
"""""""""""""""""""""""""""""""""""""""""""""""
.. _introduction_key_features:

+ Static and run-time assert framework for embedded targets.

- Linked lists.

+ Endianness compatibility macros.

- Event-driven finite state machines and hierarchical state machines.

+ Hardware-independent timers.

- Generic (n-ary) trees.

This project is targeted for embedded platforms by following these principles:

+ No dynamic memory allocation.

- No recursion.

+ Source code is in C, with support for C99 and newer.

- Build system is set up for cross-compilation. Easily import ECU into your project and build 
  it with whatever toolchain you are using.

+ No external dependencies. No reliance on stdlib system calls.

- Hardware-independent.

    - Any modules dealing with hardware (i.e. timers) do so through lightweight
      interfaces. Definitions for these interfaces are user-supplied in order to fit your hardware platform.

+ Stdlib asserts are never used. 

    + All asserts in ECU source code are made to call a user-defined callback if they fire, so 
      system response under an assert condition is fully customizable. Asserts in source code 
      can also be disabled via '-DECU_DISABLE_RUNTIME_ASSERTS' for better performance
      at the cost of reliability.


Requirements
"""""""""""""""""""""""""""""""""""""""""""""""
+ C99 or newer compiler.
    
- While not required, it is recommended to use a CMake build system. 


ECU Directory Structure
"""""""""""""""""""""""""""""""""""""""""""""""
.. _directory_structure:

.. code-block:: text

    ecu
    ├── doc/
    ├── inc/
    |   └── ecu/
    ├── src/
    ├── tests/ 
    ├── toolchains/
    ├── CMakeLists.txt 
    └── CMakePresets.json

``doc/``: Documentation for ECU's API and this site.

``inc/ecu/``: ECU's public headers for use in your application. The ecu directory is present
to give your application additional scope when using #include directives. For example:

    .. code-block:: c

        /* ecu directory adds scope to your include paths. */
        #include "ecu/dlist.h"

``src/``: ECU source code. This should be compiled with your project's toolchain. If you are 
using CMake, ECU's build system assists with this.

``tests/``: Unit tests and build tests. For internal use.

``toolchains/``: Contains `CMake toolchain files <https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html>`_ 
for ECU's internal use. This is used to test ECU against various compilers.

``CMakeLists.txt``: Top-level CMake file for ECU's internal build system.

``CMakePresets.json``: ECU's build configurations. For internal use.


Next Steps
"""""""""""""""""""""""""""""""""""""""""""""""
See :ref:`Getting Started Section <getting_started>` to learn how to import ECU into your project.

Navigate to the :ref:`Contents Sidebar <asserter_h>` (asserter.h, attributes.h, etc) to view 
in-depth documentation and APIs for each library module.
