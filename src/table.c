#include "table.h"

#include "txr/macro.h"
#include "txr/token.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE_K(K) (K){0}
#define TOMBSTONE_V(V) (V){0}

static hash_t
murmur (const char* value)
{
	const uint64_t		 m	 = UINT64_C (0xc6a4a7935bd1e995);
	const size_t		 len = strlen (value);
	const unsigned char* p	 = (const unsigned char*) value;
	const unsigned char* end = p + (len & ~(uint64_t) 0x7);
	uint64_t			 h	 = (len * m);

	while (p != end) {
		uint64_t k;
		memcpy (&k, p, sizeof (k));

		k *= m;
		k ^= k >> 47u;
		k *= m;

		h ^= k;
		h *= m;
		p += 8;
	}

	switch (len & 7u) {
		case 7: h ^= (uint64_t) p[6] << 48ul;
		case 6: h ^= (uint64_t) p[5] << 40ul;
		case 5: h ^= (uint64_t) p[4] << 32ul;
		case 4: h ^= (uint64_t) p[3] << 24ul;
		case 3: h ^= (uint64_t) p[2] << 16ul;
		case 2: h ^= (uint64_t) p[1] << 8ul;
		case 1:
			h ^= (uint64_t) p[0]; // fall through
			h *= m;
		default: break;
	}

	h ^= h >> 47u;
	h *= m;
	h ^= h >> 47u;

	return (hash_t) h;
}

static hash_t
hash32 (uint32_t i)
{ return (hash_t) i; }

static int
strequ (const char* a, const char* b)
{ return strcmp (a, b) == 0; }

static int
intcmp (int a, int b)
{ return a == b; }

static hash_t
hashdbl (double x)
{
	uint64_t bits;
	memcpy (&bits, &x, sizeof (bits));
	return bits;
}

static int
dblequ (double x, double y)
{ return x == y; }

