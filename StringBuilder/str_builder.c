#include <stdio.h>  // fprintf
#include <stdlib.h> // malloc/free
#include <string.h> // strcmp

#include "str_builder.h"

/* typedef struct Slice { */
/*     char *buffer; // buffer to use as a slice */
/*     int start;          // start position */
/*     int length;         // len of slice */

/*     struct Slice *next; // next slice */
/*     struct Slice *prev; // prev slice */
/* } Slice;                // slice type */

/* typedef struct String { */
/*     Slice *head; // head of slices */
/*     Slice *tail; // tail of slices */
/*     int length;  // length of all slices */
/* } String;        // string type */

// helpers
// free a slice from memory

static int _free_slice(Slice *slice) {

    if (!slice)
        return 0; // failed

    if (slice->prev)
        slice->prev->next = slice->next;

    if (slice->next)
        slice->next->prev = slice->prev;

    free((void *)slice->buffer);
    slice->buffer = NULL;
    free(slice);

    return 1; // success
}

// free the entire string from memory
static int _free_all(String *string) {
    Slice *tmp = string->head;

    while (tmp) {
        Slice *next_slice = tmp->next;
        _free_slice(tmp);
        tmp = next_slice;
    }

    string->head = NULL;
    string->tail = NULL;

    return 1;
}

void delete_string(String *string) {
    if (string) {
        _free_all(string);
        free(string);
    }
    string = NULL;
}

// create a slice
Slice *create_slice(const char *buffer_to_use) {
    int length = strlen(buffer_to_use);
    // length without null terminated char '\0'

    // malloc buffer
    char *buffer = (char *)malloc(length * sizeof(char));
    if (!buffer) {
        fprintf(stderr, "Failed to init buffer for slice.\n");
        return NULL;
    }

    // init slice (check if it made mem)
    Slice *slice = (Slice *)malloc(sizeof(Slice));
    if (!slice) {
        fprintf(stderr, "Failed to init slice.\n");
        free(buffer);
        return NULL;
    }

    // non null-terminated
    slice->buffer = buffer;
    memcpy(slice->buffer, buffer_to_use, length);

    slice->start = (length - length) + 1;
    slice->length = length;
    slice->next = NULL;
    slice->prev = NULL;

    return slice;
}

// init string fields
String *string_init() {
    // init string (check if it made mem)
    String *string = (String *)malloc(sizeof(String));

    if (!string) {
        fprintf(stderr, "Failed to init string.\n");
        return NULL;
    }
    string->head = NULL;
    string->tail = NULL;
    string->length = 0;

    return string;
}

// add a slice to the end
void string_append(String *str, Slice *slice) {
    // check both exist
    if (!str || !slice) {
        fprintf(stderr, "no string or slice initialised.\n");
        return; // failed
    }

    // append to head if head is null
    if (!str->head) {
        str->head = slice;
        str->tail = slice;
    } else {
        // otherwise add to end
        str->tail->next = slice;
        slice->prev = str->tail;
        str->tail = slice;
    }
    // all slices have an extra space for the null terminator
    str->length += slice->length;
}

// insert a slice after a defined slice
void string_insert_after(String *string, Slice *after_this, Slice *slice) {

    // deal with obj attributes
    string->length += slice->length;

    // insert after first slice and ensure the next is the new slice with new
    // slice-> the previous slice
    Slice *tmp = after_this->next;
    after_this->next = slice;
    slice->prev = after_this;
    slice->next = tmp;
    tmp->prev = slice;
}

// remove a slice
int string_remove(String *string, Slice *slice) {
    if (!string || !slice) {
        fprintf(stdout, "No string or slice to remove.\n");
        return 0;
    }
    if (!slice->prev)
        string->head = slice->next;
    if (!slice->next)
        string->tail = slice->prev;

    string->length -= slice->length;

    _free_slice(slice);

    return 1; // success
}

