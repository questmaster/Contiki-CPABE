cmake_minimum_required(VERSION 3.16)

set(WARNING_FLAGS -Wall -Wextra -Wno-missing-braces -Werror -pedantic -Wduplicated-cond)

function(setup_module module_name)
#    target_compile_options(${module_name} PRIVATE ${WARNING_FLAGS})
endfunction()


