#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../setup.h"
#include "paperserver.h"
#include "html.h"

void handle_list(CLIENT* cl) {
    list_out *reply;

    reply = list_proc_1(NULL, cl);
    if(reply == NULL) {
        clnt_perror(cl, "Failed to retrieve list");
        exit(1);
    }

    // Print header of list
    printf("<h1>Papers in the system</h1>\n<p>This are all the papers in the "
        "system:</p>\n<p>\n");

    if(reply->id == -1) {
        printf("No papers found in the system\n</p>\n");
        return;
    }

    printf("<ul>\n");
    while(1) {
        printf("<li>%s by %s. <a href=\"paperview.cgi?%i\">"
            "Download</a></li>\n", reply->paper_title, reply->author_name,
            reply->id);

        if(reply->next != NULL)
            reply = reply->next;
        else
            break;
    }

    // Close list
    printf("</ul></p>\n");
    return;
}

int main(int argc, char **argv) {
    CLIENT *cl;
    cl = clnt_create(PAPERSERVER_ADDR, PAPERSERVER_PROG, PAPERSERVER_VERS, \
        "tcp");
    if (cl==NULL) {
        perror("Error creating RPC client!");
        return 1;
    }

    html_header("Papers in system");

    // Print list of papers
    handle_list(cl);

    // Print end of page
    html_close();

    return 0;
}
