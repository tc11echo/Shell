#include "const.h"

void process_cmd(char *cmdline)
{
    //printf("The input cmdline is: %s\n", cmdline);

    char *file_name_in, *file_name_out, *cmd;
    int fd_in, fd_out;
    int flag_in=0, flag_out=0;

    //redirection
    if(strchr(cmdline, '<')||strchr(cmdline, '>')){
        char *segments[3];
        char *file_name[2];
        int temp;

        if(strchr(cmdline, '>')==NULL && strchr(cmdline, '<')){ // <
            flag_in=1;

            read_tokens(segments, cmdline, &temp, "<");
            read_tokens(file_name, segments[1], &temp, SPACE_CHARS); //trim
            file_name_in=file_name[0];

            cmd=segments[0];
            
        }else if(strchr(cmdline, '>') && strchr(cmdline, '<')==NULL){ // >
            flag_out=1;

            read_tokens(segments, cmdline, &temp, ">");
            read_tokens(file_name, segments[1], &temp, SPACE_CHARS); //trim
            file_name_out=file_name[0];

            cmd=segments[0];
            
        }else if(strchr(cmdline, '>') > strchr(cmdline, '<')){ // < >
            flag_in=1; flag_out=1;

            read_tokens(segments, cmdline, &temp, ">");
            read_tokens(file_name, segments[1], &temp, SPACE_CHARS); //trim
            file_name_out=file_name[0];

            read_tokens(segments, segments[0], &temp, "<");
            read_tokens(file_name, segments[1], &temp, SPACE_CHARS); //trim
            file_name_in=file_name[0];

            cmd=segments[0];
            
        }else if(strchr(cmdline, '>') < strchr(cmdline, '<')){ // > <
            flag_in=1; flag_out=1;

            read_tokens(segments, cmdline, &temp, "<");
            read_tokens(file_name, segments[1], &temp, SPACE_CHARS); //trim
            file_name_in=file_name[0];

            read_tokens(segments, segments[0], &temp, ">");
            read_tokens(file_name, segments[1], &temp, SPACE_CHARS); //trim
            file_name_out=file_name[0];

            cmd=segments[0];
            
        }

        if(flag_in==1){ //read file
            fd_in=open(file_name_in, O_RDONLY, S_IRUSR | S_IWUSR); //permission: 600
            close(STDIN_FILENO);
            dup2(fd_in, STDIN_FILENO);
        }

        if(flag_out==1){ //write file
            fd_out=open(file_name_out, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR); //permission: 600
            close(STDOUT_FILENO);
            dup2(fd_out, STDOUT_FILENO);
        }

        cmdline=cmd;
        
    }
    //pipe
    char *pipe_segments[MAX_PIPE_SEGMENTS];
    int num_pipe_segments, i;
    read_tokens(pipe_segments, cmdline, &num_pipe_segments, PIPE_CHAR);

    for(i=0; i<num_pipe_segments-1; i++){
        int pfds[2]; //pfds[0] read end, pfds[1] write end
        pipe(pfds);

        pid_t pid = fork();
        if(pid == 0){ //child
            close(STDOUT_FILENO);
            dup2(pfds[1], STDOUT_FILENO); //make stdout as pipe input
            close(pfds[0]); //close read end

            char *arguments[MAX_ARGUMENTS_PER_SEGMENT];
            int num_arguments;
            read_tokens(arguments, pipe_segments[i], &num_arguments, SPACE_CHARS);
            arguments[num_arguments]=NULL;
            execvp(arguments[0], arguments);
            //exit(1);
        }

        //parent
        close(STDIN_FILENO);
        dup2(pfds[0], STDIN_FILENO); //make stdin as pipe output
        close(pfds[1]); //close write end
        wait(NULL);
    }

    char *arguments[MAX_ARGUMENTS_PER_SEGMENT];
    int num_arguments;
    read_tokens(arguments, pipe_segments[i], &num_arguments, SPACE_CHARS);
    arguments[num_arguments]=NULL;
    execvp(arguments[0], arguments);
    //exit(1);
}


void read_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}

void show_prompt()
{
    printf("myshell> ");
}

int get_cmd_line(char *cmdline)
{
    int i;
    int n;
    if (!fgets(cmdline, MAX_CMDLINE_LEN, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(cmdline);
    cmdline[--n] = '\0';
    i = 0;
    while (i < n && cmdline[i] == ' ') {
        ++i;
    }
    if (i == n) {
        // Empty command
        return -1;
    }
    return 0;
}
