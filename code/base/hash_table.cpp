
must_use internal HT_Entry *
get_free_entry(Hash_Table *ht) {
  Assert(ht);
  Assert(ht->free_entries_cap > 1 && ht->buckets);
  HT_Entry *entry = ht->free_list;
  ErrorIf(!entry, "Hash Table has no free nodes! (all %zu taken)", ht->free_entries_cap);
  DLL_remove(ht->free_list, entry);
  return entry;
}

must_use Hash_Table *
make_hash_table(Arena *arena, u64 sz) {
  Assert(arena);
  Assert(sz > 1);
  Hash_Table *ht    = arena_alloc<Hash_Table>(arena);
  ht->buckets       = arena_alloc_array<HT_Bucket>(arena, sz);
  ht->buckets_sz    = sz / 4; // Assume about 4 items per bucket.
  ht->free_entries_cap            = sz;
  HT_Entry *entries = arena_alloc_array<HT_Entry>(arena, sz);
  for (u64 i = 0; i < sz; i++) {
    DLL_insert_at_front(ht->free_list, &(entries[i]));
  }
  return ht;
}

must_use void *
ht_find(Hash_Table *ht, u64 hash) {
  Assert(ht);
  Assert(ht->free_entries_cap > 1 && ht->buckets);

  u64 const idx = (hash % ht->buckets_sz);
  HT_Entry *it  = ht->buckets[idx].root;
  while (it) {
    if (it->hash == hash) {
      return it->value;
    }
    it = it->next;
  }
  return 0;
}

void
ht_insert(Hash_Table *ht, u64 hash, void *value) {
  Assert(ht);
  Assert(ht->free_entries_cap > 1 && ht->buckets);
  Assert(value);

  HT_Entry *entry = get_free_entry(ht);
  entry->hash     = hash;
  entry->value    = value;

  u64 const  idx  = (hash % ht->buckets_sz);
  HT_Entry *&root = ht->buckets[idx].root;
  DLL_insert_at_front(root, entry);
}

void
ht_erase(Hash_Table *ht, u64 hash) {
  Assert(ht);
  Assert(ht->free_entries_cap > 1 && ht->buckets);

  u64 const  idx  = (hash % ht->buckets_sz);
  HT_Entry *&root = ht->buckets[idx].root;
  HT_Entry  *it   = root;
  while (it) {
    if (it->hash == hash) {
      break;
    }
    it = it->next;
  }
  if (!it) {
    // Entry not found, nothing to erase.
    return;
  }
  DLL_remove(root, it);
  DLL_insert_at_front(ht->free_list, it);
}
