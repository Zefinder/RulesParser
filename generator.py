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
DEFAULT_RULE_FILE: str = './parsed_rules.h'

NO_VALUE: str = 'NULL'
CSV_FILE: str = './rules.csv'
SUBRULE_RULE_FORMAT: str = '{prefix:s}_rule{rule_id:d}_subrule{subrule_id:d}'
SUBRULE_STRUCTURE_FORMAT: str = 'subrule_t {name:s} = {{ .raw_body = {{ \
0x{action:0>2X}{start:0>2X}{rule_id:0>4X}, \
0x{function_code:0>4X}{message_id:0>4X}, \
0x{length:0>4X}{offset:0>4X} }}, .value={value:s} }};'
RULE_STRUCTURE_FORMAT: str = 'rule_t {prefix:s}_rule{rule_id:d} = {{ .holder=0, .state_subrules={state_subrules:s}, .subrule_number={subrule_number:s} }};'
FILE_STRUCTURE_FORMAT: str = '''#pragma once

#include <stdlib.h>
#include "rules.h"

{subrule_definitions:s}

{rule_definitions:s}
'''

# Variables
rules: dict[int, list[Subrule]] = {}
subrules_str: list[str] = []
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
    

def main(rules_prefix: str, output_file: str) -> None:
    with open(CSV_FILE, 'r') as file:
        # First line is a header
        file.readline()
        
        line: str = file.readline()
        while line: 
            add_subrule(line=line)            
            line = file.readline()
    
    for rule_id in rules:
        # Sort subrules in each rules
        subrules: list[Subrule] = rules[rule_id]
        subrules.sort(key=lambda subrule: subrule.start_state, reverse=False)
        
        subrules_per_start_state: dict[int, list[str]] = {}
        subrules_number_per_start_state: dict[int, int] = {}
        subrule_id: int = 0
        for subrule in subrules:
            subrule_name = SUBRULE_RULE_FORMAT.format(prefix=rules_prefix, rule_id=rule_id, subrule_id=subrule_id)
            
            # Create structure string
            subrules_str.append(SUBRULE_STRUCTURE_FORMAT.format(name=subrule_name,
                                                                action=subrule.end_state,
                                                                start=subrule.start_state,
                                                                rule_id=rule_id,
                                                                function_code=subrule.function_code,
                                                                message_id=subrule.message_id,
                                                                length=subrule.length,
                                                                offset=subrule.offset,
                                                                value=f'"{subrule.value:s}"' if subrule.value != '' else NO_VALUE))
            
            # Put in start state dict
            if subrule.start_state not in subrules_per_start_state:
                subrules_per_start_state[subrule.start_state] = []
                subrules_number_per_start_state[subrule.start_state] = 0
                
            subrules_per_start_state[subrule.start_state].append(subrule_name)
            subrules_number_per_start_state[subrule.start_state] += 1        
            subrule_id += 1
        
        # Create rule
        state_subrules_tmp: list[str] = []
        for values in list(subrules_per_start_state.values()):
            state_subrules_tmp.append('{ &' + ', &'.join(values) + ' }')
            
        state_subrules: str = '{ ' + ', '.join(state_subrules_tmp) + ' }'
        subrule_number: str = '{ ' + ', '.join(str(i) for i in list(subrules_number_per_start_state.values())) + ' }'
        
        
        rules_str.append(RULE_STRUCTURE_FORMAT.format(prefix=rules_prefix,
                                                      rule_id=rule_id,
                                                      state_subrules=state_subrules,
                                                      subrule_number=subrule_number))
    
    # Print result in file
    with open(output_file, 'w') as file:
        file.write(FILE_STRUCTURE_FORMAT.format(subrule_definitions='\n'.join(subrules_str), rule_definitions='\n'.join(rules_str)))
    

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-p', '--prefix')
    parser.add_option('-o', '--output')
    
    # Parse and ensure values
    opts, args = parser.parse_args(sys.argv[1:])
    opts.ensure_value('prefix', DEFAULT_PREFIX)
    opts.ensure_value('output', DEFAULT_RULE_FILE)
        
    main(opts.prefix, opts.output)