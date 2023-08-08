#ifndef DA
#define DA

// Initial capacity
#define DA_INIT_CAPACITY 32

// Error prefix
#define DA_ERROR_PREFIX "DA_ERROR:"

// Error messages
#define DA_ERROR_EMPTY "dynamic array is empty."
#define DA_ERROR_REALLOC "couldn't reallocate dynamic array."

// Returns the item at an index
#define DA_INDEX(arr, index) (arr).items[index]

// Constructs a structure for the dynamic array
#define DA_STRUCT(name, type) typedef struct name##Array { \
    type* items; \
    size_t count; \
    size_t capacity; \
} name##Array;

// Appends a new item to the array
#define DA_APPEND(arr, item) { \
    if ((arr).items == NULL) { \
        (arr).items = kod_calloc(sizeof(item), DA_INIT_CAPACITY); \
        (arr).capacity = DA_INIT_CAPACITY; \
    } else if ((arr).count >= (arr).capacity) { \
        size_t new_cap = (arr).capacity * 2; \
        void* p = kod_realloc((arr).items, new_cap * sizeof(item)); \
        if (!p) { fprintf(stderr, DA_ERROR_PREFIX": "DA_ERROR_REALLOC"\n"); exit(1); } \
        memset((arr).items + (arr).capacity, 0, new_cap - (arr).capacity); \
        (arr).capacity = new_cap; \
        (arr).items = p; \
    } \
    (arr).items[(arr).count++] = item; \
}

// Alias for DA_APPEND
#define DA_PUSH_BACK(arr, item) DA_APPEND(arr, item)

// Returns true if the array is empty
#define DA_IS_EMPTY(arr) ((arr).count == 0)

// Asserts that the array isn't empty
#define DA_ASSERT_NOT_EMPTY(arr) { assert(!(DA_IS_EMPTY(arr)) && DA_ERROR_PREFIX": "DA_ERROR_EMPTY"\n"); }

// Returns the last element of the array
#define DA_TOP(arr, output) { \
    DA_ASSERT_NOT_EMPTY(arr); \
    output = DA_INDEX(arr, (arr).count - 1); \
}

// Alias for DA_TOP
#define DA_LAST(arr, output) DA_TOP(arr, output)

// Pops the last element of the array
#define DA_POP(arr) { \
    DA_ASSERT_NOT_EMPTY(arr); \
    DA_INDEX(arr, (arr).count - 1) = 0; \
    --(arr).count; \
}

// Pops and copies the last element of the array to output
#define DA_POP_OUT(arr, output) { \
    DA_TOP(arr, output); \
    DA_POP(arr); \
}

// Frees the array and zeroes the count and capacity
#define DA_FREE(arr) { kod_free((arr).items); (arr).count=0; (arr).capacity=0; }

// Zeroing the count and the elements
#define DA_CLEAN(arr) { (arr).count=0; memset(arr.items, 0, a.capacity * sizeof(*a.items)); }

// Performs func() on each element of the array (func takes a pointer to that element)
#define DA_FOR_EACH(arr, func) { for (size_t i = 0; i < (arr).count; ++i) { func(&(arr).items[i]); }; }

#endif // DA