#pragma once

struct HT_Entry {
  HT_Entry *next  = 0;
  HT_Entry *prev  = 0;
  u64       hash  = 0;
  void     *value = 0;
};

struct HT_Bucket {
  HT_Entry *root;
};

// Basic, fixed-size hash table without collision handling.
//
struct Hash_Table {
  HT_Bucket *buckets;
  u64        buckets_sz;
  HT_Entry  *free_list;
  u64        free_entries_cap;
};

must_use Hash_Table *
make_hash_table(Arena *arena, u64 sz);

must_use void *
ht_find(Hash_Table *ht, u64 hash);

void
ht_insert(Hash_Table *ht, u64 hash, void *value);

void
ht_erase(Hash_Table *ht, u64 hash);