Slice *get_slice_from_string(String *string, char start, char end) {
    if (!string) {
        fprintf(stderr, "No string.\n");
        return NULL;
    }

    int start_pos = -1;
    int end_pos = -1;

    Slice *tmp = string->head;
    int current_pos = 0;

    while (tmp) {
        for (int i = 0; i < tmp->length; i++) {
            if (tmp->buffer[i] == start && start_pos == -1) {
                start_pos = current_pos + i;
            }
            if (tmp->buffer[i] == end && start_pos != -1) {
                end_pos = current_pos + i;
                break;
            }
        }

        if (end_pos != -1) {
            break;
        }

        current_pos += tmp->length;
        tmp = tmp->next;
    }

    if (start_pos != -1 && end_pos != -1 && end_pos >= start_pos) {
        // Total length of the slice
        int slice_length = end_pos - start_pos + 1;

        char *new_buffer = (char *)malloc(slice_length + 1);
        if (!new_buffer) {
            fprintf(stderr, "Failed to allocate memory for new slice.\n");
            return NULL;
        }

        tmp = string->head;
        current_pos = 0;
        int copied = 0;

        while (tmp && copied < slice_length) {
            for (int i = 0; i < tmp->length; i++) {
                int absolute_pos = current_pos + i;
                if (absolute_pos >= start_pos && absolute_pos <= end_pos) {
                    new_buffer[copied++] = tmp->buffer[i];
                }
                if (copied >= slice_length) {
                    break;
                }
            }
            current_pos += tmp->length;
            tmp = tmp->next;
        }

        new_buffer[slice_length] = '\0';

        Slice *new_slice = create_slice(new_buffer);
        free(new_buffer);
        return new_slice;
    }

    fprintf(stderr, "Could not find start or end characters in string.\n");
    return NULL;
}

// Edit a slice, return new slice
int edit_slice(String *string, Slice *slice, char *buffer) {
    if (!string || !slice || !buffer) {
        fprintf(stderr,
                "Failed to edit slice, no string or slice or buffer.\n");
        return 1; // failed
    }
    string->length -= slice->length; // take away the old length
    slice->length = strlen(buffer);
    string->length += slice->length; // add the new length
    slice->start = (slice->length - slice->length) + 1;
    slice->buffer = realloc(slice->buffer, slice->length);
    memcpy(slice->buffer, buffer, slice->length);

    return 1; // success
}
// get all chars from slice 1 to slice 2
char *buffer_slice_to_slice(String *string, Slice *from, Slice *to) {
    // check params
    if (!string || !from || !to) {
        fprintf(stderr, "No string or slice (from) or slice (to).\n");
        return NULL;
    }

    // get the length for the buffer
    Slice *tmp = from;

    int total_length = 0;

    while (tmp) {
        total_length += tmp->length;
        if (tmp == to) {
            break;
        }
        tmp = tmp->next;
    }

    // + 1 for null terminator
    char *return_buffer = (char *)malloc(total_length + 1);

    if (!return_buffer) {
        fprintf(stderr, "Failed to init return buffer.\n");
        return NULL;
    }

    char *current_pos_in_buffer = return_buffer;

    tmp = from;
    while (tmp) {
        memcpy(current_pos_in_buffer, tmp->buffer, tmp->length);
        current_pos_in_buffer += tmp->length;
        if (tmp == to) {
            break;
        }
        tmp = tmp->next;
    }

    return_buffer[total_length] = '\0';

    return return_buffer;
}

// certain slice to buffer
char *slice_to_buffer(Slice *slice) {
    // + 1 for null-terminator
    char *return_buffer = (char *)malloc(slice->length + 1);
    if (!return_buffer) {
        fprintf(stderr, "Failed to init return buffer.\n");
        return NULL;
    }

    memcpy(return_buffer, slice->buffer, slice->length);
    return_buffer[slice->length] = '\0';

    return return_buffer;
}

// the whole string
char *string_to_buffer(String *string) {
    // + 1 for null-terminator
    char *return_buffer = (char *)malloc(string->length + 1);
    if (!return_buffer) {
        fprintf(stderr, "Failed to init return buffer.\n");
        return NULL;
    }
    char *current_pos_in_buffer = return_buffer;

    Slice *tmp = string->head;

    while (tmp) {
        memcpy(current_pos_in_buffer, tmp->buffer, tmp->length);
        current_pos_in_buffer += tmp->length;
        tmp = tmp->next;
    }
    // insert null terminator;
    return_buffer[string->length] = '\0';
    return return_buffer;
}
