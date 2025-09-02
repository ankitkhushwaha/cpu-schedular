#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "dbg.h"

extern burst_line *read_burstfile(char *file_path)
{
    FILE *fp;
    burst_line *output;
    fp = fopen(file_path, "r");
    if (!fp)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    // skip first 3 lines
    int i = 0;
    char *str = (char *)calloc(1024, sizeof(char));
    fseek(fp, 20, SEEK_SET);

    fgets(str, 1024, fp);
}

void _process_burst_line_s(char *line_s, burst_line *output){

}

// this function assumes to get correct burst data
// e.g "0 100 2 90 2 80 3 70 2 60 2 10 -1\n"
void _process_burst_line(char *line, burst_line *data)
{
    check(line, "string is Null");
    check(data, "burst_data is NUll");

    int a_time;
    char *savetoken;
    const char *delimiter = " ";
    int *cpu_burst = (int *)calloc(1024, sizeof(int));
    int *io_burst = (int *)calloc(1024, sizeof(int));

    char *token = strtok_r(line, delimiter, &savetoken);
    if (!token)
    {
        // No token present
        goto error;
    }
    data->a_time = atoi(token);

    int i, cpu_burst_size, io_burst_size;
    i = cpu_burst_size = io_burst_size = 0;
    for (i = 0, token = strtok_r(NULL, delimiter, &savetoken); token != NULL;
         i++, token = strtok_r(NULL, delimiter, &savetoken))
    {
        if (strcmp(token, "-1") == 0)
            break;
        // use strtol for error handlling https://stackoverflow.com/questions/14176123/correct-usage-of-strtol
        if (i % 2 == 0)
        {
            cpu_burst[cpu_burst_size++] = atoi(token);
            debug("%d", cpu_burst[cpu_burst_size - 1]);
        }
        else
        {
            io_burst[io_burst_size++] = atoi(token);
            debug("%d", io_burst[io_burst_size - 1]);
        }
    }
    data->cpu_burst = calloc(cpu_burst_size, sizeof(int));
    data->io_burst = calloc(io_burst_size, sizeof(int));

    int j = 0;
    for (j = 0; j < cpu_burst_size; j++)
    {
        data->cpu_burst[j] = cpu_burst[i];
    }
    for (j = 0; j < io_burst_size; j++)
    {
        data->io_burst[j] = io_burst[i];
    }
    free(cpu_burst);
    free(io_burst);

error:
}

int main(int argc, char *argv[])
{
    burst_line *data = (burst_line *)calloc(1, sizeof(burst_line));

    char str[] = "6 10 2 60 2 30 3 70 2 10 2 10 -1";
    _process_burst_line(str, data);

    return 0;
}
