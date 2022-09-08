#!/usr/bin/env python3

from distutils.core import setup, Extension
from pathlib import Path
import sysconfig
#import pybind11

inc_path_pybind11 = str(Path.joinpath(Path.cwd(), "../pybind11/include"));
#inc_path_quantlib = str(Path.joinpath(Path.cwd(), "../aadcQuantLib-v1.28"));
inc_path_quantlib = str(Path.joinpath(Path.cwd(), "../quantlib"));

lib_path = str(Path.joinpath(Path.cwd(), "../quantlib/build/ql/"));

#pybind11_inc = pybind11.get_include()

extra_compile_args = ["-std=gnu++17", "-mavx", "-march=native" ] # , "-Wall", "-Wextra"
extra_compile_args += ["-g3", "-O0", "-DDEBUG=1", "-UNDEBUG" ]
extra_compile_args += ["-DQL_USE_STD_SHARED_PTR" ]

## disable chatty compiler warnings that are enabled by default somewhere else:
extra_compile_args += ["-Wno-ignored-attributes","-Wno-unused-variable","-Wno-sign-compare","-Wno-reorder"]

quantlib = Extension(
    "quantlib",
    sources = [ "src/quantlib.cpp" ],
    include_dirs = [ inc_path_pybind11, inc_path_quantlib ],
    library_dirs = [ lib_path ],
    libraries = [ 'QuantLib' ],
    extra_compile_args = extra_compile_args
)

setup(	
	name = "quantlib-pybind", 
	version = "1.0", 
	ext_modules = [quantlib],
    data_files=[(
        'lib', ["../quantlib/build/ql/libQuantLib.so"]
    )]
	 )
