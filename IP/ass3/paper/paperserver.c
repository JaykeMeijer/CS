#include "paperserver.h"

struct paper_container {
    int id;
    char* author_name;
    char* paper_name;
	u_int data_len;
	char *data_val;
    struct paper_container *next;
};

struct paper_container *paper_list = NULL;

/* Locate the required paper container. */
struct paper_container *find(int id) {
    struct paper_container *curr = paper_list;
    while(curr != NULL) {
        if(curr->id == id) {
            return curr;
        } else {
            curr = curr->next;
        }
    }
    return NULL;
}

add_out *add_proc_1_svc(paper *in, struct svc_req *rqstp) {
    static add_out out;
    struct paper_container *curr = paper_list;
    struct paper_container *new = malloc(sizeof(struct paper_container));
    int i;

    // Store metadata
    new->author_name = strdup(in->author_name);
    new->paper_name = strdup(in->paper_name);
    new->next = NULL;

    // Store actual paper
    new->data_val = malloc(in->data.data_len * sizeof(char));
    memcpy(new->data_val, in->data.data_val, in->data.data_len * sizeof(char));
    new->data_len = in->data.data_len;

    // If no paper yet or id 0 is free, become new head
    if(curr == NULL || curr->id != 0) {
        if(curr != NULL)
            new->next = curr;
        new->id = 0;
        paper_list = new;
        out = 0;
        return &out;
    }

    // Find my slot in the list
    while(curr->next != NULL) {
        if(curr->next->id > (curr->id+1))
            break;
        else
            curr = curr->next;
    }

    // Store paper container at located point in the list.
    new->id = curr->id + 1;
    new->next = curr->next;
    curr->next = new;
    out = new->id;

    return (&out);
}

int *remove_proc_1_svc(remove_in *in, struct svc_req *rqstp) {
    struct paper_container *curr = paper_list, *tmp;
    static int out = 0;

    // No paper in the system
    if(curr == NULL)
        return &out;

    // Paper is the head of the list.
    if(curr->id == (int)*in) {
        paper_list = curr->next;
        free(curr);
        out = 1;
        return &out;
    }

    // Locate the required paper in the list
    while(curr->next != NULL) {
        if(curr->next->id == (int)*in) {
            // Paper found, remove it by redirecting the next pointer
            tmp = curr->next;
            curr->next = curr->next->next;
            out = 1;
            free(tmp->data_val);
            free(tmp);
            return (&out);
        } else {
            // Paper not found, continue traversal
            curr = curr->next;
        }
    }

    // Paper not found
    return (&out);
}

list_out *list_proc_1_svc(void *in, struct svc_req *rqstp) {
    static struct list_out head;
    struct list_out *tmp;
    struct paper_container *curr = paper_list;
    tmp = &head;

    // List is empty, return id -1.
    if(curr == NULL) {
        head.id = -1;
        head.author_name = "";
        head.paper_title = "";
        head.next = NULL;
        return &head;
    }

    while(1) {
        // Create a new node as next and set the data for the current node.
        tmp->id = curr->id;
        tmp->author_name = malloc(sizeof(char) * 64);
        tmp->paper_title = malloc(sizeof(char) * 64);
        tmp->author_name = strdup(curr->author_name);
        tmp->paper_title = strdup(curr->paper_name);

        if(curr->next != NULL) {
            // Continue to the next paper.
            tmp->next = (struct list_out*)malloc(sizeof(struct list_out));
            tmp = tmp->next;
            curr = curr->next;
        } else {
            break;
        }
    }
    tmp->next = NULL;

    tmp = &head;

    return &head;
}

info_out *info_proc_1_svc(info_in *in, struct svc_req *rqstp) {
    static struct info_out out;
    struct paper_container *curr = find((int)*in);
    if(curr != NULL) {
        out.author_name = curr->author_name;
        out.paper_name = curr->paper_name;
        return (&out);
    }
    return NULL;
}

paper_out *fetch_proc_1_svc(fetch_in *in, struct svc_req *rqstp) {
    static struct paper_out out;
    struct paper_container *curr;
    int datasize;

    curr = find((int)*in);

    if(curr != NULL) {
        out.succes = 1;

        datasize = sizeof(char) * curr->data_len;
        out.data.data_val = malloc(datasize);
        memcpy(out.data.data_val, curr->data_val, datasize);
        out.data.data_len = curr->data_len;

        return (&out);
    } else {
        out.succes = 0;
        out.data.data_len = 0;
        return &out;
    }
}
