# Imports
import optparse
import sys

# Classes
class Subrule(object):
    start_state: int
    end_state: int
    message_id: int
    function_code: int
    offset: int
    length: int
    value: str
    
    
    def __init__(self,
                 start_state: int,
                 end_state: int,
                 message_id: int,
                 function_code: int,
                 offset: int,
                 length: int,
                 value: str) -> None:
        self.start_state = start_state
        self.end_state = end_state
        self.message_id = message_id
        self.function_code = function_code
        self.offset = offset
        self.length = length
        self.value = value
        
    
# Constants
DEFAULT_PREFIX: str = ''
DEFAULT_RULE_FILE_NAME: str = 'parsed_rules'

NO_VALUE: str = 'NULL'
CSV_FILE: str = './rules.csv'
SUBRULE_RULE_FORMAT: str = '{prefix:s}_rule{rule_id:d}_subrule{subrule_id:d}'
SUBRULE_STRUCTURE_FORMAT: str = '{prefix:s}_subrules[{subrule_id:d}] = (subrule_t){{ .raw_body = {{ \
0x{action:0>2X}{start:0>2X}{rule_id:0>4X}, \
0x{function_code:0>4X}{message_id:0>4X}, \
0x{length:0>4X}{offset:0>4X} }}, .value={value:s} }};'
SUBRULE_NUMBER_ARRAY_NAME_FORMAT: str = '{prefix:s}_subrule_array{rule_id:d}'
SUBRULE_NUMBER_ARRAY_FORMAT: str = 'int {name:s}[{transition_number:d}] = {subrule_number:s};'
RULE_STRUCTURE_FORMAT: str = '{prefix:s}_rules[{rule_id:d}] = (rule_t){{ .holder=0, .state_subrules=&{prefix:s}_subrules[{subrule_start:d}], .subrule_number={subrule_number_name:s} }};'
INCLUDE_FILE_STRUCTURE_FORMAT: str = '''#pragma once

#include <stdlib.h>
#include "rules.h"

#define {cap_prefix:s}_RULE_NUMBER {rule_number:d}

rule_t {prefix:s}_rules[{rule_number:d}];
subrule_t {prefix:s}_subrules[{subrule_number:d}];

void {prefix:s}_init(void);
'''
DECL_FILE_STRUCTURE_FORMAT: str = '''#include <stdlib.h>
#include "{include_file_name:s}"
#include "rules.h"

void {prefix:s}_init(void)
{{
    {subrule_definitions:s}
    
    {subrule_number_definitions:s}

    {rule_definitions:s}
}}
'''

# Variables
rules: dict[int, list[Subrule]] = {}
subrules_str: list[str] = []
subrule_number_str: list[str] = []
rules_str: list[str] = []

# Functions
def add_subrule(line: str) -> None:
    parameters: list[str] = line.split(",")
    rule_id: int = int(parameters[0])
    start_state: int = int(parameters[1])
    end_state: int = int(parameters[2])
    message_id: int = int(parameters[3])
    function_code: int = int(parameters[4])
    offset: int = int(parameters[5])
    length: int = int(parameters[6])
    value: str = parameters[7].strip()

    subrule: Subrule = Subrule(start_state=start_state,
                               end_state=end_state,
                               message_id=message_id,
                               function_code=function_code,
                               offset=offset,
                               length=length,
                               value=value)
    
    if rule_id not in rules:
        rules[rule_id] = []
        
    rules[rule_id].append(subrule)
    

def main(rules_prefix: str, include_file: str, decl_file: str) -> None:
    with open(CSV_FILE, 'r') as file:
        # First line is a header
        file.readline()
        
        line: str = file.readline()
        while line: 
            add_subrule(line=line)            
            line = file.readline()
    
    subrule_count: int = 0
    for rule_id in rules:
        # Sort subrules in each rules
        subrules: list[Subrule] = rules[rule_id]
        subrules.sort(key=lambda subrule: subrule.start_state, reverse=False)
        
        subrules_per_start_state: dict[int, list[str]] = {}
        subrules_number_per_start_state: dict[int, int] = {}
        
        subrule_start = subrule_count
        subrule_id: int = 0
        for subrule in subrules:
            subrule_name = SUBRULE_RULE_FORMAT.format(prefix=rules_prefix, rule_id=rule_id, subrule_id=subrule_id)
            
            # Create structure string
            subrules_str.append(SUBRULE_STRUCTURE_FORMAT.format(prefix=rules_prefix, 
                                                                subrule_id=subrule_count,
                                                                action=subrule.end_state,
                                                                start=subrule.start_state,
                                                                rule_id=rule_id,
                                                                function_code=subrule.function_code,
                                                                message_id=subrule.message_id,
                                                                length=subrule.length,
                                                                offset=subrule.offset,
                                                                value=f'"{subrule.value:s}"' if subrule.value != '' else NO_VALUE))
            subrule_count += 1
            
            # Put in start state dict
            if subrule.start_state not in subrules_per_start_state:
                subrules_per_start_state[subrule.start_state] = []
                subrules_number_per_start_state[subrule.start_state] = 0
                
            subrules_per_start_state[subrule.start_state].append(subrule_name)
            subrules_number_per_start_state[subrule.start_state] += 1        
            subrule_id += 1
        
        state_subrules_tmp: list[str] = []
        for values in list(subrules_per_start_state.values()):
            state_subrules_tmp.append('{ &' + ', &'.join(values) + ' }')
            
        state_subrules: str = '{ ' + ', '.join(state_subrules_tmp) + ' }'
        subrule_number: str = '{ ' + ', '.join(str(i) for i in list(subrules_number_per_start_state.values())) + ' }'
        
        # Create subrule number array
        subrule_number_name = SUBRULE_NUMBER_ARRAY_NAME_FORMAT.format(prefix=rules_prefix,rule_id=rule_id)
        subrule_number_str.append(SUBRULE_NUMBER_ARRAY_FORMAT.format(name=subrule_number_name,
                                                                     transition_number=len(subrules_number_per_start_state),
                                                                     subrule_number=subrule_number))
        
        # Create rule
        rules_str.append(RULE_STRUCTURE_FORMAT.format(prefix=rules_prefix,
                                                      rule_id=rule_id,
                                                      state_subrules=state_subrules,
                                                      subrule_start=subrule_start,
                                                      subrule_number_name=subrule_number_name))
    
    # Print result in file
    with open(include_file, 'w') as file:
        # file.write(INCLUDE_FILE_STRUCTURE_FORMAT.format(subrule_definitions='\n'.join(subrules_str), rule_definitions='\n'.join(rules_str)))
        file.write(INCLUDE_FILE_STRUCTURE_FORMAT.format(cap_prefix=rules_prefix.upper(), prefix=rules_prefix, rule_number=len(rules_str), subrule_number=len(subrules_str)))
        
    with open(decl_file, 'w') as file:
        file.write(DECL_FILE_STRUCTURE_FORMAT.format(prefix=rules_prefix,
                                                     include_file_name=include_file.split('/')[-1], 
                                                     subrule_definitions='\n\t'.join(subrules_str), 
                                                     subrule_number_definitions='\n\t'.join(subrule_number_str),
                                                     rule_definitions='\n\t'.join(rules_str)))
    

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-p', '--prefix')
    parser.add_option('-o', '--output')
    
    # Parse and ensure values
    opts, args = parser.parse_args(sys.argv[1:])
    opts.ensure_value('prefix', DEFAULT_PREFIX)
    opts.ensure_value('output', DEFAULT_RULE_FILE_NAME)
    
    main(opts.prefix, f'./{opts.output}.h', f'./{opts.output}.c')