/* rijndael-api-fst.h --- Rijndael cipher implementation.
 * Copyright (C) 2005, 2009-2022 Free Software Foundation, Inc.
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

/* Adapted for gnulib by Simon Josefsson. */

/**
 * rijndael-api-fst.h
 *
 * @version 2.9 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * @author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Acknowledgements:
 *
 * We are deeply indebted to the following people for their bug reports,
 * fixes, and improvement suggestions to this implementation. Though we
 * tried to list all contributions, we apologise in advance for any
 * missing reference.
 *
 * Andrew Bales <Andrew.Bales@Honeywell.com>
 * Markus Friedl <markus.friedl@informatik.uni-erlangen.de>
 * John Skodon <skodonj@webquill.com>
 */

#ifndef __RIJNDAEL_API_FST_H
#define __RIJNDAEL_API_FST_H

#include "rijndael-alg-fst.h"

#include <stdio.h>

/* Default number of bits in a cipher block */
#define RIJNDAEL_BITSPERBLOCK 128

/* Number of ASCII char's needed to represent a key */
#define RIJNDAEL_MAX_KEY_SIZE 64

/* Number bytes needed to represent an IV  */
#define RIJNDAEL_MAX_IV_SIZE 16

typedef enum
{
  /*  Key direction is invalid, e.g., unknown value */
  RIJNDAEL_BAD_KEY_DIR = -1,
  /*  Key material not of correct length */
  RIJNDAEL_BAD_KEY_MAT = -2,
  /*  Key passed is not valid */
  RIJNDAEL_BAD_KEY_INSTANCE = -3,
  /*  Params struct passed to cipherInit invalid */
  RIJNDAEL_BAD_CIPHER_MODE = -4,
  /*  Cipher in wrong state (e.g., not initialized) */
  RIJNDAEL_BAD_CIPHER_STATE = -5,
  RIJNDAEL_BAD_BLOCK_LENGTH = -6,
  RIJNDAEL_BAD_CIPHER_INSTANCE = -7,
  /*  Data contents are invalid, e.g., invalid padding */
  RIJNDAEL_BAD_DATA = -8,
  /*  Unknown error */
  RIJNDAEL_BAD_OTHER = -9
} rijndael_rc;

typedef enum
{
  RIJNDAEL_DIR_ENCRYPT = 0,     /*  Are we encrypting?  */
  RIJNDAEL_DIR_DECRYPT = 1      /*  Are we decrypting?  */
} rijndael_direction;

typedef enum
{
  RIJNDAEL_MODE_ECB = 1,        /*  Are we ciphering in ECB mode?   */
  RIJNDAEL_MODE_CBC = 2,        /*  Are we ciphering in CBC mode?   */
  RIJNDAEL_MODE_CFB1 = 3        /*  Are we ciphering in 1-bit CFB mode? */
} rijndael_mode;

/*  The structure for key information */
typedef struct
{
  /* Key used for encrypting or decrypting? */
  rijndael_direction direction;
  /* Length of the key  */
  size_t keyLen;
  /* Raw key data in ASCII, e.g., user input or KAT values */
  char keyMaterial[RIJNDAEL_MAX_KEY_SIZE + 1];
  /* key-length-dependent number of rounds */
  int Nr;
  /* key schedule */
  uint32_t rk[4 * (RIJNDAEL_MAXNR + 1)];
  /* CFB1 key schedule (encryption only) */
  uint32_t ek[4 * (RIJNDAEL_MAXNR + 1)];
} rijndaelKeyInstance;

/*  The structure for cipher information */
typedef struct
{                               /* changed order of the components */
  rijndael_mode mode;           /* MODE_ECB, MODE_CBC, or MODE_CFB1 */
  /* A possible Initialization Vector for ciphering */
  char IV[RIJNDAEL_MAX_IV_SIZE];
} rijndaelCipherInstance;

/*  Function prototypes  */

/* Create KEY, for encryption or decryption depending on DIRECTION,
   from KEYMATERIAL, a hex string, of KEYLEN size.  KEYLEN should be
   128, 192 or 256. Returns 0 on success, or an error code. */
extern rijndael_rc
rijndaelMakeKey (rijndaelKeyInstance *key, rijndael_direction direction,
                 size_t keyLen, const char *keyMaterial);

/* Initialize cipher state CIPHER for encryption MODE (e.g.,
   RIJNDAEL_MODE_CBC) with initialization vector IV, a hex string of
   2*RIJNDAEL_MAX_IV_SIZE length.  IV may be NULL for modes that do
   not need an IV (i.e., RIJNDAEL_MODE_ECB).  */
extern rijndael_rc
rijndaelCipherInit (rijndaelCipherInstance *cipher,
                    rijndael_mode mode, const char *IV);

/* Encrypt data in INPUT, of INPUTLEN/8 bytes length, placing the
   output in the pre-allocated OUTBUFFER which must hold at least
   INPUTLEN/8 bytes of data.  The CIPHER is used as state, and must be
   initialized with rijndaelCipherInit before calling this function.
   The encryption KEY must be initialized with rijndaelMakeKey before
   calling this function.  Return the number of bits written, or a
   negative rijndael_rc error code. */
extern int
rijndaelBlockEncrypt (rijndaelCipherInstance *cipher,
                      const rijndaelKeyInstance *key,
                      const char *input, size_t inputLen,
                      char *restrict outBuffer);

/* Encrypt data in INPUT, of INPUTOCTETS bytes length, placing the
   output in the pre-allocated OUTBUFFER which must hold at least
   INPUTOCTETS aligned to the next block size boundary.
   Ciphertext-Stealing as described in RFC 2040 is used to encrypt
   partial blocks.  The CIPHER is used as state, and must be
   initialized with rijndaelCipherInit before calling this function.
   The encryption KEY must be initialized with rijndaelMakeKey before
   calling this function.  Return the number of bits written, or a
   negative rijndael_rc error code. */
extern int
rijndaelPadEncrypt (rijndaelCipherInstance *cipher,
                    const rijndaelKeyInstance *key,
                    const char *input, size_t inputOctets,
                    char *restrict outBuffer);

/* Decrypt data in INPUT, of INPUTLEN/8 bytes length, placing the
   output in the pre-allocated OUTBUFFER which must hold at least
   INPUTLEN/8 bytes of data.  The CIPHER is used as state, and must be
   initialized with rijndaelCipherInit before calling this function.
   The encryption KEY must be initialized with rijndaelMakeKey before
   calling this function.  Return the number of bits written, or a
   negative rijndael_rc error code. */
extern int
rijndaelBlockDecrypt (rijndaelCipherInstance *cipher,
                      const rijndaelKeyInstance *key,
                      const char *input, size_t inputLen,
                      char *restrict outBuffer);

/* Decrypt data in INPUT, of INPUTOCTETS bytes length, placing the
   output in the pre-allocated OUTBUFFER which must hold at least
   INPUTOCTETS aligned to the next block size boundary.
   Ciphertext-Stealing as described in RFC 2040 is used to encrypt
   partial blocks.  The CIPHER is used as state, and must be
   initialized with rijndaelCipherInit before calling this function.
   The encryption KEY must be initialized with rijndaelMakeKey before
   calling this function.  Return the number of bits written, or a
   negative rijndael_rc error code. */
extern int
rijndaelPadDecrypt (rijndaelCipherInstance *cipher,
                    const rijndaelKeyInstance *key,
                    const char *input, size_t inputOctets,
                    char *restrict outBuffer);

#endif /* __RIJNDAEL_API_FST_H */
