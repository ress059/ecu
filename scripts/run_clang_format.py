"""
clang-format does not run for all files recursively - you must pass them in one by one.
This script allows you to pass in files and directories to clang-format. 
It will run clang-format recursively on all files supplied to this script, and on all files
within the supplied directories. Current supported file extensions are .c, .h, .cpp, and .hpp
Type --help for how to call this script.
"""
import argparse
import re
import sys
import subprocess
import os

def check_clang_format_binary(args):
    """
    Checks if invoking supplied clang-format binary works.
    Return the major version of clang-format. For example if clang-format
    is v14.2.4 this function returns 14
    """
    try:
        result = subprocess.run([args.clang_format_binary, '--version'], capture_output=True, text=True)
        stdout = result.stdout.strip()
        version = re.search(r'(\d+)(\.\d+)+', stdout)
        return version.group(1) # Return the major version of clang-format.
    except OSError:
        print('Unable to run clang-format. Is supplied argument --clang-format-binary= correct?')
        raise

def parse_args():
    parser = argparse.ArgumentParser(description='Runs clang-format over all files in given directories.'
                                                 ' Requires clang-format in PATH.')
    
    parser.add_argument('--clang-format-binary=', default='clang-format', type=str, dest='clang_format_binary',
                        help='Path to clang-format binary. Defaults to \'clang-format\' if not supplied. '
                             'Example: --clang-format-binary=path/to/clang_format_binary.exe')
    
    parser.add_argument('--Werror', action='store_true', dest='werror',
                        help='See --Werror flag in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Example: --Werror')
    
    parser.add_argument('--fallback-style=', default='none', type=str, dest='fallback_style',
                        help='See --fallback-style flag in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Defaults to \'none\' if not supplied. '
                             'Example: --fallback-style=LLVM')
    
    parser.add_argument('-n', '--dry-run', action='store_true', dest='dry_run',
                        help='See --dry-run and -n flags in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Example: --dry-run')
    
    parser.add_argument('--ferror-limit=', nargs=1, dest='ferror_limit', type=int,
                        help='See --ferror-limit= flag in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Example: --ferror-limit=2')
    
    parser.add_argument('--files', nargs='+', dest='files', type=str, required=True,
                        help='List of files and directories to process. Similar to --files= clang-format command but also takes '
                             'in directories. clang-format will recursively run on all files in the supplied directories. '
                             'Supplied files and directories must be space separated without an equals sign after the --files directive. '
                             'Example: --files file.c '
                             'Example: --files file.c dir1 file3.h')
    
    parser.add_argument('-i', action='store_true', dest='i',
                        help='See -i flag in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Example: -i')
    
    parser.add_argument('--style=', type=str, nargs=1, default='file', dest='style',
                        help='See --style= flag in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Defaults to \'file\' if not supplied. '
                             'Example: --style=file')
    
    parser.add_argument('--verbose', action='store_true', dest='verbose',
                        help='See --verbose flag in clang-format docs: https://clang.llvm.org/docs/ClangFormat.html. '
                             'Example: --verbose')
    
    parser.add_argument('--clang-format-version-PRIVATE', dest='clang_format_version', type=int,
                        help=argparse.SUPPRESS)
    
    args = parser.parse_args(sys.argv[1:])
    args.clang_format_version = check_clang_format_binary(args)

    return args

def glob_files(args):
    """
    Returns list of files to run clang-format on. Takes in list of files and directories 
    supplied in command line when this script is called. I.e. Calling
    run_clang_format.py --files=file1.cpp dir2 file2.hpp dir1 should pass in
    [file1.cpp, dir2, file2.hpp, dir1]
    """
    clang_format_files = []
    supported_extensions = ['.c', '.cpp', '.h', '.hpp']

    def file_extension_supported(file):
        nonlocal supported_extensions

        if os.path.splitext(file)[1] in supported_extensions:
            return True
        else:
            return False

    for arg in args.files:
        if os.path.isdir(arg):
            for dirpath, _, files in os.walk(arg):
                for file in files:
                    if file_extension_supported(file):
                        clang_format_files.append(os.path.join(dirpath, file))
        elif os.path.isfile(arg):
            if file_extension_supported(arg):
                clang_format_files.append(arg)
            else:
                sys.exit('ERROR: Supplied file {} has unsupported extension. Currently support {}'.format(arg, supported_extensions))
        else:
            sys.exit(f'ERROR: Supplied argument was not a file or directory {arg}. Did you supply the full path to the files and directories?')

    return clang_format_files


def run_clang_format(args, files):
    """
    args =  arguments to pass when calling clang-format. This is supplied by the user
            when this script is called and uses the same flags as clang-format. 
            See https://clang.llvm.org/docs/ClangFormat.html

    files = list of files to pass into clang-format.
    """
    cmd = [args.clang_format_binary]

    if args.werror:
        cmd.append('--Werror')
    cmd.append(f'--fallback-style={args.fallback_style}') # Defaults to 'none' if not supplied.
    if args.dry_run:
        cmd.append('--dry-run')
    if args.ferror_limit:
        cmd.append(f'--ferror-limit={args.ferror_limit[0]}')
    if args.i:
        cmd.append('-i')
    cmd.append(f'--style={args.style}') # Defaults to 'file' if not supplied.
    if args.verbose:
        cmd.append('--verbose')

    # clang-format does not have a --files argument for versions less than 16....
    if int(args.clang_format_version) < 16:
        for file in files:
            print(f'Running clang-format on file {file}')
            subprocess.run(cmd + [file])
    else:
        for file in files:
            print(f'Running clang-format on file {file}')
            print(cmd + [f'--files={file}'])
    # !!!!!!!!!! TODO Stopped here. clang-format not able to handle -i command...when you delete
    # -i clang-format permanently hangs after first file processed....
            subprocess.run(cmd + [f'--files= {file}'])

def main():
    args = parse_args()
    files = glob_files(args)
    run_clang_format(args, files)

if __name__ == '__main__':
    main()