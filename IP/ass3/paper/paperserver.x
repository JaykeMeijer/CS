struct paper {
    string author_name<64>;
    string paper_name<64>;
    opaque data<>;
};

struct info_out {
    string author_name<64>;
    string paper_name<64>;
};

struct paper_out {
    int succes;
    opaque data<>;
};

struct list_out {
    int id;
    string author_name<64>;
    string paper_title<64>;
    struct list_out *next;
};

typedef int add_out;
typedef int remove_in;
typedef int info_in;
typedef int fetch_in;

program PAPERSERVER_PROG {
    version PAPERSERVER_VERS {
        add_out ADD_PROC(paper) = 1;
        bool REMOVE_PROC(remove_in) = 2;
        list_out LIST_PROC() = 3;
        info_out INFO_PROC(info_in) = 4;
        paper_out FETCH_PROC(fetch_in) = 5;
    } = 1;
} = 0x20001234;
