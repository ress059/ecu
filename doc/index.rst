.. _home:

ECU
#################################################
Embedded C Utilities (ECU) is a hardware-independent collection of data structures and frameworks targeted for embedded platforms. This is a personal project primarily for personal use, but is documented here for those that may find is useful. Some features include:

- Assertion framework.

- Endianness framework.

- Linked lists.

- N-ary trees.

- State machine framework.

- Timers.

- And more.

The following principles are followed to make ECU compatible with embedded platforms:

- No dynamic memory allocation.

- No recursion.

- Portable. ISO C99-compliant with no system calls.

Directory Structure
=================================================
| ecu/
| ├── .devcontainer/
| ├── .github/
| ├── .vscode/
| ├── doc/
| ├── inc/
| ├── src/
| ├── tests
| └── toolchains/

- ``.devcontainer/``: Settings for this repository's `VSCode devcontainer <https://code.visualstudio.com/docs/devcontainers/containers>`__.

- ``.github/``: GitHub actions settings for CI pipeline.

- ``.vscode``: VSCode settings for debugging.
  
- ``doc/``: Documentation.
  
- ``inc/``: Public API used by application.
  
- ``src/``: Source code cross-compiled by application.
  
- ``tests/``: Unit and integration tests.

- ``toolchains/``: CMake toolchain files.

All files are internal to ECU besides ``inc/`` and ``src/`` which are consumed by the end user.

Porting
=================================================
ECU must be cross-compiled with the user's toolchain and requires at least an ISO C99 compliant compiler. The following steps explain how to add ECU to applications that use various build systems:

.. tabs::
    .. tab:: CMake Build System (recommended)
        #. Import ECU using `CMake FetchContent <https://cmake.org/cmake/help/latest/module/FetchContent.html>`_:
           
            .. code-block:: cmake

                # ------------------------ Application CMakeLists.txt ------------------------
                FetchContent_Declare(
                    ecu
                    GIT_REPOSITORY https://github.com/ress059/ecu.git
                    GIT_TAG        v0.1.0 # Replace with specific version desired.
                )
                FetchContent_MakeAvailable(ecu)

        #. Link ECU against the application with `target_link_libraries() <https://cmake.org/cmake/help/latest/command/target_link_libraries.html>`_. CMake will automatically handle ECU's include paths, compiler options, and other settings since FetchContent was used. 
           
            .. code-block:: cmake

                # ------------------------ Application CMakeLists.txt ------------------------
                target_link_libraries(application # Name of application target.
                    PRIVATE 
                        ecu
                )

        #. If assertions are enabled, define ``ecu_assert_handler()``. This function executes when a run-time assertion fires in ECU. System response to this condition must be defined by the application. **Leaving this undefined with assertions enabled results in a linker error.**

            .. code-block:: c

                /* ------------------------------- application.c ----------------------- */
                #include "ecu/asserter.h"

                void ecu_assert_handler(const char *file, int line)
                {
                    record(file, line);
                    reset_cpu();
                }

    .. tab:: Other Build Systems
        #. Import ECU as a git submodule.

            .. code-block:: bash 

                git submodule add https://github.com/ress059/ecu.git

        #. Checkout ECU submodule to the specific version desired.

            .. code-block:: bash

                cd ./ecu
                git checkout v0.1.0 # Replace with specific version desired. 

        #. Add the ``inc/`` folder of ECU to the application's include paths.

        #. If assertions are enabled, define ``ecu_assert_handler()``. This function executes when a run-time assertion fires in ECU. System response to this condition must be defined by the application. **Leaving this undefined with assertions enabled results in a linker error.**

            .. code-block:: c

                /* ------------------------------- application.c ----------------------- */
                #include "ecu/asserter.h"

                void ecu_assert_handler(const char *file, int line)
                {
                    record(file, line);
                    reset_cpu();
                }

        #. Compile all files within the ``src/`` folder of ECU using the application's toolchain.

            .. note::

                If using an eclipse-based IDE, all directories in ECU besides ``inc/`` and ``src/`` may have to be excluded from the build.

        #. Link the generated ECU object files against the application.

Once imported, ECU can be used throughout the application. To provide additional scope, all public symbols are prefixed with "ecu" and library headers are placed in ``inc/ecu/``:

    .. code-block:: c 

        /* #include "ecu/dlist.h" instead of #include "dlist.h" adds scope to avoid potential file name clashes. */
        #include "ecu/dlist.h" 

        /* All public symbols are prefixed with "ecu_" to add scope and prevent potential symbol name clashes. */
        static struct ecu_dnode my_node; 
        ecu_dnode_ctor(&my_node, .....);

Further Reading
=================================================
Navigate through the sidebar or click any of the icons below to view more extensive documentation:

.. raw:: html

    <div class="homepage-icons">
        <a href="doxygen/html/files.html"><img class="icon" src="_static/images/api_logo.svg" alt="Doxygen Documentation."></a>
        <a href="asserter_h/index.html"><img class="icon" src="_static/images/modules_logo.svg" alt="Explanation of ECU Modules."></a>
        <a href="https://github.com/ress059/ecu"><img class="icon" src="_static/images/github_logo.svg" alt="View ECU on GitHub."></a>
    </div>

.. toctree:: 
    :maxdepth: 3
    :caption: Modules
    :hidden:

    asserter.h <asserter_h/index>
    attributes.h <attributes_h/index>
    dlist.h <dlist_h/index>
    endian.h <endian_h/index>
    event.h <event_h/index>
    fsm.h <fsm_h/index>
    hsm.h <hsm_h/index>
    ntnode.h <ntnode_h/index>
    object_id.h <object_id_h/index>
    timer.h <timer_h/index>
    utils.h <utils_h/index>

.. toctree::
    :maxdepth: 1
    :caption: Reference
    :hidden:

    API <doxygen/html/files>
    GitHub <https://github.com/ress059/ecu>
