#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../setup.h"
#include "paperserver.h"
#include "html.h"

int parse_input(char *input, int length, char **author_name, char **paper_title,
        char **data) {
    int line_start = 0, line_end = 0, line_length, data_start, i,
    line_counter = 0, filesize;

    for(i = 0; i < length; i++) {
        if(input[i] == '\n') {
            // End of this line.
            line_end = i;

            if(line_counter == 7) {
                // The name of the author is on this line
                line_length = line_end - line_start - 1;
                *author_name = (char*) malloc(line_length);
                memcpy(*author_name, &input[line_start], line_length);
                (*author_name)[line_length] = '\0';
            } else if (line_counter == 3) {
                // The name of the paper is on this line
                line_length = line_end - line_start - 1;
                *paper_title = malloc(line_length);
                memcpy(*paper_title, &input[line_start], line_length);
                (*paper_title)[line_length] = '\0';
                data_start = 0;
            } else if (line_counter == 11) {
                // The data starts on this line
                data_start = i + 1;
            }

            line_counter++;
            if(i != length - 1)
                line_start = i + 1;
        }
    }

    // The filesize can now be determined, and copied to local memory
    filesize = line_start - data_start - 3;
    *data = malloc(filesize);
    memcpy(*data, &input[data_start], filesize);
    return filesize;
}

void handle_add(char* author_name, char* paper_name, char* data,
        int data_length, CLIENT* cl) {
    struct paper in;
    struct stat st;
    add_out *out;
    int write_bytes;

    in.author_name = author_name;
    in.paper_name = paper_name;
    in.data.data_val = malloc(data_length);
    in.data.data_len = data_length;

    memcpy(in.data.data_val, data, data_length);

    // Perform RPC
    out = add_proc_1(&in, cl);

    // Check return value
    if (out==NULL)
        printf("Error storing document\n");
    else if (*out == -1)
        printf("Error: system is full\n");
    else
        printf("Succes uploading the document, id: %i\n", *out);
}

int main(int argc, char **argv) {
    char *input, *author_name, *paper_title, *data;
    int c_length, read_n, i, filesize;
    CLIENT *cl;
    cl = clnt_create(PAPERSERVER_ADDR, PAPERSERVER_PROG, PAPERSERVER_VERS,
        "tcp");
    if (cl==NULL) {
        perror("Error creating RPC client!");
        return 1;
    }

    // Retrieve data
    c_length = atoi(getenv("CONTENT_LENGTH"));
    input = malloc(c_length + 1);
    read_n = fread(input, 1, c_length, stdin);
    input[c_length] = '\0';

    html_header("Article upload");

    // Parse the input
    filesize = parse_input(input, c_length, &author_name, &paper_title, &data);

    // Perform the request
    handle_add(author_name, paper_title, data, filesize, cl);

    html_close();

    free(input);
    free(author_name);
    free(paper_title);
    return 0;
}
