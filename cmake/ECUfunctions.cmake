function(ecu_print)
    foreach(arg ${ARGN})
        message("${arg} = ${${arg}}")
    endforeach()
endfunction()