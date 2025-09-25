#pragma once
#include <stdio.h>

FILE* open_file(char* name);
void increment_line(void);
unsigned int get_line(void);
void increment_location(unsigned int);
unsigned int get_start_location(void);
unsigned int get_end_location(void);
void start_token(void);
void print_file_line(); // Only when file has been set