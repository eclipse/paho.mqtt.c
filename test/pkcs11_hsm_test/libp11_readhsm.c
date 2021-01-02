/* libp11 example code: readhsm.c
 *
 * This examply simply connects to the softhsm via PKCS11 and retrieves the private key.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libp11.h>
#include <arpa/inet.h>

#define RANDOM_SOURCE "/dev/urandom"
#define RANDOM_SIZE 20
#define MAX_SIGSIZE 256
#define SOFTHSM_LIBRARY_PATH "/usr/lib/softhsm/libsofthsm2.so"

int main(int argc, char *argv[])
{
	PKCS11_CTX *ctx;
    PKCS11_SLOT *slots, *slot;
    PKCS11_KEY *keys;
	PKCS11_CERT *certs;

	PKCS11_KEY *authkey;
	PKCS11_CERT *authcert;
	EVP_PKEY *pubkey = NULL;

    /* This is just an example. This should be changed properly. */
    char *password_table[1] = {"abcdefg"};
	unsigned char *random = NULL, *signature = NULL;

	char password[20];
	int rc, fd, logged_in;
	unsigned int nslots, nkeys, ncerts, siglen;

	ctx = PKCS11_CTX_new();

	/* load pkcs #11 module */
	rc = PKCS11_CTX_load(ctx, SOFTHSM_LIBRARY_PATH);
	if (rc) {
		rc = 1;
		goto nolib;
	}

	/* get information on all slots */
	rc = PKCS11_enumerate_slots(ctx, &slots, &nslots);
	if (rc < 0) {
		rc = 2;
		goto noslots;
	}

	slot = PKCS11_find_token(ctx, slots, nslots);
	for (unsigned int i = 1; i <= nslots; i++) {
        if (!slot || !slot->token || !slot->token->label ) {
            goto notoken;
        }
        /* get all certs */
        rc = PKCS11_enumerate_certs(slot->token, &certs, &ncerts);
        if (rc) {
            rc = 13;
            goto failed;
        }
        if (ncerts <= 0) {
            rc = 14;
            goto failed;
        }
        for (unsigned int j = 1; j <= ncerts; j++) {
            PKCS11_CERT *cert = &certs[j-1];
            unsigned short id;
            memcpy(&id, cert->id, 2);
        }

        /* get all public keys */
        rc = PKCS11_enumerate_public_keys(slot->token, &keys, &nkeys);
        if (rc) {
            rc = 13;
            goto failed;
        }
        if (nkeys <= 0) {
            rc = 14;
            goto failed;
        }
        for (unsigned int j = 1; j <= nkeys; j++) {
            PKCS11_KEY *key = &keys[j-1];
            unsigned short id;
            memcpy(&id, key->id, 2);
            EVP_PKEY *evp_key;
            evp_key = PKCS11_get_public_key(key);
        }

        /* perform pkcs #11 login */
        rc = PKCS11_login(slot, 0, password_table[i-1]);
        if (rc) {
            rc = 12;
            goto failed;
        }

        /* get all private keys */
        rc = PKCS11_enumerate_keys(slot->token, &keys, &nkeys);
        if (rc) {
            rc = 13;
            goto failed;
        }
        if (nkeys <= 0) {
            rc = 14;
            goto failed;
        }
        for (unsigned int j = 1; j <= nkeys; j++) {
            PKCS11_KEY *key = &keys[j-1];
            unsigned short id;
            memcpy(&id, key->id, 2);
            EVP_PKEY *evp_key;
            evp_key = PKCS11_get_private_key(key);
        }

        slot = PKCS11_find_next_token(ctx, slots, nslots, slot);
    }

	/* use the first cert */
	authcert=&certs[0];

	/* get random bytes */
	random = OPENSSL_malloc(RANDOM_SIZE);
	if (random == NULL) {
		rc = 15;
		goto failed;
	}

	fd = open(RANDOM_SOURCE, O_RDONLY);
	if (fd < 0) {
		rc = 16;
		goto failed;
	}

	rc = read(fd, random, RANDOM_SIZE);
	if (rc < 0) {
		close(fd);
		rc = 17;
		goto failed;
	}

	if (rc < RANDOM_SIZE) {
		close(fd);
		rc = 18;
		goto failed;
	}

	close(fd);

	authkey = PKCS11_find_key(authcert);
	if (authkey == NULL) {
		rc = 19;
		goto failed;
	}

	/* ask for a sha1 hash of the random data, signed by the key */
	siglen = MAX_SIGSIZE;
	signature = OPENSSL_malloc(MAX_SIGSIZE);
	if (signature == NULL) {
		rc = 20;
		goto failed;
	}

	rc = PKCS11_sign(NID_sha1, random, RANDOM_SIZE,
		signature, &siglen, authkey);
	if (rc != 1) {
		rc = 21;
		goto failed;
	}

	/* verify the signature */
	pubkey = X509_get_pubkey(authcert->x509);
	if (pubkey == NULL) {
		rc = 22;
		goto failed;
	}

	/* now verify the result */
	rc = RSA_verify(NID_sha1, random, RANDOM_SIZE,
#if OPENSSL_VERSION_NUMBER >= 0x10100003L && !defined(LIBRESSL_VERSION_NUMBER)
			signature, siglen, EVP_PKEY_get0_RSA(pubkey));
#else
			signature, siglen, pubkey->pkey.rsa);
#endif
	if (rc != 1) {
		rc = 23;
		goto failed;
	}

	rc = 0;

failed:
	if (rc)
		ERR_print_errors_fp(stderr);
	if (random != NULL)
		OPENSSL_free(random);
	if (pubkey != NULL)
		EVP_PKEY_free(pubkey);
	if (signature != NULL)
		OPENSSL_free(signature);

notoken:
	PKCS11_release_all_slots(ctx, slots, nslots);

noslots:
	PKCS11_CTX_unload(ctx);

nolib:
	PKCS11_CTX_free(ctx);
	return rc;
}

/* vim: set noexpandtab: */
