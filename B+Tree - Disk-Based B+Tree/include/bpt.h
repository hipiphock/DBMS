#ifndef _BPT_H__
#define _BPT_H__

int open_db(const char* pathname);
int insert(int64_t key, const char* value);
char* find(int64_t key);
int delete(int64_t key);

#endif