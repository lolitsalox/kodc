#include "dict_object.h"
#include "bool_object.h"

 Status dict_get_impl(KodObjectDict* self, KodObject* key, KodObject* kwargs, KodObject** out) {
    //  if (!key->type->hash) {
    //      ERROR_ARGS("Type", "unhashable type : '%s'\n", key->type->tp_name);
    //      RETURN_STATUS_FAIL("Unhashable type");
    //  }

    //  Status s;
    //  size_t index = 0;
    //  if ((s = key->type->hash(key, &index)).type == ST_FAIL) return s;

    //  index %= self->capacity;

    // KodObjectEntry* current = self->table[index];
    // KodObjectBool* is_same = NULL;
    //  while (current != NULL) {
    //      if (!key->type->eq) {
    //          RETURN_STATUS_FAIL("Type has no eq attribute")
    //      }
    //      if ((s = key->type->eq(current->key, key, &AS_OBJECT(is_same))).type == ST_FAIL) return s;
    //      if (is_same->_bool) {
    //          if ((s = kod_object_deref(AS_OBJECT(is_same))).type == ST_FAIL) return s;

    //          if ((s = kod_object_ref(current->value)).type == ST_FAIL) return s;
    //          *out = key;
    //          RETURN_STATUS_OK
    //      }
    //      if ((s = kod_object_deref(AS_OBJECT(is_same))).type == ST_FAIL) return s;
    //      current = current->next;
    //  }

     RETURN_STATUS_FAIL("Unimplemented")
 }

 Status dict_get(KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
     return dict_get_impl((KodObjectDict*)self, args, kwargs, out);
 }

 Status dict_set(KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
     RETURN_STATUS_FAIL("dict_set unimplemented");
 }

 KodObjectSubscriptMethods dict_as_subscript = {
     .get=dict_get,
     .set=dict_set
 };

 KodObjectType KodType_Dict = {
     TYPE_HEADER("dict")
     .as_number=0,
     .as_subscript=&dict_as_subscript,
     .str=0,
     .free=kod_object_free
 };