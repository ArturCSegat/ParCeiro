#include <stdio.h>
#include <string.h>
#include "../headers/router.h"

void greet_user_with_name(Request * r) {
    printf("some one said their name is %s\n", r->content_string);
}

int main(void) {
    Router * r = new_router("3000"); 
    handle_uri(r->endpoint_map, "/my-name-is", &greet_user_with_name, strlen("/my-name-is"));
    listen_on(r);
}
