#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/decoder.h>
#include <openssl/bn.h>

/*
    A: Write a subroutine BIGNUM *good(BIGNUM *input) that 
    asks the user for the name of key file
    opens that file
    extracts d and n
    closes the file
    and returns input^d mod n.
    
    B. Now write a subroutine evil() (you may add arguments and such if you wish) that, when invoked as follows, prints out the value of the private key d.
    The coder of evil() may know the source code of good(), but may NOT know the name of the keyfile:
 */

// no globals
BIGNUM *good(BIGNUM *input) {
    char keyfile[256];
    printf("Enter name of key pem file: ");
    scanf("%s", keyfile);
    FILE *pemfile;
    if (!(pemfile = fopen(keyfile, "r"))) {
        printf("failed to open key file\n");
        return NULL;
    }

    OSSL_DECODER_CTX *dctx = NULL;
    EVP_PKEY *pkey = NULL;
    const char *format = "PEM";
    const char *structure = NULL;
    const char *keytype = "RSA";
    BIGNUM *d = NULL, *n = NULL;
    BN_CTX *ctx = BN_CTX_new();

    // construct the decoder obj
    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, format, structure, keytype, OSSL_KEYMGMT_SELECT_KEYPAIR, NULL, NULL);
    if (dctx == NULL) {
        printf("can't create decoder\n");
        return NULL;
    }
    // read the key pair from the pem file
    if (!OSSL_DECODER_from_fp(dctx, pemfile)) {
        printf("decoder can't read keypair\n");
        return NULL;
    }
    // get n
    if (!EVP_PKEY_get_bn_param(pkey, "n", &n)) {
        printf("can't get n\n");
        return NULL;
    }
    printf("modulus n:\n");
    BN_print_fp(stdout, n);
    // get d
    if (!EVP_PKEY_get_bn_param(pkey, "d", &d)) {
        printf("can't get d\n");
        return NULL;
    }
    // close file
    fclose(pemfile);
    BIGNUM *res = BN_new();
    BN_mod_exp(res, input, d, n, ctx);
    // clean up
    EVP_PKEY_free(pkey);
    OSSL_DECODER_CTX_free(dctx);
    BN_CTX_free(ctx);
    printf("\nprivate key in hex (will be the same with evil() print out:)\n");
    BN_print_fp(stdout, d);
    printf("\n");
    char *d_str = BN_bn2dec(d);
    printf("Decimal private key: %s\n", d_str);
    BIGNUM * d2 = BN_new();
    BN_copy(d2, d);
    //BN_free(d);   // can't be freed if we want to access the memory again (to steal the private key)
    //BN_free(n);
    return res;
}
    
void evil() {
    void *key = BN_new();
    // memory addresses obtained from gdb, 
    // first from private key pointer, second from the new BN address created under the variable name `key`
    long int pklong = 0x5555555aa1c0;
    long int pKey = 0x5555555a8330;
    long int diff = pKey-pklong;
    
    printf("Byte difference: %ld\n", diff);
    key = key-diff;
    
    printf("\n*** Extract private key from memory (print in dex not decimal) ***\n");
    BN_print_fp(stdout, key);
    printf("\n");
    return;
}

int main( ) {
    // no other locals in main
    BIGNUM *output;
    // set up input to be some integer smaller than your modulus
    char input_str[] = "4294967296000000";
    BIGNUM *input = BN_new();
    BN_dec2bn(&input, input_str);
    printf("number to decrypt...\n");
    BN_print_fp(stdout, input);
    printf("\n");
    
    output = good(input);
    printf("\ngood() output:\n");
    BN_print_fp(stdout, output);
    printf("\n");
    
    printf("running evil()...\n");
    evil();
    BN_free(input);
    BN_free(output);
}
