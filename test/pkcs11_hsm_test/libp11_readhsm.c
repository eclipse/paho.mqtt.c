/*
 * Copyright © 2020, Andreas Jellinghaus <andreas@ionisiert.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* libp11 example code: readhsm.c
 *
 * This examply simply connects to the softhsm
 * and retrieves the private key.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libp11.h>

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
    char *password_table[3] = {"yksecret1", "yksecret2", "yksecret3"};


	unsigned char *random = NULL, *signature = NULL;

	char password[20];
	int rc, fd, logged_in;
	unsigned int nslots, nkeys, ncerts, siglen;

	ctx = PKCS11_CTX_new();

	/* load pkcs #11 module */
	rc = PKCS11_CTX_load(ctx, SOFTHSM_LIBRARY_PATH);
	if (rc) {
		fprintf(stderr, "loading pkcs11 engine failed: %s\n",
			ERR_reason_error_string(ERR_get_error()));
		rc = 1;
		goto nolib;
	}

	/* get information on all slots */
	rc = PKCS11_enumerate_slots(ctx, &slots, &nslots);
	if (rc < 0) {
		fprintf(stderr, "no slots available\n");
		rc = 2;
		goto noslots;
	}

    printf("%d number of slots are found!\n", nslots);

	slot = PKCS11_find_token(ctx, slots, nslots);
    for (unsigned int i = 1; i <= nslots; i++) {
        if (slot == NULL || slot->token == NULL || slot->token->label[0] == NULL) {
            fprintf(stderr, "no token!\n");
            goto notoken;
        }
	    printf("#%u token: label(%s), serial(%s), login-required(0x%x)\n", \
            i, slot->token->label, slot->token->serialnr, slot->token->loginRequired);

        /* get all certs */
        rc = PKCS11_enumerate_certs(slot->token, &certs, &ncerts);
        if (rc) {
            fprintf(stderr, "PKCS11_enumerate_certs failed\n");
            rc = 13;
            goto failed;
        }
        if (ncerts <= 0) {
            fprintf(stderr, "no certificates found\n");
            rc = 14;
            goto failed;
        }
        for (unsigned int j = 1; j <= ncerts; j++) {
            PKCS11_CERT *cert = &certs[j-1];
            unsigned short id;
            memcpy(&id, cert->id, 2);
            printf("  #%u certificate: label(%s), id(0x%x), x509(%p)\n", \
            j, cert->label, ntohs(id), cert->x509);
        }

        /* get all public keys */
        rc = PKCS11_enumerate_public_keys(slot->token, &keys, &nkeys);
        if (rc) {
            fprintf(stderr, "PKCS11_enumerate_public_keys failed\n");
            rc = 13;
            goto failed;
        }
        if (nkeys <= 0) {
            fprintf(stderr, "no public key found\n");
            rc = 14;
            goto failed;
        }
        for (unsigned int j = 1; j <= nkeys; j++) {
            PKCS11_KEY *key = &keys[j-1];
            unsigned short id;
            memcpy(&id, key->id, 2);
            EVP_PKEY *evp_key;
            evp_key = PKCS11_get_public_key(key);
            printf("  #%u public key: label(%s), id(0x%x), login(%d), evp_key(%p)\n", \
            j, key->label, ntohs(id), key->needLogin, key->evp_key);
        }

        /* perform pkcs #11 login */
        rc = PKCS11_login(slot, 0, password_table[i-1]);
        if (rc) {
            fprintf(stderr, "PKCS11_login failed\n");
            rc = 12;
            goto failed;
        }

        /* get all private keys */
        rc = PKCS11_enumerate_keys(slot->token, &keys, &nkeys);
        if (rc) {
            fprintf(stderr, "PKCS11_enumerate_keys failed\n");
            rc = 13;
            goto failed;
        }
        if (nkeys <= 0) {
            fprintf(stderr, "no private key found\n");
            rc = 14;
            goto failed;
        }
        for (unsigned int j = 1; j <= nkeys; j++) {
            PKCS11_KEY *key = &keys[j-1];
            unsigned short id;
            memcpy(&id, key->id, 2);
            EVP_PKEY *evp_key;
            evp_key = PKCS11_get_private_key(key);
            printf("  #%u private key: label(%s), id(0x%x), login(%d), evp_key(%p)\n", \
            j, key->label, ntohs(id), key->needLogin, key->evp_key);
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
		fprintf(stderr, "fatal: cannot open RANDOM_SOURCE: %s\n",
				strerror(errno));
		rc = 16;
		goto failed;
	}

	rc = read(fd, random, RANDOM_SIZE);
	if (rc < 0) {
		fprintf(stderr, "fatal: read from random source failed: %s\n",
			strerror(errno));
		close(fd);
		rc = 17;
		goto failed;
	}

	if (rc < RANDOM_SIZE) {
		fprintf(stderr, "fatal: read returned less than %d<%d bytes\n",
			rc, RANDOM_SIZE);
		close(fd);
		rc = 18;
		goto failed;
	}

	close(fd);

	authkey = PKCS11_find_key(authcert);
	if (authkey == NULL) {
		fprintf(stderr, "no key matching certificate available\n");
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
		fprintf(stderr, "fatal: pkcs11_sign failed\n");
		rc = 21;
		goto failed;
	}

	/* verify the signature */
	pubkey = X509_get_pubkey(authcert->x509);
	if (pubkey == NULL) {
		fprintf(stderr, "could not extract public key\n");
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
		fprintf(stderr, "fatal: RSA_verify failed\n");
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

	if (rc)
		printf("authentication failed.\n");
	else
		printf("authentication successfull.\n");
	return rc;
}

/* vim: set noexpandtab: */
