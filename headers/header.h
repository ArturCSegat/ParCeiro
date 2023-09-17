#ifndef HEADER_H
#define HEADER_H

struct header{
    const char * name;
    const char * value;
};

typedef struct{
    struct header * arr;
    int cap;
    int len;
}HeaderList ;

#endif

