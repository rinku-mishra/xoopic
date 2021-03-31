#!/usr/bin/env python3

import re
import sys

def parse_xoopic_input(filename):
    '''
    Takes the name of a XOOPIC input file and returns a dictionary with all the
    variables in the Variable-block. NB: This is a hacky but convenient solution.
    '''

    with open(filename) as file:
        code = file.read()

    # Extract contents of Variables{...}
    code = re.search('Variables\s*?{([\s\S]*?)}', code).groups()[0]

    code = re.sub('//.*', '', code) # Remove comments
    code = re.sub('\n[ \t]*', '\n', code) # Remove leading whitespace

    # Import sqrt and other mathematical function
    code = 'from math import *\n' + code

    # Execute as Python
    vars = dict()
    exec(code, None, vars)

    return vars
