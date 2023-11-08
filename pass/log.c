#include <stdio.h>

void pass_funcStartLogger(char* funcName) { printf("[LOG] Start function '%s'\n", funcName); }

void pass_callLogger(char* calleeName, char* callerName) {
    printf("[LOG] CALL '%s' -> '%s'\n", calleeName, callerName);
}

void pass_funcEndLogger(char* funcName) { printf("[LOG] End function '%s'\n", funcName); }

void pass_binOptLogger(char* opName, char* funcName) {
    printf("[LOG] In function '%s': '%s' occured\n", funcName, opName);
}
