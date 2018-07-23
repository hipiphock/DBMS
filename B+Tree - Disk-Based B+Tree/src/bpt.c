#include "bpt.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define PAGE_SIZE 4096
#define RECORD_SIZE 128


// BASIC STRUCTURES

// first page, offset 0-4095, contains metadata
typedef union _HeaderPage{
    //free page offset
    int64_t freePageOffset;
    //root page offset
    int64_t rootPageOffset;
    //numbe of pages
    int64_t numOfPages;
    // empty space
    char reserved[4072];
} HeaderPage;

// Free Page
typedef union _FreePage{
    //next page offset
    int64_t nextFreePage;
    // empty space 
    char reserved[4088];
} FreePage;

// Page Header inside Leaf Page
typedef union _PageHeader{
    // parent page offset
    int64_t parentPageOffset;
    // is leaf?
    int32_t isLeaf;
    // number of keys
    int32_t numOfKeys;
    // empty space
    char reserved[104];
    // right sibling page offset
    int64_t rightSiblingPageOffset;
} PageHeader;

// Record inside Leaf Page
typedef union _Record{
    int64_t key;
    char value[120];
} Record;

// leaf page
typedef union _LeafPage{
    PageHeader pageHeader;
    Record[31] record;
} LeafPage;

// key-offset is needed in Internal Page
typedef union _KeyOffset{
    int64_t key;
    int64_t pageOffset;
} KeyOffset;

typedef union _InternalPage{
    PageHeader pageHeader;
    KeyOffset keyOffset[248];
} InternalPage;

// GLOBAL VARIABLES

// file descriptor
int fd;

// header page
HeaderPage* headerPage;

// FUNCTIONS

// reading file
void* read_db(int64_t offset){
    void* page = calloc(0, PAGE_SIZE);
    lseek(fd, (off_t)offset, SEEK_SET);
    if(read(fd, page, PAGE_SIZE) < 0){
        free(page);
        return NULL;
    }
    return page;
}

// writing file
void write_db(void* page, int64_t offset){
    lseek(fd, (off_t)offset, SEEK_SET);
    write(fd, page, PAGE_SIZE);
    fsync(fd);
}

// for opening DB
int open_db(const char* pathname){
    fd = open(pathname, O_RDWR);
    if(fd < 0){
        fd = open(pathname, O_CREAT);
        if(fd < 0)  return -1;
        headerPage->freePageOffset = PAGE_SIZE;
        headerPage->rootPageOffset = PAGE_SIZE * 2;
        headerPage->numOfPages = 0;
        write_db(headerPage, PAGE_SIZE);
        return 0;
    }
    return 0;
}

// for finding

// finding leaf
LeafPage* find_leaf(int64_t key){
    // first, fetch the root node
    InternalPage* internalPage = (InternalPage)*read_db(headerPage->rootPageOffset);
    while(!internalPage->pageHeader.isLeaf){
        int left = 0, right = internalPage->pageHeader.numOfKeys - 1;
        while(left < right){
            int mid = (left + right) / 2;
            int64_t mid_key = internalPage->keyOffset[mid].key;
            if(key < mid_key) right = mid - 1;
            else left = mid + 1
        }
        free(internalPage);
        int64_t next_offset = internalPage->keyOffset[right].pageOffset;
        internalPage = (InternalPage*)read_db(next_offset);
    }
    return (LeafPage*)internalPage;
}

// find function
char* find(int64_t key){
    LeafPage* leafPage = find_leaf(key);
    int left = 0, right = leafPage->pageHeader.numOfKeys - 1;
    while(left < right){
        int mid = (left + right) / 2;
        int64_t mid_key = leafPage->record[mid].key;
        if(key < mid_key) right = mid - 1;
        else if(key > mid_key) left = mid + 1;
        else{
            free(leafPage);
            return leafPage->record[mid].value;
        }
    }
    free(leafPage);
    return NULL;
}

// INSERTION
Record make_record(int64_t key, char* value){
    Record* record;
    record = (Record*)malloc(sizeof(record));
    // error handling needed
    record->key = key;
    Record->value = value;
    return record;
}

int insert(int64_t key, char* value){
    if(find(key) == NULL){
        Record* newRecord = make_record(key, value);
        // key에 근접한 자리를 찾자.
    }
}