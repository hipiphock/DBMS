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
            if(isBetween(internalPage, key, mid) == 1){
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
    LeafPage* leafPage = find_leaf(key);
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
            return leafPage->records[mid].value;
        }
    }
    // no matching value - error
    return NULL;
}


// INSERTION
// split leaf node
// returns splitted node (new node)
LeafPage* leaf_node_split(LeafPage* node_to_split){

    // creat new leaf node
    LeafPage* node_splited;
    node_splited = (LeafPage*)malloc(sizeof(LeafPage));
    assert(node_splited != NULL);

    // initialize
    node_splited.num_keys = 0;
    node_splited.is_leaf = 1;
    node_splited.file_offset = get_free_page();
    node_splited.parent = node_to_split.parent;
    node_splited.sibling = node_to_split.sibling;
    node_to_split.sibling = node_splited.file_offset;

    // copy half of the records
    int i = 0, prev_keys = node_to_split->num_keys, mid = prev_keys / 2;
    for(i = 0; mid + i < prev_keys; i++){
        memcpy(node_splited->records[i], node_to_split->records[mid + i], sizeof(Record));
        node_to_split->num_keys--;
        node_splited->num_keys++;
    }

    // look if parent needs spliting
    // InternalPage* node_to_verify;
    // node_to_verify = (InternalPage*)malloc(sizeof(InternalPage));
    // assert(node_to_verify != NULL);
    // node_to_verify = load_page(node_to_split.parent, (Page*)node_to_verify);
    
    // 아 새로 만든 leaf node의 부모를 어떻게 처리를 해야하지?
    // 우선 부모에는 항상 가운데 놈을 넣어준다.
    // 즉, split된 놈의 맨 앞을 올려준다.
    InternalPage* parent;
    load_page(node_splited->parent, (Page*)parent);
    insert_into_parent(parent, node_splited->records[0].key);
}

// split internal node
// returns splitted node (new node)
InternalPage* internal_node_split(InternalPage* node_to_split){
    
}

// insert into leaf
void insert_into_leaf(LeafPage* leafPage, int64_t key, const char* value){
    int i, j;
    //move records one step right
    for(i = 0; i < LEAF_ORDER - 2;){
        if(leafPage->records[i].key < key){
            i++;
            continue;
        } else {
            for(j = leafPage->num_keys; j > i; j--)
                memcpy(leafPage->records[j], leafPage->records[j - 1], sizeof(Record));
            
            // insert key and value
            leafPage->records[i].key = key;
            memcpy(leafPage->records[i].value, value, VALUE_SIZE);
            break;
        }
    }
    leafPage->num_keys++;

    // flush leaf node to the file page
    flush_page((Page*)leafPage);
}

// insert into internal node
void insert_into_parent(InternalPage* parent_to_insert, int64_t key){
    int i, j;

    // case : no splitting is needed,
    // parent_to_insert have less than INTERNAL_ORDER - 1 keys
    if(parent_to_insert->num_keys < INTERNAL_ORDER - 1){
        // move internal records one step right
        for(i = 0; i < INTERNAL_ORDER - 2; i++){
            if(parent_to_insert->internalRecords[i].key < key){
                i++;
                continue;
            } else {
                for(j = parent_to_insert->num_keys - 1; j > i; j--)
                    memcpy(parent_to_insert->internalRecords[j]. parent_to_insert->internalRecords[j - 1], sizeof(InternalRecord));

                //insert key
                parent_to_insert->internalRecords[i].key = key;
                break;
            }   
        }
        parent_to_insert->num_keys++;
        
        // flush internal node to the file page
        flush_page((Page*)parent_to_insert);
    }

    // case : splitting is needed,
    // parent have INTERNAL_ORDER - 1 keys
    else{
        
    }
}

// master function
int insert(int64_t key, const char* value){

    // case : the key already exist
    if(find(key) != NULL){
        // already exist!
        return -1;
    }

    LeafPage* leaf_to_insert = find_leaf(key);

    // case : inserting without splitting
    if(leaf_to_insert->num_keys < LEAF_ORDER - 1){
        insert_into_leaf(leaf_to_insert, key, value);
        return 0;
    }

    // case : splitting is needed (current leaf_to_insert have LEAF_ORDER - 1 keys)
    else{
        // first, just insert
        insert_into_leaf(leaf_to_insert, key, value);
        // then, split
        leaf_node_split(leaf_to_insert);
        return 0;
    }
}