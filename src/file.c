#include "file.h"
#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _process_burst_line_s(char **, burst_data *);
static void _process_burst_line(char *, burst_line *);

burst_data *read_burstfile(char *file_path) {
    FILE *fp;
    burst_data *output;

    fp = fopen(file_path, "r");
    if (!fp) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // skip first 3 lines
    int i = 0;
    char **str = (char **)calloc(1024, sizeof(char *));
    for (i = 0; i < 1024; i++) {
        str[i] = (char *)calloc(1024, sizeof(char));
    }
    fseek(fp, 20, SEEK_SET);

    int total_process = 0;
    while (!feof(fp)) {
        fgets(str[total_process++], 1024, fp);
        // strcat(str[i], )
    }

    total_process = total_process - 2;

#ifndef NDEBUG
    for (i = 0; i < total_process; i++) {
        debug("%d: %s", i, str[i]);
    }
#endif

    output = (burst_data *)calloc(total_process, sizeof(burst_data));
    output->b_data = (burst_line **)calloc(total_process, sizeof(burst_line *));

    for (i = 0; i < total_process; i++) {
        output->b_data[i] = (burst_line *)calloc(1, sizeof(burst_line));
    }
    output->t_process = total_process;
    // exclude last line
    _process_burst_line_s(str, output);

    fclose(fp);
    return output;
}

static void _process_burst_line_s(char **line_s, burst_data *output) {
    int i = 0;
    for (i = 0; i < output->t_process; i++) {
        _process_burst_line(line_s[i], output->b_data[i]);
    }
}

// this function assumes to get correct burst data
// e.g "0 100 2 90 2 80 3 70 2 60 2 10 -1\n"
static void _process_burst_line(char *line, burst_line *data) {
    if (!line) {
        fprintf(stderr, "Line string is Null.");
        return;
    }
    if (!data) {
        fprintf(stderr, "burst_data is NUll.");
        return;
    }
    // strtok modifies the *line so it can't accept char *
    int l_line = strlen(line) + 1;
    char string[l_line];
    strcpy(string, line);

    int a_time;
    char *savetoken;
    const char *delimiter = " ";
    int *cpu_burst = (int *)calloc(1024, sizeof(int));
    int *io_burst = (int *)calloc(1024, sizeof(int));

    char *token = strtok_r(string, delimiter, &savetoken);
    if (!token) {
        // No token present
        return;
    }
    a_time = atoi(token);

    int i, int_token, cpu_burst_size, io_burst_size;
    i = cpu_burst_size = io_burst_size = 0;
    for (i = 0, token = strtok_r(NULL, delimiter, &savetoken); token != NULL;
         i++, token = strtok_r(NULL, delimiter, &savetoken)) {
        int_token = atoi(token);
        if (int_token == -1) {
            debug("END of string is reached");
            break;
        }
        // use strtol for error handlling
        // https://stackoverflow.com/questions/14176123/correct-usage-of-strtol
        if (i % 2 == 0) {
            cpu_burst[cpu_burst_size++] = int_token;
            debug("%d", cpu_burst[cpu_burst_size - 1]);
        } else {
            io_burst[io_burst_size++] = int_token;
            debug("%d", io_burst[io_burst_size - 1]);
        }
    }

    data->a_time = a_time;
    data->cpu_burst_size = cpu_burst_size;
    data->io_burst_size = io_burst_size;

    data->cpu_burst = calloc(cpu_burst_size, sizeof(int));
    data->io_burst = calloc(io_burst_size, sizeof(int));

    int j = 0;
    for (j = 0; j < cpu_burst_size; j++) {
        data->cpu_burst[j] = cpu_burst[j];
    }
    for (j = 0; j < io_burst_size; j++) {
        data->io_burst[j] = io_burst[j];
    }

    free(cpu_burst);
    free(io_burst);
}

// int main(int argc, char *argv[])
// {
// char *str[] = {"6 10 2 60 2 30 3 70 2 10 2 10 -1", "0 15 2 5 2 15 2 5 2 15 2 5 2 15 2 5 2 15 2 5
// 2 15 2 5 2 15 2 5 2 15 2 5 2 5 2 15 2 5 2 5 -1" char *s = "6 1 -1"; _process_burst_line_s(str, 3,
// data); burst_line *hello = (burst_line *)calloc(1, sizeof(burst_line)); _process_burst_line(s,
// hello); burst_data *data = read_burstfile("test_cases/process3.dat");

// #ifndef NDEBUG
//     debug("t_process: %d", data->t_process);
//     debug("");

//     int i = 0;
//     debug("cpu_burst_size[last_index]: %d", data->b_data[data->t_process - 1]->cpu_burst_size);
//     for (i = 0; i < data->b_data[data->t_process - 1]->cpu_burst_size; i++)
//     {
//         debug("%dth index: %d", i, data->b_data[data->t_process - 1]->cpu_burst[i]);
//     }
// #endif

//     return 0;
// }
