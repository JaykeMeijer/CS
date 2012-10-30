#include "html.h"

void html_header(char *title) {
    printf("Content-type: text/html\n\n<!DOCTYPE HTML PUBLIC \"-//W3C//DTD "
           "HTML 4.01 Transitional//EN\"\"http://www.w3.org/TR/html4/loose.dtd"
           "\"><html>\n<head>\n<title>%s</title>\n<meta http-equiv="
           "\"Content-Type\" content=\"text/html;charset=utf-8\"></head>\n"
           "<body>\n", title);
}

void html_close() {
    printf("<p><a href=\"../index.html\">Return to homepage</a></p>"
        "\n</body>\n</html>\n");
}

void html_404() {
    printf("Status: 404 Not Found\n\n<html>\n<head>\n"
           "<title>404 File not found</title>\n</head>\n<body>\n<h1>File not "
           "found</h1>\n</body>\n</html>");
}
