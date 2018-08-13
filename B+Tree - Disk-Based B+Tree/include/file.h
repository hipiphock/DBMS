#include <stddef.h>
#include <inttypes.h>

#define PAGE_SIZE 4096

#define INTERNAL_ORDER 5
#define LEAF_ORDER 5

#define KEY_SIZE 8
#define VALUE_SIZE 120
#define RECORD_SIZE (KEY_SIZE + VALUE_SIZE)

typedef struct _Record {
    int64_t key;
    char value[SIZE_VALUE];
} Record;

typedef struct _InternalRecord {
    int64_t key;
    off_t offset;
} InternalRecord;

typedef struct _Page {
    char data[PAGE_SIZE];
    
    // in-memory data
    off_t file_offset;
} Page;

typedef struct _FreePage {
    off_t next;
    char reserved[PAGE_SIZE - 8];

    // in-memory data
    off_t file_offset;
} FreePage;

typedef struct _HeaderPage {
    union{
        struct{
            off_t free_page_offset;
            off_t root_offset;
            uint64_t num_pages;
        }
        char reserved[PAGE_SIZE];
    }

    // in-memory data
    off_t file_offset;
} HeaderPage;

typedef struct _InternalPage {
    union {
        struct {
            off_t parent;
            int is_leaf;
            int num_keys;
            char reserved[112 - 16];
            InternalRecord internalRecords[INTERNAL_ORDER];
        };
        char space[PAGE_SIZE];
    };
    // in-memory data
    off_t file_offset;
} InternalPage;

typedef struct _LeafPage {
    union {
        struct {
            off_t parent;
            int is_leaf;
            int num_keys;
            char reserved[120 - 16];
            off_t sibling;
            Record records[LEAF_ORDER - 1];
        };
        char space[PAGE_SIZE];
    };

    // in-memory data
    off_t file_offset;
} LeafPage;

// think as parent class of internal page and leaf page
typedef struct _NodePage {
    union {
        struct {
            off_t parent;
            int is_leaf;
            int num_keys;
        };
        char space[PAGE_SIZE];
    };

    // in-memory data
    off_t file_offset;
} NodePage;

// Get free page to use
off_t get_free_page();

// Put free page to the free list
void put_free_page(off_t page_offset);

// Expand file size and prepend pages to the free list
void expand_file(size_t cnt_page_to_expand);

// Load file page into the in-memory page
void load_page(off_t offset, Page* page);

// Flush page into the file
void flush_page(Page* page);