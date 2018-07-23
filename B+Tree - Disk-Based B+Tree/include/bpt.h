#ifndef _BPT_H_
#define _BPT_H_

int open_db(const char* pathname);
int insert(int64_t key, char* value);
char* find(int64_t key);
int delete(int64_t key);

#endif