.. _getting_started:


Getting Started
###############################################
Explains how to import :term:`ECU` into your project and start using the library.

.. raw:: html

   <hr>


Importing ECU Into Your Project
==============================================
ECU is targeted for embedded platforms so it is not distributed as a static library. It is 
meant to be cross-compiled with your project's toolchain.

Importing steps depend on your build system, which are described in the variants below. 

.. note:: 
    ECU uses CMake, so that is the recommended approach. CMake will automatically add ECU's 
    headers to your include paths, build ECU's source with the proper compiler flags, and 
    link it against your application.

    These steps have to be done manually if you are not using CMake.

.. tabs::

    .. tab:: CMake (recommended)

        Add the following snippets into your CMakeLists.txt files if your project uses a CMake 
        build system.

        #. Use `CMake FetchContent <https://cmake.org/cmake/help/latest/module/FetchContent.html>`_ to obtain 
           ECU's source code:
           
            .. code-block:: cmake

                # Fetch ECU from GitHub
                FetchContent_Declare(
                    ecu
                    GIT_REPOSITORY https://github.com/ress059/ecu.git
                    GIT_TAG        f331593a09a0004aa1dea6eb1f009bf035ac1035 # Replace with specific release version you want.
                )
                FetchContent_MakeAvailable(ecu) # ECU will be available after this line executes.

            .. todo:: 

                Update Git link on release.

        #. Use `target_compile_options() <https://cmake.org/cmake/help/latest/command/target_compile_options.html>`_ 
           to set the optimization level of ECU depending on your project's requirements. **This step is optional.**
           
            .. code-block:: cmake 

                # This example is for a project using GCC. It compiles ECU with -Os optimization. 
                # Adjust depending on your project's requirements and toolchain.
                
                target_compile_options(ecu
                    PRIVATE 
                        # Set optimization level of ECU.
                        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Os>

                        # Specify any additional compiler flags you want ECU to use.
                )

           If unspecified, this will default to -O0. You can also specify any additional compiler flags ECU 
           should use (**see note below**).

            .. note::

                ECU is compiled with all warning flags enabled. -ffunction-sections and -fdata-sections
                are also used for linker garbage collection. Therefore you should not specify these 
                flags again in this step.
                
                These flags are private in order to keep ECU's compilation settings completely separate 
                from your project's settings. In other words, your application's code will never inherit 
                ECU's internal compiler flags.

        #. Use `target_link_libraries() <https://cmake.org/cmake/help/latest/command/target_link_libraries.html>`_ 
           to link ECU against your application. 
           
            .. code-block:: cmake

                target_link_libraries(your_application # Replace with the name of your application target.
                    PRIVATE 
                        ecu
                )

           Include paths, preprocessor defines, etc are all handled automatically by CMake. Also note that ECU 
           will use the C standard specified by your project's CMake build. **This must be at least C99 or newer.**
           If this was never specified by your project, ECU will use C99.

           You can also explicitly specify ECU's C standard by adding a `target_compile_features() 
           <https://cmake.org/cmake/help/latest/command/target_compile_features.html>`_ command to the previous
           step, however this should not be needed in most cases.

            .. code-block:: cmake
                
                target_compile_features(ecu
                    PRIVATE 
                        c_std_11 # Change to whatever C standard you want ECU to use.
                )

    .. tab:: Other

        Follow these steps if you are not using CMake. In this case, the recommended route is 
        to use git submodules. 
        
        You will have to manually add ECU's headers to your include paths. 
        You will also have to manually compile and link ECU's source code against your application.

        #. Import ECU into your project as a git submodule.

            .. code-block:: bash 

                git submodule add https://github.com/ress059/ecu.git

           Note that if others clone your repository they must run the following commands to 
           initialize all submodules after cloning:

            .. code-block:: bash 

                git submodule init 
                git submodule update

        #. Add the "inc" folder of ECU to your include paths. See :ref:`ECU's directory structure <directory_structure>`
           for further reference.

        #. Compile all files within the "src" folder of ECU with your project's toolchain. See :ref:`ECU's 
           directory structure <directory_structure>` for further reference. 
           
           Note that you will have to manually specify all compiler flags for ECU to use. **At a minimum,
           it is required for ECU to be compiled in C99 or newer**.

        #. Link the generated object files against the rest of your application.


Using ECU In Your Project
==============================================
After importing, you can use ECU's API throughout your project.

To prevent polluting the global namespace, ECU gives scope to your application by appending 
all public types and functions with "ecu\_". Library headers are also placed in a directory 
named "ecu" to give scope to your #include directives.

    .. code-block:: c 

        /* ecu directory adds scope to avoid potential file name clashes. */
        #include "ecu/circular_dll.h" 

        /* All public types are appended with 'ecu_' to avoid polluting the global namespace. */
        static struct ecu_circular_dll_node my_node; 

        /* All public functions are appended with 'ecu_' to avoid polluting the global namespace. */
        ecu_circular_dll_node_ctor(&my_node, .....);


Next Steps
==============================================
See :ref:`Features Section <features>` for an in-depth explanation of ECU's features and API.
