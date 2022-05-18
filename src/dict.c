#include "dict.h"

static const unsigned int prime_1 = 3079;
static const unsigned int prime_2 = 1572869;

unsigned long hash(char *str) {
    if (str == NULL)
        return 0;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int compare(int hash_content, size_t item, size_t value) {
    switch (hash_content) {
    case 0:
        return item == value;
    case 1:
    default:
        return strcmp((char *)value, (char *)item) == 0;
    }
}

dict_t *_dict_new_args(dict_args_t args) {
    int hash_content = args.hash_content ? args.hash_content : 0;
    return _dict_new(hash_content);
}

dict_t *_dict_new(int hash_content) {
    dict_t *d = calloc(1, sizeof(struct dict_s));

    if (d == NULL) // out of memory
        return NULL;

    d->hash_content = hash_content;
    d->nbits = 3;                                    // 2^3 = 8 buckets
    d->capacity = (size_t)(1 << d->nbits);           // is increased when needed
    d->mask = d->capacity - 1;                       // 2^3 - 1 = 7
    d->keys = calloc(d->capacity, sizeof(size_t));   // keys array
    d->values = calloc(d->capacity, sizeof(size_t)); // values array
    if (d->keys == NULL || d->values == NULL) {
        dict_free(d);
        return NULL;
    }
    d->nitems = 0;          // no items yet
    d->n_deleted_items = 0; // no deleted items yet
    return d;
}

void dict_free(dict_t *d) {
    if (d) {
        free(d->keys);
        free(d->values);
    }
    free(d);
}

int _dict_push_item(dict_t *dict, void *key, void *value) {
    size_t k;
    size_t v = (size_t)value; // cast value to size_t
    size_t ii;                // index of the bucket

    switch (dict->hash_content) {
    case 0:
        k = (size_t)key;
        break;
    case 1:
    default:
        k = hash((char *)key);
        break;
    }

    if (k == 0 || k == 1) // if key casts to 0 or 1
        return -1;        // return error

    ii = dict->mask & (prime_1 * k); // hash value modulo capacity

    while (dict->keys[ii] != 0 &&
           dict->keys[ii] != 1) // find empty or deleted bucket
    {
        // if key is already in set
        if (compare(dict->hash_content, dict->keys[ii], (size_t)key)) {
            dict->values[ii] = v; // update value
            return 0;             // return success 0
        } else {
            ii = dict->mask & (ii + prime_2); // get index of next bucket
        }
    }
    dict->nitems++;              // increase number of items
    if (dict->keys[ii] == 1)     // if bucket is deleted
        dict->n_deleted_items--; // decrease number of deleted items

    dict->keys[ii] = (size_t)key; // insert key
    dict->values[ii] = v;         // insert value
    return 1;                     // return success 1
}

static int _maybe_rehash(dict_t *dict) {
    size_t *old_keys;        // old keys array
    size_t *old_values;      // old values array
    size_t old_capacity, ii; // old capacity, index of the bucket

    if (dict->nitems + dict->n_deleted_items >= (double)dict->capacity * 0.85) {
        old_keys = dict->keys;
        old_values = dict->values;
        old_capacity = dict->capacity;
        dict->nbits++;
        dict->capacity = (size_t)(1 << dict->nbits);
        dict->mask = dict->capacity - 1;
        dict->keys = calloc(dict->capacity, sizeof(size_t));
        dict->values = calloc(dict->capacity, sizeof(size_t));
        if (dict->keys == NULL || dict->values == NULL) {
            free(dict->keys);
            free(dict->values);
            dict->keys = old_keys;
            dict->values = old_values;
            dict->capacity = old_capacity;
            dict->nbits--;
            return 2;
        }

        dict->nitems = 0;
        dict->n_deleted_items = 0;
        ASSERT(dict->keys);
        ASSERT(dict->values);
        for (ii = 0; ii < old_capacity; ii++) {
            if (old_keys[ii] != 0 && old_keys[ii] != 1) {
                _dict_push_item(dict, (void *)old_keys[ii],
                                (void *)old_values[ii]);
            }
        }
        free(old_keys);
        free(old_values);
    }
    return 1;
}

int dict_push(dict_t *dict, void *key, void *value) {
    if (dict == NULL) // if dict is NULL
        return -1;    // return error

    int rv = _dict_push_item(dict, key, value); // insert item
    int r = _maybe_rehash(dict);                // rehash if needed
    return rv == 1 ? r : rv;
}

size_t dict_get(dict_t *dict, void *key) {
    if (dict == NULL || key == NULL)
        return 0; // return error

    size_t k;
    switch (dict->hash_content) {
    case 0:
        k = (size_t)key; // cast key to size_t
        break;
    case 1:
    default:
        k = hash((char *)key); // hash key before casting
        break;
    }

    // if dict is NULL or key casts to 0 or 1
    if (dict == NULL || k == 0 || k == 1)
        return 0; // return error

    size_t ii = dict->mask & (prime_1 * k); // hash value modulo capacity

    // continue searching for item until empty bucket is found
    while (dict->keys[ii] != 0) {
        // if key is found
        if (compare(dict->hash_content, dict->keys[ii], (size_t)key)) {
            return dict->values[ii]; // return value
        } else {
            ii = dict->mask & (ii + prime_2); // get index of next bucket
        }
    }
    return 0; // return 0 if key is not found
}

int dict_discard(dict_t *dict, void *item) {
    if (dict == NULL || item == NULL)
        return 0; // return error

    size_t key;
    switch (dict->hash_content) {
    case 0:
        key = (size_t)item;
        break;
    case 1:
    default:
        key = hash((char *)item);
        break;
    }

    // if dict is NULL or key casts to 0 or 1
    if (dict == NULL || key == 0 || key == 1)
        return -1; // return error

    size_t ii = dict->mask & (prime_1 * key); // hash value modulo capacity

    // continue searching for item until empty bucket is found
    while (dict->keys[ii] != 0) {
        // if key is found
        if (compare(dict->hash_content, dict->keys[ii], (size_t)item)) {
            dict->keys[ii] = 1;      // mark bucket as deleted
            dict->nitems--;          // decrease number of items
            dict->n_deleted_items++; // increase number of deleted items
            return 1;                // return success 1
        } else {
            ii = dict->mask & (ii + prime_2); // get index of next bucket
        }
    }
    return 0; // return error if key is not found
}

dict_t *dict_copy(dict_t *dict) {
    dict_t *new_dict = calloc(1, sizeof(struct dict_s));
    if (new_dict == NULL)
        return NULL;

    new_dict->capacity = dict->capacity;
    new_dict->nbits = dict->nbits;
    new_dict->mask = dict->mask;
    new_dict->nitems = dict->nitems;
    new_dict->n_deleted_items = dict->n_deleted_items;

    new_dict->keys = calloc(new_dict->capacity, sizeof(size_t));
    new_dict->values = calloc(new_dict->capacity, sizeof(size_t));
    if (new_dict->keys == NULL || new_dict->values == NULL) {
        if (new_dict->keys != NULL)
            free(new_dict->keys);
        if (new_dict->values != NULL)
            free(new_dict->values);

        free(new_dict);
        return NULL;
    }

    size_t n = new_dict->capacity * sizeof(size_t);
    void *key = memcpy(new_dict->keys, dict->keys, n);
    void *value = memcpy(new_dict->values, dict->values, n);
    ASSERT(key);
    ASSERT(value);
    ASSERT(key == new_dict->keys);
    ASSERT(value == new_dict->values);
    return new_dict;
}

size_t dict_merge(dict_t *dict1, dict_t *dict2) {
    size_t count = 0; // number of merged items

    if (dict1 == NULL || dict2 == NULL || dict1 == dict2)
        return 0;

    size_t ii; // index of the bucket
    for (ii = 0; ii < dict2->capacity; ii++) {
        if (dict2->keys[ii] != 0 && dict2->keys[ii] != 1) {
            int rv = dict_push(dict1, (void *)dict2->keys[ii],
                               (void *)dict2->values[ii]);
            ASSERT(rv == 0 || rv == 1);
            if (rv == 1)
                count++;
        }
    }
    return count;
}

size_t dict_nitems(dict_t *dict) { return dict->nitems; }

dict_itr_t *dict_itr_new(dict_t *dict) {
    dict_itr_t *itr = calloc(1, sizeof(dict_itr_t));
    if (itr == NULL)
        return NULL;
    itr->dict = dict;
    itr->index = 0;
    return itr;
}

int dict_itr_has_next(dict_itr_t *itr) {
    size_t index;

    // either there are no items or we are at the end of the dict
    if (itr->dict->nitems == 0 || itr->index >= itr->dict->capacity)
        return 0;

    index = itr->index;
    while (index < itr->dict->capacity) {
        size_t value = itr->dict->keys[index++];
        if (value != 0)
            return 1;
    }
    return 0;
}

size_t dict_itr_next(dict_itr_t *itr) {
    if (dict_itr_has_next(itr) == 0)
        return -1;

    itr->index++;
    while (itr->index < itr->dict->capacity &&
           (itr->dict->keys[(itr->index)] == 0 ||
            itr->dict->keys[(itr->index)] == 1))
        itr->index++;

    return itr->index;
}

size_t dict_itr_key(dict_itr_t *itr) {
    if (itr->dict->keys[itr->index] == 0 || itr->dict->keys[itr->index] == 1)
        dict_itr_next(itr);

    return itr->dict->keys[itr->index];
}

size_t dict_itr_value(dict_itr_t *itr) {
    if (itr->dict->keys[itr->index] == 0 || itr->dict->keys[itr->index] == 1)
        dict_itr_next(itr);

    return itr->dict->values[itr->index];
}

void dict_itr_free(dict_itr_t *itr) {
    if (itr == NULL)
        return;
    free(itr);
}

void dict_itr_reset(dict_itr_t *itr) { itr->index = 0; }

void *dict_itr_for_each(dict_itr_t *itr, for_each_callback_t *fe1, void *data1,
                        for_each_callback_t *fe2, void *data2) {
    dict_itr_reset(itr); // reset iterator

    if (fe1 == NULL && fe2 == NULL)
        return NULL;

    while (dict_itr_has_next(itr)) {
        void *key = (void *)dict_itr_key(itr);
        void *value = (void *)dict_itr_value(itr);
        ASSERT(key);
        ASSERT(value);

        if (key != NULL && fe1 != NULL) {
            void *p = (fe1)(key, data1);
            if (p != NULL)
                return p;
        }
        if (value != NULL && fe2 != NULL) {
            void *p = (fe2)(value, data2);
            if (p != NULL)
                return p;
        }
        dict_itr_next(itr);
    }
    return NULL;
}

void dict_itr_discard_all(dict_itr_t *itr, delete_callback_t *dc1,
                          delete_callback_t *dc2) {
    dict_itr_reset(itr); // reset iterator

    while (dict_itr_has_next(itr)) {
        void *key = (void *)dict_itr_key(itr);
        void *value = (void *)dict_itr_value(itr);
        ASSERT(key);
        ASSERT(value);

        if (key != NULL) {
            int p = dict_discard(itr->dict, key); // remove key-value pair
            ASSERT(p == 1);                       // yields success
            if (dc1 != NULL)
                (dc1)(key);
            if (value != NULL && dc2 != NULL)
                (dc2)(value);
        }
        dict_itr_next(itr);
    }
}
