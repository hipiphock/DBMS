#include "file.h"
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

HeaderPage headerPage;
int fd;

// get free page
// return free page offset
off_t get_free_page(){
    off_t free_page_offset;
    free_page_offset = headerPage.free_page_offset;

    if(free_page_offset == 0){  // case when there is no free page left
        expand_file(headerPage.num_pages);
        free_page_offset = headerPage.free_page_offset;
    }

    FreePage freePage;
    load_page(free_page_offset, (Page*)&FreePage);
    headerPage.free_page_offset = freePage.next;
    flush_page((Page*)&headerPage);
    
    return free_page_offset;
}

// Put free page to the free list
void put_free_page(off_t page_offset) {
    FreePage freepage;
    memset(&freepage, 0, PAGE_SIZE);

    freepage.next = headerPage.freelist;
    freepage.file_offset = page_offset;
    flush_page((Page*)&freepage);
    
    headerPage.freelist = page_offset;

    flush_page((Page*)&headerPage);
}

// Expand file pages and prepend them to the free list
void expand_file(size_t cnt_page_to_expand) {
    off_t offset = headerPage.num_pages * PAGE_SIZE;

    if (headerPage.num_pages > 1024 * 1024) {
        // Test code: do not expand over than 4GB
        assert("Test: you are already having a DB file over than 4GB");
    }
    
    int i;
    for (i = 0; i < cnt_page_to_expand; i++) {
        put_free_page(offset);
        headerPage.num_pages++;
        offset += PAGE_SIZE;
    }

    flush_page((Page*)&headerPage);
}

// load page
void load_page(off_t offset, Page* page) {
    lseek(fd, offset, SEEK_SET);
    read(fd, page, PAGE_SIZE);
    page->file_offset = offset;
}

// flush into file
void flush_page(Page* page) {
    lseek(fd, page->file_offset, SEEK_SET);
    write(fd, page, PAGE_SIZE);
}

