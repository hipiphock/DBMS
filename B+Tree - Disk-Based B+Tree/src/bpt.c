#include "bpt.h"
#include "file.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

// GLOBAL VARIABLES
extern HeaderPage headerPage;
extern int fd;


// OPEN DB FILE
// open a db file
int open_db(const char* pathname){
    fd = open(pathname, O_RDWR);
    if(fd < 0){
        //create a new file
        fd = open(pathname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if(fd < 0){
            //error : file is not created
            return -1;
        }
    } else {
        // file exist, so read it.
        load_page(0, (Page*)&headerPage);
        headerPage.file_offset = 0;
    }
    return 0;
}

// Close a db file
void close_db(){
    close(fd);
}


// FINDING
// finds leaf
// should handle binary search part
LeafPage* find_leaf(int64_t key){
    LeafPage* leafPage;
    off_t root_offset = headerPage.root_offset;

    if(root_offset == 0){
        // there is no root
        return NULL;
    }

    NodePage page;
    load_page(root_offset, (Page*)&page);

    while(!page.is_leaf){
        InternalPage* internalPage = (InternalPage*)&page;
        int64_t left = 0, right = INTERNAL_ORDER - 1, mid;
        while(left <= right){
            mid = (left + right) / 2;
            if(key > InternalPage->internalRecords[mid].key){
                left = mid + 1;
            } else if (key < InternalPage->internalRecords[mid].key){
                right = mid - 1;
            } else {
                load_page(InternalPage->internalRecords[mid].offset, (Page*)&page);
                break;
            }
        }
        if(key == InternalPage->internalRecords[mid].offset) continue;
        else{
            // there is no matching key in file
            return NULL;
        }
    }
    return (LeafPage*)&page;
}

// returns value paired with key
char* find(int64_t key){
    char* ret_value;
    LeafPage leafPage = find_leaf(key);
    if(leafPage == NULL){
        // no matching value
        return NULL;
    }
    int64_t left = 0, right = LEAF_ORDER - 2, mid;
    while(left <= right){
        mid = (left + right) / 2;
        if(key > leafPage->records[mid].key){
            left = mid + 1;
        } else is (key < leafPage->records[mid].key){
            right = mid - 1;
        } else {
            ret_value = (char*)malloc(VALUE_SIZE * sizeof(char));
            memcpy(ret_value, leafPage->records[mid].value);
            return ret_value;
        }
    }
    // no matching value - error
    return NULL;
}


// INSERTION
// insert into leaf
void insert_into_leaf(){

}

// master function
int insert(int64_t key, const char* value){

    // case : the key already exist
    if(find(key) != NULL){
        // already exist!
        return -1;
    }

    // case : inserting without splitting
    if(find_leaf(key)->num_keys < LEAF_ORDER - 1){
        
    }
}