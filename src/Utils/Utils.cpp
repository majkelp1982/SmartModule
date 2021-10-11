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

  bool blink1Hz() {
    unsigned long currentS = (unsigned long)(millis()/1000);
    if ((currentS%2) == 0)
    return true;
    return false;
}
