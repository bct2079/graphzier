#include <stdbool.h>
#include <stdlib.h>

typedef struct
{
    int index;
    char *type;
    bool isError;
} Error;

Error *init_error()
{
    Error *error = calloc(1, sizeof(Error));
    error->index = -1;
    error->type = NULL;
    error->isError = false;
    return error;
}

void define_error(Error *error, int index, char *type)
{
    error->index = index;
    error->type = type;
    error->isError = true;
}
void clear_error(Error *error)
{
    error->index = -1;
    error->type = NULL;
    error->isError = false;
}
