#pragma once

#include "lib.h"

struct dict_s {
    int hash_content; // 0: hash pointer, otherwise hash content
    size_t nbits;     // number of bits of the mask
    size_t mask;      // mask for the number of buckets

    size_t capacity;        // number of buckets
    size_t *keys;           // keys array
    size_t *values;         // values array
    size_t nitems;          // number of items
    size_t n_deleted_items; // number of deleted items
};
/**
 * @brief dictionary data structure that maps uniques keys to some value
 *
 */
typedef struct dict_s dict_t;

struct dict_itr_s {
    dict_t *dict;
    size_t index;
};
/**
 * @brief iterator for dictionary, somehow order is preserved but not guaranteed
 *
 */
typedef struct dict_itr_s dict_itr_t;

struct dict_args_s {
    int hash_content;
};
typedef struct dict_args_s dict_args_t;

/**
 * @brief new dictionary
 *
 * @return dict_t*
 */
dict_t *_dict_new(int hash_content);

dict_t *_dict_new_args(dict_args_t args);

#define dict_new(...) _dict_new_args((dict_args_t){__VA_ARGS__})

/**
 * @brief free dictionary underlying structure.
 * It is caller responsibility to free the values if needed
 *
 * @param dict dictionary to free
 */
void dict_free(dict_t *dict);

/**
 * @brief add new key-value pair to dictionary.
 * If key already exists, the value is replaced.
 *
 * @param dict dictionary
 * @param key key
 * @param value value
 * @return int - `-1` if error (bad key value),
 * `0` if already present (value replaced),
 * `1` if added (new pair),
 * `2` if rehash did not work (dict integrity compromised)
 */
int dict_push(dict_t *dict, void *key, void *value);

/**
 * @brief get value for given key
 *
 * @param dict dictionary
 * @param key key
 * @return size_t - value or 0 if key not found
 */
size_t dict_get(dict_t *dict, void *key);

/**
 * @brief remove key-value pair from dictionary
 *
 * @param dict dictionary
 * @param item key of the pair to remove
 * @return int - -1 if error (bad key value),
 * `0` if not found (no change),
 * `1` if removed (key-value pair removed),
 */
int dict_discard(dict_t *dict, void *key);

/**
 * @brief return a new dictionary
 * Does not copy any of the key-value pairs
 *
 * @param dict dictionary
 * @return dict_t* - copy of the original dictionary
 */
dict_t *dict_copy(dict_t *dict);

/**
 * @brief merge the second dictionary into the first one.
 * Does not copy any of the key-value pairs.
 *
 * @param dict1 first dictionary
 * @param dict2 second dictionary
 * @return size_t* - number of items added to the first dictionary
 */
size_t dict_merge(dict_t *dict1, dict_t *dict2);

/**
 * @brief get number of items in dictionary
 *
 * @param dict dictionary
 * @return size_t
 */
size_t dict_nitems(dict_t *dict);

/**
 * @brief create a new iterator for dictionary, advance to first item
 *
 * @param dict dictionary
 * @return dict_itr_t*
 */
dict_itr_t *dict_itr_new(dict_t *dict);

/**
 * @brief free iterator only
 *
 * @param itr iterator
 */
void dict_itr_free(dict_itr_t *itr);

/**
 * @brief get next key from iterator
 *
 * @param itr iterator
 * @return size_t - pointer to key
 */
size_t dict_itr_key(dict_itr_t *itr);

/**
 * @brief get next value from iterator
 *
 * @param itr iterator
 * @return size_t - pointer to value
 */
size_t dict_itr_value(dict_itr_t *itr);

/**
 * @brief return 1 if can advance, 0 otherwise
 *
 * @param itr iterator
 * @return int - 0 if we reached the end, 1 otherwise
 */
int dict_itr_has_next(dict_itr_t *itr);

/**
 * @brief check if iterator can advance, if so advances.
 * Returns current index if can advance and -1 otherwise
 *
 * @param itr iterator
 * @return size_t
 */
size_t dict_itr_next(dict_itr_t *itr);

/**
 * @brief reset the iterator to the beginning of the dictionary
 *
 * @param itr iterator
 */
void dict_itr_reset(dict_itr_t *itr);

/**
 * @brief iterate over dictionary and call the callback for each key-value pair
 * if the function returns non-null, the iteration is stopped
 * and the value is returned
 *
 * @details it is the caller's responsibility to reset the iterator
 * after calling this function
 *
 * @param itr dictionary iterator
 * @param fe1 function to call for each key
 * @param data1 data to pass to callback
 * @param fe2 function to call for each value
 * @param data2 data to pass to callback
 * @return void* - data returned by callback if non-null
 */
void *dict_itr_for_each(dict_itr_t *itr, for_each_callback_t *fe1, void *data1,
                        for_each_callback_t *fe2, void *data2);

/**
 * @brief discard all key-value pairs from dictionary
 *
 * @param itr dictionary iterator
 * @param dc1 delete callback for each key
 * @param dc2 delete callback for each value
 */
void dict_itr_discard_all(dict_itr_t *itr, delete_callback_t *dc1,
                          delete_callback_t *dc2);