#define implementfull(Name, K, V, cmp, hashfn)                            \
                                                                          \
	static Name##TableItem* Name##probe (                                 \
		Name##Table*	 table,                                           \
		Name##TableItem* dest,                                            \
		K				 key,                                             \
		size_t			 capacity)                                        \
	{                                                                     \
		hash_t			 hash  = hashfn (key);                            \
		uint32_t		 index = hash % capacity;                         \
		Name##TableItem* tomb  = 0;                                       \
                                                                          \
		for (;;) {                                                        \
			Name##TableItem* place = dest + index;                        \
			if (place->key == TOMBSTONE_K (K)) {                          \
				if (memcmp (&place->value, &TOMBSTONE_V (V), sizeof (V))  \
					== 0) {                                               \
					return tomb != 0 ? tomb : place;                      \
				} else if (tomb == 0) {                                   \
					tomb = place;                                         \
				}                                                         \
			} else if (Name##TableCmp (place, key, hash)) {               \
				return place;                                             \
			}                                                             \
                                                                          \
			index = (index + 1) % table->cap;                             \
		}                                                                 \
	}                                                                     \
                                                                          \
	static void Name##resize (Name##Table* table, size_t newcapacity)     \
	{                                                                     \
		Name##TableItem* newentries                                       \
			= calloc (1, newcapacity * sizeof (Name##TableItem));         \
                                                                          \
		for (int i = 0; i < table->cap; ++i) {                            \
			Name##TableItem* item = table->entries + i;                   \
			if (item->key == TOMBSTONE_K (K)) { continue; }               \
                                                                          \
			Name##TableItem* dest = Name##probe (                         \
				table, newentries, item->key, newcapacity);               \
			Name##TableAssign (                                           \
				dest, item->key, item->value, Name##TableHashOf (item));  \
		}                                                                 \
                                                                          \
		free (table->entries);                                            \
                                                                          \
		table->entries = newentries;                                      \
		table->cap	   = newcapacity;                                     \
	}                                                                     \
                                                                          \
	int Name##TableInsert (Name##Table* table, K key, V value)            \
	{                                                                     \
		if (table->size >= table->cap * TABLE_MAX_LOAD) {                 \
			Name##resize (table, table->cap * 2);                         \
		}                                                                 \
                                                                          \
		Name##TableItem* dest                                             \
			= Name##probe (table, table->entries, key, table->cap);       \
		int isnew = dest->key == TOMBSTONE_K (K);                         \
		if (isnew                                                         \
			&& memcmp (&dest->value, &TOMBSTONE_V (V), sizeof (V))        \
				   == 0) {                                                \
			table->size++;                                                \
		}                                                                 \
		Name##TableAssign (dest, key, value, hashfn (key));               \
		return isnew;                                                     \
	}                                                                     \
                                                                          \
	V* Name##TableGet (Name##Table* table, K key)                         \
	{                                                                     \
		if (table->size == 0) { return 0; }                               \
                                                                          \
		Name##TableItem* item                                             \
			= Name##probe (table, table->entries, key, table->cap);       \
                                                                          \
		if (item->key == TOMBSTONE_K (K)) { return 0; }                   \
                                                                          \
		return &item->value;                                              \
	}                                                                     \
                                                                          \
	int Name##TableContains (Name##Table* table, K key)                   \
	{                                                                     \
		Name##TableItem* item                                             \
			= Name##probe (table, table->entries, key, table->cap);       \
		if (item->key == TOMBSTONE_K (K)) { return 0; }                   \
                                                                          \
		return 1;                                                         \
	}                                                                     \
                                                                          \
	void Name##TableRemove (Name##Table* table, K key)                    \
	{                                                                     \
		if (table->size == 0) { return; }                                 \
                                                                          \
		Name##TableItem* item                                             \
			= Name##probe (table, table->entries, key, table->cap);       \
		item->key	= TOMBSTONE_K (K);                                    \
		item->value = TOMBSTONE_V (V);                                    \
	}                                                                     \
                                                                          \
	void Name##TableInit (Name##Table* table)                             \
	{                                                                     \
		table->cap	= TABLE_INITIAL_CAP;                                  \
		table->size = 0;                                                  \
		table->entries                                                    \
			= calloc (1, table->cap * sizeof (Name##TableItem));          \
	}                                                                     \
	void Name##TableFree (Name##Table* table)                             \
	{                                                                     \
		assert (table->entries != NULL);                                  \
		free (table->entries);                                            \
		table->entries = NULL;                                            \
		table->cap	   = 0;                                               \
		table->size	   = 0;                                               \
		table		   = 0;                                               \
	}

#define implement(Name, K, V, cmp, hashfn)                                \
	static int Name##TableCmp (Name##TableItem* a, K key, hash_t hash)    \
	{ return cmp (a->key, key); }                                         \
	static void Name##TableAssign (                                       \
		Name##TableItem* item, K key, V value, hash_t hash)               \
	{                                                                     \
		(void) hash;                                                      \
		item->value = value;                                              \
		item->key	= key;                                                \
	}                                                                     \
	static inline hash_t Name##TableHashOf (Name##TableItem* item)        \
	{ return hashfn (item->key); }                                        \
                                                                          \
	implementfull (Name, K, V, cmp, hashfn)

#define implementstr(Name, K, V, cmp, hashfn)                             \
	static int Name##TableCmp (Name##TableItem* a, K key, hash_t hash)    \
	{ return a->hash == hash && cmp (a->key, key); }                      \
	static void Name##TableAssign (                                       \
		Name##TableItem* item, K key, V value, hash_t hash)               \
	{                                                                     \
		item->key	= key;                                                \
		item->value = value;                                              \
		item->hash	= hash;                                               \
	}                                                                     \
	static inline hash_t Name##TableHashOf (Name##TableItem* item)        \
	{ return item->hash; }                                                \
                                                                          \
	implementfull (Name, K, V, cmp, hashfn)

implementstr (, const char*, int, strequ, murmur);
implementstr (String, const char*, const char*, strequ, murmur);
implementstr (Keyword, const char*, TokenType, strequ, murmur);
implementstr (VariableSet, const char*, size_t, strequ, murmur);
implementstr (Variable, const char*, tvalue, strequ, murmur);
implementstr (Macro, char*, Macro, strequ, murmur);
implementstr (Arg, char*, ExtendedToken*, strequ, murmur);
implementstr (Pragma, const char*, PragmaKind, strequ, murmur);

implement (Num, int, int, intcmp, hash32);
implement (Constant, tvalue, uint32_t, dblequ, hashdbl);
