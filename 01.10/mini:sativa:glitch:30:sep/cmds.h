// #ifndef CMDS_H
// #define CMDS_H

// #include "minishell.h"
// #include <stdlib.h>
// #include <string.h>

// // Define constants for command node types
// #define CMD_TYPE_SIMPLE 1
// #define CMD_TYPE_PIPELINE 2

// typedef struct Command 
// {
//     char **args;
//     char *input_file;
//     char *output_file;
//     int append; // For output redirection (1: append, 0: overwrite)
//     struct Command *next;
//     int heredoc;
//     char *heredoc_delim;
// } Command;

// Command *new_command();
// void add_argument(Command *cmd, char *arg);
// Command *parse_pipeline(Token **tokens, envvar **env_list);
// void free_commands(Command *commands);


// #endif
