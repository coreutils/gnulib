/* des.c --- DES and Triple-DES encryption/decryption Algorithm
 * Copyright (C) 1998-1999, 2001-2007, 2009-2022 Free Software Foundation, Inc.
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/* Adapted for gnulib by Simon Josefsson, based on Libgcrypt. */

/*
 * For a description of triple encryption, see:
 *   Bruce Schneier: Applied Cryptography. Second Edition.
 *   John Wiley & Sons, 1996. ISBN 0-471-12845-7. Pages 358 ff.
 * This implementation is according to the definition of DES in FIPS
 * PUB 46-2 from December 1993.
 *
 * Written by Michael Roth <mroth@nessie.de>, September 1998
 */

/*
 *  U S A G E
 * ===========
 *
 * For DES or Triple-DES encryption/decryption you must initialize a proper
 * encryption context with a key.
 *
 * A DES key is 64bit wide but only 56bits of the key are used. The remaining
 * bits are parity bits and they will _not_ checked in this implementation, but
 * simply ignored.
 *
 * For Triple-DES you could use either two 64bit keys or three 64bit keys.
 * The parity bits will _not_ checked, too.
 *
 * After initializing a context with a key you could use this context to
 * encrypt or decrypt data in 64bit blocks in Electronic Codebook Mode.
 *
 * DES Example
 * -----------
 *     unsigned char key[8];
 *     unsigned char plaintext[8];
 *     unsigned char ciphertext[8];
 *     unsigned char recoverd[8];
 *     gl_des_ctx context;
 *
 *     // Fill 'key' and 'plaintext' with some data
 *     ....
 *
 *     // Set up the DES encryption context
 *     gl_des_setkey(&context, key);
 *
 *     // Encrypt the plaintext
 *     des_ecb_encrypt(&context, plaintext, ciphertext);
 *
 *     // To recover the original plaintext from ciphertext use:
 *     des_ecb_decrypt(&context, ciphertext, recoverd);
 *
 *
 * Triple-DES Example
 * ------------------
 *     unsigned char key1[8];
 *     unsigned char key2[8];
 *     unsigned char key3[8];
 *     unsigned char plaintext[8];
 *     unsigned char ciphertext[8];
 *     unsigned char recoverd[8];
 *     gl_3des_ctx context;
 *
 *     // If you would like to use two 64bit keys, fill 'key1' and'key2'
 *     // then setup the encryption context:
 *     gl_3des_set2keys(&context, key1, key2);
 *
 *     // To use three 64bit keys with Triple-DES use:
 *     gl_3des_set3keys(&context, key1, key2, key3);
 *
 *     // Encrypting plaintext with Triple-DES
 *     gl_3des_ecb_encrypt(&context, plaintext, ciphertext);
 *
 *     // Decrypting ciphertext to recover the plaintext with Triple-DES
 *     gl_3des_ecb_decrypt(&context, ciphertext, recoverd);
 */


#include <config.h>

#include "des.h"

#include <stdio.h>
#include <string.h>             /* memcpy, memcmp */

/*
 * The s-box values are permuted according to the 'primitive function P'
 * and are rotated one bit to the left.
 */
