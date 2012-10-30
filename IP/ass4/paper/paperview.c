#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../setup.h"
#include "paperserver.h"
#include "html.h"

void handle_fetch(int paperid, CLIENT* cl) {
    struct paper_out *out;
    int i;

    // Perform RPC
    out = fetch_proc_1(&paperid, cl);
    if(!out->succes) {
        html_404();
        return;
    }

    // Print end of header
    printf("Content-Type: application/pdf\n\n");

    // Print the data character for character. Printing a string is not possible
    // since \0 does not indicate EOF in pdf, and can therefore be part of the
    // pdf.
    for(i = 0; i < out->data.data_len; i++)
        printf("%c", out->data.data_val[i]);
}

int main(int argc, char **argv) {
    char* input = NULL;
    int paper, i = 0;
    CLIENT *cl;
    cl = clnt_create(PAPERSERVER_ADDR, PAPERSERVER_PROG, PAPERSERVER_VERS, \
        "tcp");
    if (cl==NULL) {
        perror("Error creating RPC client!");
        return 1;
    }

    // Retrieve paper id
    input = getenv("QUERY_STRING");
    if(input == NULL) {
        html_404();
        return 0;
    }

    // Get ID
    paper = atoi(input);

    handle_fetch(paper, cl);

    return 0;
}
