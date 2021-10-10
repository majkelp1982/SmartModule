#include <Utils/Utils.h>

bool empty(String text) {
    if ((text == NULL) || (text == "")) {
       return true;
    }
   else
   return false;
}

String getString(const char* value) {
    String result = value;
    return result;
}