static const uint32_t sbox1[64] = {
  0x01010400, 0x00000000, 0x00010000, 0x01010404, 0x01010004, 0x00010404,
  0x00000004, 0x00010000, 0x00000400, 0x01010400, 0x01010404, 0x00000400,
  0x01000404, 0x01010004, 0x01000000, 0x00000004, 0x00000404, 0x01000400,
  0x01000400, 0x00010400, 0x00010400, 0x01010000, 0x01010000, 0x01000404,
  0x00010004, 0x01000004, 0x01000004, 0x00010004, 0x00000000, 0x00000404,
  0x00010404, 0x01000000, 0x00010000, 0x01010404, 0x00000004, 0x01010000,
  0x01010400, 0x01000000, 0x01000000, 0x00000400, 0x01010004, 0x00010000,
  0x00010400, 0x01000004, 0x00000400, 0x00000004, 0x01000404, 0x00010404,
  0x01010404, 0x00010004, 0x01010000, 0x01000404, 0x01000004, 0x00000404,
  0x00010404, 0x01010400, 0x00000404, 0x01000400, 0x01000400, 0x00000000,
  0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const uint32_t sbox2[64] = {
  0x80108020, 0x80008000, 0x00008000, 0x00108020, 0x00100000, 0x00000020,
  0x80100020, 0x80008020, 0x80000020, 0x80108020, 0x80108000, 0x80000000,
  0x80008000, 0x00100000, 0x00000020, 0x80100020, 0x00108000, 0x00100020,
  0x80008020, 0x00000000, 0x80000000, 0x00008000, 0x00108020, 0x80100000,
  0x00100020, 0x80000020, 0x00000000, 0x00108000, 0x00008020, 0x80108000,
  0x80100000, 0x00008020, 0x00000000, 0x00108020, 0x80100020, 0x00100000,
  0x80008020, 0x80100000, 0x80108000, 0x00008000, 0x80100000, 0x80008000,
  0x00000020, 0x80108020, 0x00108020, 0x00000020, 0x00008000, 0x80000000,
  0x00008020, 0x80108000, 0x00100000, 0x80000020, 0x00100020, 0x80008020,
  0x80000020, 0x00100020, 0x00108000, 0x00000000, 0x80008000, 0x00008020,
  0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const uint32_t sbox3[64] = {
  0x00000208, 0x08020200, 0x00000000, 0x08020008, 0x08000200, 0x00000000,
  0x00020208, 0x08000200, 0x00020008, 0x08000008, 0x08000008, 0x00020000,
  0x08020208, 0x00020008, 0x08020000, 0x00000208, 0x08000000, 0x00000008,
  0x08020200, 0x00000200, 0x00020200, 0x08020000, 0x08020008, 0x00020208,
  0x08000208, 0x00020200, 0x00020000, 0x08000208, 0x00000008, 0x08020208,
  0x00000200, 0x08000000, 0x08020200, 0x08000000, 0x00020008, 0x00000208,
  0x00020000, 0x08020200, 0x08000200, 0x00000000, 0x00000200, 0x00020008,
  0x08020208, 0x08000200, 0x08000008, 0x00000200, 0x00000000, 0x08020008,
  0x08000208, 0x00020000, 0x08000000, 0x08020208, 0x00000008, 0x00020208,
  0x00020200, 0x08000008, 0x08020000, 0x08000208, 0x00000208, 0x08020000,
  0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const uint32_t sbox4[64] = {
  0x00802001, 0x00002081, 0x00002081, 0x00000080, 0x00802080, 0x00800081,
  0x00800001, 0x00002001, 0x00000000, 0x00802000, 0x00802000, 0x00802081,
  0x00000081, 0x00000000, 0x00800080, 0x00800001, 0x00000001, 0x00002000,
  0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002001, 0x00002080,
  0x00800081, 0x00000001, 0x00002080, 0x00800080, 0x00002000, 0x00802080,
  0x00802081, 0x00000081, 0x00800080, 0x00800001, 0x00802000, 0x00802081,
  0x00000081, 0x00000000, 0x00000000, 0x00802000, 0x00002080, 0x00800080,
  0x00800081, 0x00000001, 0x00802001, 0x00002081, 0x00002081, 0x00000080,
  0x00802081, 0x00000081, 0x00000001, 0x00002000, 0x00800001, 0x00002001,
  0x00802080, 0x00800081, 0x00002001, 0x00002080, 0x00800000, 0x00802001,
  0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const uint32_t sbox5[64] = {
  0x00000100, 0x02080100, 0x02080000, 0x42000100, 0x00080000, 0x00000100,
  0x40000000, 0x02080000, 0x40080100, 0x00080000, 0x02000100, 0x40080100,
  0x42000100, 0x42080000, 0x00080100, 0x40000000, 0x02000000, 0x40080000,
  0x40080000, 0x00000000, 0x40000100, 0x42080100, 0x42080100, 0x02000100,
  0x42080000, 0x40000100, 0x00000000, 0x42000000, 0x02080100, 0x02000000,
  0x42000000, 0x00080100, 0x00080000, 0x42000100, 0x00000100, 0x02000000,
  0x40000000, 0x02080000, 0x42000100, 0x40080100, 0x02000100, 0x40000000,
  0x42080000, 0x02080100, 0x40080100, 0x00000100, 0x02000000, 0x42080000,
  0x42080100, 0x00080100, 0x42000000, 0x42080100, 0x02080000, 0x00000000,
  0x40080000, 0x42000000, 0x00080100, 0x02000100, 0x40000100, 0x00080000,
  0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const uint32_t sbox6[64] = {
  0x20000010, 0x20400000, 0x00004000, 0x20404010, 0x20400000, 0x00000010,
  0x20404010, 0x00400000, 0x20004000, 0x00404010, 0x00400000, 0x20000010,
  0x00400010, 0x20004000, 0x20000000, 0x00004010, 0x00000000, 0x00400010,
  0x20004010, 0x00004000, 0x00404000, 0x20004010, 0x00000010, 0x20400010,
  0x20400010, 0x00000000, 0x00404010, 0x20404000, 0x00004010, 0x00404000,
  0x20404000, 0x20000000, 0x20004000, 0x00000010, 0x20400010, 0x00404000,
  0x20404010, 0x00400000, 0x00004010, 0x20000010, 0x00400000, 0x20004000,
  0x20000000, 0x00004010, 0x20000010, 0x20404010, 0x00404000, 0x20400000,
  0x00404010, 0x20404000, 0x00000000, 0x20400010, 0x00000010, 0x00004000,
  0x20400000, 0x00404010, 0x00004000, 0x00400010, 0x20004010, 0x00000000,
  0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const uint32_t sbox7[64] = {
  0x00200000, 0x04200002, 0x04000802, 0x00000000, 0x00000800, 0x04000802,
  0x00200802, 0x04200800, 0x04200802, 0x00200000, 0x00000000, 0x04000002,
  0x00000002, 0x04000000, 0x04200002, 0x00000802, 0x04000800, 0x00200802,
  0x00200002, 0x04000800, 0x04000002, 0x04200000, 0x04200800, 0x00200002,
  0x04200000, 0x00000800, 0x00000802, 0x04200802, 0x00200800, 0x00000002,
  0x04000000, 0x00200800, 0x04000000, 0x00200800, 0x00200000, 0x04000802,
  0x04000802, 0x04200002, 0x04200002, 0x00000002, 0x00200002, 0x04000000,
  0x04000800, 0x00200000, 0x04200800, 0x00000802, 0x00200802, 0x04200800,
  0x00000802, 0x04000002, 0x04200802, 0x04200000, 0x00200800, 0x00000000,
  0x00000002, 0x04200802, 0x00000000, 0x00200802, 0x04200000, 0x00000800,
  0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const uint32_t sbox8[64] = {
  0x10001040, 0x00001000, 0x00040000, 0x10041040, 0x10000000, 0x10001040,
  0x00000040, 0x10000000, 0x00040040, 0x10040000, 0x10041040, 0x00041000,
  0x10041000, 0x00041040, 0x00001000, 0x00000040, 0x10040000, 0x10000040,
  0x10001000, 0x00001040, 0x00041000, 0x00040040, 0x10040040, 0x10041000,
  0x00001040, 0x00000000, 0x00000000, 0x10040040, 0x10000040, 0x10001000,
  0x00041040, 0x00040000, 0x00041040, 0x00040000, 0x10041000, 0x00001000,
  0x00000040, 0x10040040, 0x00001000, 0x00041040, 0x10001000, 0x00000040,
  0x10000040, 0x10040000, 0x10040040, 0x10000000, 0x00040000, 0x10001040,
  0x00000000, 0x10041040, 0x00040040, 0x10000040, 0x10040000, 0x10001000,
  0x10001040, 0x00000000, 0x10041040, 0x00041000, 0x00041000, 0x00001040,
  0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * These two tables are part of the 'permuted choice 1' function.
 * In this implementation several speed improvements are done.
 */
static const uint32_t leftkey_swap[16] = {
  0x00000000, 0x00000001, 0x00000100, 0x00000101,
  0x00010000, 0x00010001, 0x00010100, 0x00010101,
  0x01000000, 0x01000001, 0x01000100, 0x01000101,
  0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const uint32_t rightkey_swap[16] = {
  0x00000000, 0x01000000, 0x00010000, 0x01010000,
  0x00000100, 0x01000100, 0x00010100, 0x01010100,
  0x00000001, 0x01000001, 0x00010001, 0x01010001,
  0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Numbers of left shifts per round for encryption subkeys.  To
 * calculate the decryption subkeys we just reverse the ordering of
 * the calculated encryption subkeys, so there is no need for a
 * decryption rotate tab.
 */
static const unsigned char encrypt_rotate_tab[16] = {
  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/*
 * Table with weak DES keys sorted in ascending order.  In DES there
 * are 64 known keys which are weak. They are weak because they
 * produce only one, two or four different subkeys in the subkey
 * scheduling process.  The keys in this table have all their parity
 * bits cleared.
 */
static const unsigned char weak_keys[64][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},     /*w */
  {0x00, 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e},
  {0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0},
  {0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe},
  {0x00, 0x1e, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x0e},     /*sw */
  {0x00, 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e, 0x00},
  {0x00, 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0, 0xfe},
  {0x00, 0x1e, 0xfe, 0xe0, 0x00, 0x0e, 0xfe, 0xf0},
  {0x00, 0xe0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xf0},     /*sw */
  {0x00, 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e, 0xfe},
  {0x00, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0, 0x00},
  {0x00, 0xe0, 0xfe, 0x1e, 0x00, 0xf0, 0xfe, 0x0e},
  {0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe},     /*sw */
  {0x00, 0xfe, 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0},
  {0x00, 0xfe, 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e},
  {0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00},
  {0x1e, 0x00, 0x00, 0x1e, 0x0e, 0x00, 0x00, 0x0e},
  {0x1e, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x0e, 0x00},     /*sw */
  {0x1e, 0x00, 0xe0, 0xfe, 0x0e, 0x00, 0xf0, 0xfe},
  {0x1e, 0x00, 0xfe, 0xe0, 0x0e, 0x00, 0xfe, 0xf0},
  {0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x00},
  {0x1e, 0x1e, 0x1e, 0x1e, 0x0e, 0x0e, 0x0e, 0x0e},     /*w */
  {0x1e, 0x1e, 0xe0, 0xe0, 0x0e, 0x0e, 0xf0, 0xf0},
  {0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e, 0xfe, 0xfe},
  {0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0, 0x00, 0xfe},
  {0x1e, 0xe0, 0x1e, 0xe0, 0x0e, 0xf0, 0x0e, 0xf0},     /*sw */
  {0x1e, 0xe0, 0xe0, 0x1e, 0x0e, 0xf0, 0xf0, 0x0e},
  {0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0, 0xfe, 0x00},
  {0x1e, 0xfe, 0x00, 0xe0, 0x0e, 0xfe, 0x00, 0xf0},
  {0x1e, 0xfe, 0x1e, 0xfe, 0x0e, 0xfe, 0x0e, 0xfe},     /*sw */
  {0x1e, 0xfe, 0xe0, 0x00, 0x0e, 0xfe, 0xf0, 0x00},
  {0x1e, 0xfe, 0xfe, 0x1e, 0x0e, 0xfe, 0xfe, 0x0e},
  {0xe0, 0x00, 0x00, 0xe0, 0xf0, 0x00, 0x00, 0xf0},
  {0xe0, 0x00, 0x1e, 0xfe, 0xf0, 0x00, 0x0e, 0xfe},
  {0xe0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xf0, 0x00},     /*sw */
  {0xe0, 0x00, 0xfe, 0x1e, 0xf0, 0x00, 0xfe, 0x0e},
  {0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e, 0x00, 0xfe},
  {0xe0, 0x1e, 0x1e, 0xe0, 0xf0, 0x0e, 0x0e, 0xf0},
  {0xe0, 0x1e, 0xe0, 0x1e, 0xf0, 0x0e, 0xf0, 0x0e},     /*sw */
  {0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e, 0xfe, 0x00},
  {0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00},
  {0xe0, 0xe0, 0x1e, 0x1e, 0xf0, 0xf0, 0x0e, 0x0e},
  {0xe0, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0},     /*w */
  {0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0, 0xfe, 0xfe},
  {0xe0, 0xfe, 0x00, 0x1e, 0xf0, 0xfe, 0x00, 0x0e},
  {0xe0, 0xfe, 0x1e, 0x00, 0xf0, 0xfe, 0x0e, 0x00},
  {0xe0, 0xfe, 0xe0, 0xfe, 0xf0, 0xfe, 0xf0, 0xfe},     /*sw */
  {0xe0, 0xfe, 0xfe, 0xe0, 0xf0, 0xfe, 0xfe, 0xf0},
  {0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe},
  {0xfe, 0x00, 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0},
  {0xfe, 0x00, 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e},
  {0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00},     /*sw */
  {0xfe, 0x1e, 0x00, 0xe0, 0xfe, 0x0e, 0x00, 0xf0},
  {0xfe, 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e, 0xfe},
  {0xfe, 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0, 0x00},
  {0xfe, 0x1e, 0xfe, 0x1e, 0xfe, 0x0e, 0xfe, 0x0e},     /*sw */
  {0xfe, 0xe0, 0x00, 0x1e, 0xfe, 0xf0, 0x00, 0x0e},
  {0xfe, 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e, 0x00},
  {0xfe, 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0, 0xfe},
  {0xfe, 0xe0, 0xfe, 0xe0, 0xfe, 0xf0, 0xfe, 0xf0},     /*sw */
  {0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00},
  {0xfe, 0xfe, 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e},
  {0xfe, 0xfe, 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0},
  {0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe}      /*w */
};

bool
gl_des_is_weak_key (const char * key)
{
  char work[8];
  int i, left, right, middle, cmp_result;

  /* clear parity bits */
  for (i = 0; i < 8; ++i)
    work[i] = ((unsigned char)key[i]) & 0xfe;

  /* binary search in the weak key table */
  left = 0;
  right = 63;
  while (left <= right)
    {
      middle = (left + right) / 2;

      if (!(cmp_result = memcmp (work, weak_keys[middle], 8)))
        return -1;

      if (cmp_result > 0)
        left = middle + 1;
      else
        right = middle - 1;
    }

  return 0;
}

/*
 * Macro to swap bits across two words.
 */
#define DO_PERMUTATION(a, temp, b, offset, mask)        \
    temp = ((a>>offset) ^ b) & mask;                    \
    b ^= temp;                                          \
    a ^= temp<<offset;


/*
 * This performs the 'initial permutation' of the data to be encrypted
 * or decrypted. Additionally the resulting two words are rotated one bit
 * to the left.
 */
#define INITIAL_PERMUTATION(left, temp, right)          \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)    \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)   \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333)    \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)    \
    right =  (right << 1) | (right >> 31);              \
    temp  =  (left ^ right) & 0xaaaaaaaa;               \
    right ^= temp;                                      \
    left  ^= temp;                                      \
    left  =  (left << 1) | (left >> 31);

/*
 * The 'inverse initial permutation'.
 */
#define FINAL_PERMUTATION(left, temp, right)            \
    left  =  (left << 31) | (left >> 1);                \
    temp  =  (left ^ right) & 0xaaaaaaaa;               \
    left  ^= temp;                                      \
    right ^= temp;                                      \
    right  =  (right << 31) | (right >> 1);             \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)    \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333)    \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff)   \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)


/*
 * A full DES round including 'expansion function', 'sbox substitution'
 * and 'primitive function P' but without swapping the left and right word.
 * Please note: The data in 'from' and 'to' is already rotated one bit to
 * the left, done in the initial permutation.
 */
#define DES_ROUND(from, to, work, subkey)               \
    work = from ^ *subkey++;                            \
    to ^= sbox8[  work      & 0x3f ];                   \
    to ^= sbox6[ (work>>8)  & 0x3f ];                   \
    to ^= sbox4[ (work>>16) & 0x3f ];                   \
    to ^= sbox2[ (work>>24) & 0x3f ];                   \
    work = ((from << 28) | (from >> 4)) ^ *subkey++;    \
    to ^= sbox7[  work      & 0x3f ];                   \
    to ^= sbox5[ (work>>8)  & 0x3f ];                   \
    to ^= sbox3[ (work>>16) & 0x3f ];                   \
    to ^= sbox1[ (work>>24) & 0x3f ];

/*
 * Macros to convert 8 bytes from/to 32bit words.
 */
#define READ_64BIT_DATA(data, left, right)                                 \
    left  = ((uint32_t) data[0] << 24)                                     \
            | ((uint32_t) data[1] << 16)                                   \
            | ((uint32_t) data[2] << 8)                                    \
            | (uint32_t) data[3];                                          \
    right = ((uint32_t) data[4] << 24)                                     \
            | ((uint32_t) data[5] << 16)                                   \
            | ((uint32_t) data[6] << 8)                                    \
            | (uint32_t) data[7];

#define WRITE_64BIT_DATA(data, left, right)                                \
    data[0] = (left >> 24) &0xff; data[1] = (left >> 16) &0xff;            \
    data[2] = (left >> 8) &0xff; data[3] = left &0xff;                     \
    data[4] = (right >> 24) &0xff; data[5] = (right >> 16) &0xff;          \
    data[6] = (right >> 8) &0xff; data[7] = right &0xff;

/*
 * des_key_schedule():    Calculate 16 subkeys pairs (even/odd) for
 *                        16 encryption rounds.
 *                        To calculate subkeys for decryption the caller
 *                        have to reorder the generated subkeys.
 *
 *    rawkey:       8 Bytes of key data
 *    subkey:       Array of at least 32 uint32_ts. Will be filled
 *                  with calculated subkeys.
 *
 */
static void
des_key_schedule (const char * _rawkey, uint32_t * subkey)
{
  const unsigned char *rawkey = (const unsigned char *) _rawkey;
  uint32_t left, right, work;
  int round;

  READ_64BIT_DATA (rawkey, left, right)
    DO_PERMUTATION (right, work, left, 4, 0x0f0f0f0f)
    DO_PERMUTATION (right, work, left, 0, 0x10101010)
    left = ((leftkey_swap[(left >> 0) & 0xf] << 3)
            | (leftkey_swap[(left >> 8) & 0xf] << 2)
            | (leftkey_swap[(left >> 16) & 0xf] << 1)
            | (leftkey_swap[(left >> 24) & 0xf])
            | (leftkey_swap[(left >> 5) & 0xf] << 7)
            | (leftkey_swap[(left >> 13) & 0xf] << 6)
            | (leftkey_swap[(left >> 21) & 0xf] << 5)
            | (leftkey_swap[(left >> 29) & 0xf] << 4));

  left &= 0x0fffffff;

  right = ((rightkey_swap[(right >> 1) & 0xf] << 3)
           | (rightkey_swap[(right >> 9) & 0xf] << 2)
           | (rightkey_swap[(right >> 17) & 0xf] << 1)
           | (rightkey_swap[(right >> 25) & 0xf])
           | (rightkey_swap[(right >> 4) & 0xf] << 7)
           | (rightkey_swap[(right >> 12) & 0xf] << 6)
           | (rightkey_swap[(right >> 20) & 0xf] << 5)
           | (rightkey_swap[(right >> 28) & 0xf] << 4));

  right &= 0x0fffffff;

  for (round = 0; round < 16; ++round)
    {
      left = ((left << encrypt_rotate_tab[round])
              | (left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
      right = ((right << encrypt_rotate_tab[round])
               | (right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

      *subkey++ = (((left << 4) & 0x24000000)
                   | ((left << 28) & 0x10000000)
                   | ((left << 14) & 0x08000000)
                   | ((left << 18) & 0x02080000)
                   | ((left << 6) & 0x01000000)
                   | ((left << 9) & 0x00200000)
                   | ((left >> 1) & 0x00100000)
                   | ((left << 10) & 0x00040000)
                   | ((left << 2) & 0x00020000)
                   | ((left >> 10) & 0x00010000)
                   | ((right >> 13) & 0x00002000)
                   | ((right >> 4) & 0x00001000)
                   | ((right << 6) & 0x00000800)
                   | ((right >> 1) & 0x00000400)
                   | ((right >> 14) & 0x00000200)
                   | (right & 0x00000100)
                   | ((right >> 5) & 0x00000020)
                   | ((right >> 10) & 0x00000010)
                   | ((right >> 3) & 0x00000008)
                   | ((right >> 18) & 0x00000004)
                   | ((right >> 26) & 0x00000002)
                   | ((right >> 24) & 0x00000001));

      *subkey++ = (((left << 15) & 0x20000000)
                   | ((left << 17) & 0x10000000)
                   | ((left << 10) & 0x08000000)
                   | ((left << 22) & 0x04000000)
                   | ((left >> 2) & 0x02000000)
                   | ((left << 1) & 0x01000000)
                   | ((left << 16) & 0x00200000)
                   | ((left << 11) & 0x00100000)
                   | ((left << 3) & 0x00080000)
                   | ((left >> 6) & 0x00040000)
                   | ((left << 15) & 0x00020000)
                   | ((left >> 4) & 0x00010000)
                   | ((right >> 2) & 0x00002000)
                   | ((right << 8) & 0x00001000)
                   | ((right >> 14) & 0x00000808)
                   | ((right >> 9) & 0x00000400)
                   | ((right) & 0x00000200)
                   | ((right << 7) & 0x00000100)
                   | ((right >> 7) & 0x00000020)
                   | ((right >> 3) & 0x00000011)
                   | ((right << 2) & 0x00000004)
                   | ((right >> 21) & 0x00000002));
    }
}

void
gl_des_setkey (gl_des_ctx *ctx, const char * key)
{
  int i;

  des_key_schedule (key, ctx->encrypt_subkeys);

  for (i = 0; i < 32; i += 2)
    {
      ctx->decrypt_subkeys[i] = ctx->encrypt_subkeys[30 - i];
      ctx->decrypt_subkeys[i + 1] = ctx->encrypt_subkeys[31 - i];
    }
}

bool
gl_des_makekey (gl_des_ctx *ctx, const char * key, size_t keylen)
{
  if (keylen != 8)
    return false;

  gl_des_setkey (ctx, key);

  return !gl_des_is_weak_key (key);
}

void
gl_des_ecb_crypt (gl_des_ctx *ctx, const char * _from, char * _to, int mode)
{
  const unsigned char *from = (const unsigned char *) _from;
  unsigned char *to = (unsigned char *) _to;
  uint32_t left, right, work;
  uint32_t *keys;

  keys = mode ? ctx->decrypt_subkeys : ctx->encrypt_subkeys;

  READ_64BIT_DATA (from, left, right)
    INITIAL_PERMUTATION (left, work, right)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    FINAL_PERMUTATION (right, work, left)
    WRITE_64BIT_DATA (to, right, left)
}

void
gl_3des_set2keys (gl_3des_ctx *ctx, const char * key1, const char * key2)
{
  int i;

  des_key_schedule (key1, ctx->encrypt_subkeys);
  des_key_schedule (key2, &(ctx->decrypt_subkeys[32]));

  for (i = 0; i < 32; i += 2)
    {
      ctx->decrypt_subkeys[i] = ctx->encrypt_subkeys[30 - i];
      ctx->decrypt_subkeys[i + 1] = ctx->encrypt_subkeys[31 - i];

      ctx->encrypt_subkeys[i + 32] = ctx->decrypt_subkeys[62 - i];
      ctx->encrypt_subkeys[i + 33] = ctx->decrypt_subkeys[63 - i];

      ctx->encrypt_subkeys[i + 64] = ctx->encrypt_subkeys[i];
      ctx->encrypt_subkeys[i + 65] = ctx->encrypt_subkeys[i + 1];

      ctx->decrypt_subkeys[i + 64] = ctx->decrypt_subkeys[i];
      ctx->decrypt_subkeys[i + 65] = ctx->decrypt_subkeys[i + 1];
    }
}

void
gl_3des_set3keys (gl_3des_ctx *ctx, const char * key1,
                    const char * key2, const char * key3)
{
  int i;

  des_key_schedule (key1, ctx->encrypt_subkeys);
  des_key_schedule (key2, &(ctx->decrypt_subkeys[32]));
  des_key_schedule (key3, &(ctx->encrypt_subkeys[64]));

  for (i = 0; i < 32; i += 2)
    {
      ctx->decrypt_subkeys[i] = ctx->encrypt_subkeys[94 - i];
      ctx->decrypt_subkeys[i + 1] = ctx->encrypt_subkeys[95 - i];

      ctx->encrypt_subkeys[i + 32] = ctx->decrypt_subkeys[62 - i];
      ctx->encrypt_subkeys[i + 33] = ctx->decrypt_subkeys[63 - i];

      ctx->decrypt_subkeys[i + 64] = ctx->encrypt_subkeys[30 - i];
      ctx->decrypt_subkeys[i + 65] = ctx->encrypt_subkeys[31 - i];
    }
}

void
gl_3des_ecb_crypt (gl_3des_ctx *ctx,
                   const char * _from,
                   char * _to, int mode)
{
  const unsigned char *from = (const unsigned char *) _from;
  unsigned char *to = (unsigned char *) _to;
  uint32_t left, right, work;
  uint32_t *keys;

  keys = mode ? ctx->decrypt_subkeys : ctx->encrypt_subkeys;

  READ_64BIT_DATA (from, left, right)
    INITIAL_PERMUTATION (left, work, right)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (left, right, work, keys) DES_ROUND (right, left, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
    FINAL_PERMUTATION (right, work, left)
    WRITE_64BIT_DATA (to, right, left)
}

bool
gl_3des_makekey (gl_3des_ctx *ctx, const char * key, size_t keylen)
{
  if (keylen != 24)
    return false;

  gl_3des_set3keys (ctx, key, key + 8, key + 16);

  return !(gl_des_is_weak_key (key)
           || gl_des_is_weak_key (key + 8)
           || gl_des_is_weak_key (key + 16));
}
