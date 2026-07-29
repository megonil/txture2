#ifndef TXTURE2_TABLE_H
#define TXTURE2_TABLE_H

// open addressing
// linear probing
// hashmap

#include "txr/token.h"
#include "txr/value.h"

#include <stdint.h>

typedef uint32_t hash_t;

#define TABLE_INITIAL_CAP 16
#define TABLE_MAX_LOAD 0.75

#define justtable

#define define_tableitemstr(Name, K, V)                                   \
	typedef struct Name##TableItem {                                      \
		K	   key;                                                       \
		V	   value;                                                     \
		hash_t hash;                                                      \
	} Name##TableItem

#define define_tableitem(Name, K, V)                                      \
	typedef struct Name##TableItem {                                      \
		K key;                                                            \
		V value;                                                          \
	} Name##TableItem

#define define_tablestruct(Name)                                          \
	typedef struct Name##Table {                                          \
		Name##TableItem* entries;                                         \
		uint32_t		 cap;                                             \
		uint32_t		 size;                                            \
	} Name##Table

#define define_tablemethods(Name, K, V)                                   \
	void Name##TableInit (Name##Table* table);                            \
	void Name##TableFree (Name##Table* table);                            \
	int	 Name##TableInsert (Name##Table* table, K key, V value);          \
	V*	 Name##TableGet (Name##Table* table, K key);                      \
	int	 Name##TableContains (Name##Table* table, K key);                 \
	void Name##TableRemove (Name##Table* table, K key)

#define define_tablestr(Name, K, V)                                       \
	define_tableitemstr (Name, K, V);                                     \
	define_tablestruct (Name);                                            \
	define_tablemethods (Name, K, V)

#define define_table(Name, K, V)                                          \
	define_tableitem (Name, K, V);                                        \
	define_tablestruct (Name);                                            \
	define_tablemethods (Name, K, V)

define_tablestr (justtable, const char*, int);
define_tablestr (String, const char*, const char*);
define_tablestr (Keyword, const char*, TokenType);
define_tablestr (Variable, const char*, value);

#undef justtable

define_table (Num, int, int);

#define TableInitList(Name, table, ...)                                   \
	do {                                                                  \
		Name##TableInit (table);                                          \
		Name##TableItem _items[] = {__VA_ARGS__};                         \
		uint32_t		_count	 = sizeof (_items) / sizeof (_items[0]);  \
		for (uint32_t _i = 0; _i < _count; _i++) {                        \
			Name##TableInsert (table, _items[_i].key, _items[_i].value);  \
		}                                                                 \
	} while (0)

#endif // !TXTURE2_TABLE_H
