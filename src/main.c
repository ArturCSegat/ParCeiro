#include "../headers/router.h"

int main(void) {
    Router * r = new_router(); 
    listen_on(r, "3000");
}
