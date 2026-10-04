#include "../../port/amiga/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int check(const void *data,size_t size,const char *expected) {
    char hash[65]; amiga_sha256_hex(data,size,hash);
    if (strcmp(hash,expected)) { fprintf(stderr,"SHA-256 mismatch at %zu bytes: %s\n",size,hash); return 0; }
    return 1;
}
int main(void) {
    if (!check(NULL,0,"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") ||
        !check("abc",3,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") ||
        !check("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",56,
               "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")) return 1;
    char *data=malloc(1000000); if (!data) return 1; memset(data,'a',1000000);
    int ok=check(data,1000000,"cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
    free(data); return ok?0:1;
}
