#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "paperserver.h"

void print_usage() {
    printf("The following commands are available:\n" \
           "\tpaperclient <address> add <author name> <paper" \
           "title> <paper location>\n" \
           "\tpaperclient <address> remove <paper number>\n" \
           "\tpaperclient <address> list\n" \
           "\tpaperclient <address> info <paper number>\n" \
           "\tpaperclient <address> fetch <paper number>");
    exit(1);
}

void handle_add(int argc, char **argv, CLIENT* cl) {
    struct paper in;
    struct stat st;
    add_out *out;
    FILE *file;
    int write_bytes;

    if(argc != 6)
        print_usage();

    // Create the request
    file = fopen(argv[5], "rb");
    if(file == NULL) {
        perror("Can not open file");
        exit(1);
    }
    stat(argv[5], &st);

    in.author_name = argv[3];
    in.paper_name = argv[4];
    in.data.data_val = malloc(sizeof(char) * st.st_size);
    in.data.data_len = fread(in.data.data_val, 1, st.st_size, file);

    if(in.data.data_len == 0) {
        perror("Failed to read");
        exit(1);
    }

    if(in.data.data_len != st.st_size)
        printf("huh?\n%i, %i\n", in.data.data_len, (int)st.st_size);
    fclose(file);

    // Perform RPC
    out = add_proc_1(&in, cl);

    // Check return value
    if (out==NULL) {
        printf("Error: %s\n",clnt_sperror(cl,argv[1]));
        exit(1);
    } else {
        printf("%d\n", *out);
    }
}

void handle_remove(int argc, char **argv, CLIENT* cl) {
    remove_in in;

    if(argc != 4)
        print_usage();

    in = atoi(argv[3]);

    // Perform RPC
    if(!remove_proc_1(&in, cl))
        printf("Failed to remove\n");
}

void handle_list(CLIENT* cl) {
    list_out *reply;

    reply = list_proc_1(NULL, cl);
    if(reply == NULL) {
        clnt_perror(cl, "Failed to retrieve list");
        return;
    }

    if(reply->id == -1)
        return;

    while(1) {
        printf("%i\t", reply->id);
        printf("%s\t", reply->author_name);
        printf("%s", reply->paper_title);
        //printf("%i\t%s\t%s", reply->id, reply->author_name, reply->paper_title);
        if(reply->next != NULL) {
            reply = reply->next;
            printf("\t");
        } else {
            break;
        }
    }

    return;
}

void handle_info(int argc, char **argv, CLIENT* cl) {
    info_in in;
    struct info_out *reply;

    if(argc != 4)
        print_usage();

    in = atoi(argv[3]);

    // Perform RPC
    reply = info_proc_1(&in, cl);
    if(reply != NULL)
        printf("%s\t%s", reply->author_name, reply->paper_name);
    else
        printf("No such element in the system\n");
}

void handle_fetch(int argc, char **argv, CLIENT* cl) {
    int in;
    struct paper_out *out;
    int i;

    if(argc != 4)
        print_usage();

    in = atoi(argv[3]);

    // Perform RPC
    out = fetch_proc_1(&in, cl);
    if(!out->succes) {
        printf("Failed\n");
        return;
    }

    // Print the data character for character. Printing a string is not possible
    // since \0 does not indicate EOF in pdf, and can therefore be part of the
    // pdf.
    for(i = 0; i < out->data.data_len; i++)
        printf("%c", out->data.data_val[i]);
}

int main(int argc, char **argv) {
    CLIENT *cl;

    if (argc < 3)
        print_usage();
    cl = clnt_create(argv[1], PAPERSERVER_PROG, \
                     PAPERSERVER_VERS, "tcp");
    if (cl==NULL) {
        perror("Error creating RPC client!");
        return 1;
    }

    // Handle requested operation
    if(!strcmp(argv[2], "add\0"))
        handle_add(argc, argv, cl);
    else if(!strcmp(argv[2], "remove\0"))
        handle_remove(argc, argv, cl);
    else if(!strcmp(argv[2], "list\0"))
        handle_list(cl);
    else if(!strcmp(argv[2], "info\0"))
        handle_info(argc, argv, cl);
    else if(!strcmp(argv[2], "fetch\0"))
        handle_fetch(argc, argv, cl);
    else
        print_usage();

    return 0;
}
