#include <stdio.h>
#include "csv_formatter.h"

#define CSV_FILE_NAME "rules.csv"
#define CSV_HEADER "Start_state,End_state,Message id,Function code,Has parameters,Offset,Length,Value"
#define CSV_LINE_FORMAT "%d,%d,%d,%d,%d,%d,%d,%s"

FILE *csv_file = NULL;

int create_csv(void)
{
    // If the file exists, do nothing
    if (csv_file == NULL)
    {
        csv_file = fopen(CSV_FILE_NAME, "w");
        if (csv_file == NULL)
        {
            return 1;
        }
    }
    
    return 0;
}

void write_rule(rule_t rule)
{

}

void close_csv(void)
{
    if (csv_file != NULL)
    {
        fclose(csv_file);
    }
}