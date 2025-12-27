typedef struct Slice {
    char *buffer; // buffer to use as a slice
    int start;    // start position
    int length;   // len of slice

    struct Slice *next; // next slice
    struct Slice *prev; // prev slice
} Slice;                // slice type

typedef struct String {
    Slice *head; // head of slices
    Slice *tail; // tail of slices
    int length;  // length of all slices
} String;        // string type

// delete string abstraction
void delete_string(String *string);

// create a slice
Slice *create_slice(const char *buffer);
// init string fields
String *string_init();
// add a slice to the end
void string_append(String *str, Slice *slice);
// insert a slice after a defined slice
void string_insert_after(String *string, Slice *after_this, Slice *slice);
// remove a slice
int string_remove(String *string, Slice *slice);

// make a slice from the string
Slice *get_slice_from_string(String *string, char start, char end);
// Edit a slice, return new slice
int edit_slice(String *string, Slice *slice, char *buffer);

// display
char *buffer_slice_to_slice(String *string, Slice *from, Slice *to);
char *slice_to_buffer(Slice *slice);
char *string_to_buffer(String *string);
