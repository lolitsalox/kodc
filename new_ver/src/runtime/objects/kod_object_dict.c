// #include "kod_object_dict.h"

// Status dict_get_impl(KodObjectDict* self, KodObject* key, KodObject* kwargs, KodObject** out) {
//     if (!key->type->hash) {
//         printf("TypeError: unhashable type: '%s'\n", key->type->tp_name);
//         RETURN_STATUS_FAIL("Unhashable type");
//     }

//     size_t index = key->type->hash(key) % self->_dict.size;
//     DictItem* current = self->_dict.table[index];
//     while (current != NULL) {
//         if (current->key == key) {
//             ref_object(key);
//             return key;
//         }
//         current = current->next;
//     }

//     RETURN_STATUS_FAIL("Unimplemented")
// }

// Status dict_get(KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
//     return dict_get_impl((KodObjectDict*)self, tuple, kwargs, out);
// }

// KodObjectSubscriptMethods dict_as_subscript = {
//     .get=dict_get,
//     .set=dict_set
// };

// KodObjectType KodType_Dict = {
//     TYPE_HEADER("dict")
//     .as_number=0,
//     .as_subscript=&dict_as_subscript,
//     .str=0,
//     .free=kod_object_free
// };