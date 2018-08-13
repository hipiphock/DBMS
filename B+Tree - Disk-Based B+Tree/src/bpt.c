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

// helper function used in find_leaf(int64_t key);
int isBetween(InternalPage* ip, int64_t key, int64_t mid){
    int64_t ir1_key = ip->internalRecords[mid].key;
    int64_t ir2_key = ip->internalRecords[mid + 1].key;
    if(ir1_key <= key && key < ir2_key)
        return 1; 
    else if(key < ir1_key)
        return -1;
    else if(key > ir2_key)
        return 0;
}

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
        int64_t left = 1, right = INTERNAL_ORDER - 1, mid;
        while(left <= right){
            mid = (left + right) / 2;
            if(isBetween(internalPage, key, mid)){
                load_page(internalPage->internalRecords[mid]->offset, internalPage);
                break;
            } else if(isBetween(internalPage, key, mid) == -1){
                right = mid + 1;
            } else if(isBetween(internalPage, key, mid) == 0){
                 left = mid - 1;
            }
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
void insert_into_leaf(LeafPage* leafPage){
    int i;
    for(i = 0; i < LEAF_ORDER - 2){
        
    }
}

// master function
int insert(int64_t key, const char* value){

    // case : the key already exist
    if(find(key) != NULL){
        // already exist!
        return -1;
    }

    // case : inserting without splitting
    LeafPage* leaf_to_insert = find_leaf(key);
    if(leaf_to_insert->num_keys < LEAF_ORDER - 1){
        insert_into_leaf(leaf_to_insert);
        return 0;
    }

    // case : splitting is needed 
}