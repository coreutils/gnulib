/* rijndael-api-fst.c --- Rijndael cipher implementation.
 * Copyright (C) 2005-2006, 2009-2022 Free Software Foundation, Inc.
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

/* Adapted for gnulib by Simon Josefsson.
 *
 * Based on public domain "Optimised C code" retrieved from (SHA1
 * 7c8e4b00d06685d1dbc6724a9e0d502353de339e):
 * https://web.archive.org/web/20060618010435/http://www.iaik.tu-graz.ac.at/research/krypto/AES/old/~rijmen/rijndael/rijndael-fst-3.0.zip
 */

#include <config.h>

/**
 * rijndael-api-fst.c
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

#include "rijndael-alg-fst.h"
#include "rijndael-api-fst.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

rijndael_rc
rijndaelMakeKey (rijndaelKeyInstance *key, rijndael_direction direction,
                 size_t keyLen, const char *keyMaterial)
{
  size_t i;
  char *keyMat;
  char cipherKey[RIJNDAEL_MAXKB];

  if (key == NULL)
    {
      return RIJNDAEL_BAD_KEY_INSTANCE;
    }

  if ((direction == RIJNDAEL_DIR_ENCRYPT)
      || (direction == RIJNDAEL_DIR_DECRYPT))
    {
      key->direction = direction;
    }
  else
    {
      return RIJNDAEL_BAD_KEY_DIR;
    }

  if ((keyLen == 128) || (keyLen == 192) || (keyLen == 256))
    {
      key->keyLen = keyLen;
    }
  else
    {
      return RIJNDAEL_BAD_KEY_MAT;
    }

  if (keyMaterial != NULL)
    {
      strncpy (key->keyMaterial, keyMaterial, keyLen / 4);
    }

  /* initialize key schedule: */
  keyMat = key->keyMaterial;
  for (i = 0; i < key->keyLen / 8; i++)
    {
      char t, v;

      t = *keyMat++;
      if ((t >= '0') && (t <= '9'))
        v = (t - '0') << 4;
      else if ((t >= 'a') && (t <= 'f'))
        v = (t - 'a' + 10) << 4;
      else if ((t >= 'A') && (t <= 'F'))
        v = (t - 'A' + 10) << 4;
      else
        return RIJNDAEL_BAD_KEY_MAT;

      t = *keyMat++;
      if ((t >= '0') && (t <= '9'))
        v ^= (t - '0');
      else if ((t >= 'a') && (t <= 'f'))
        v ^= (t - 'a' + 10);
      else if ((t >= 'A') && (t <= 'F'))
        v ^= (t - 'A' + 10);
      else
        return RIJNDAEL_BAD_KEY_MAT;

      cipherKey[i] = v;
    }
  if (direction == RIJNDAEL_DIR_ENCRYPT)
    {
      key->Nr = rijndaelKeySetupEnc (key->rk, cipherKey, keyLen);
    }
  else
    {
      key->Nr = rijndaelKeySetupDec (key->rk, cipherKey, keyLen);
    }
  rijndaelKeySetupEnc (key->ek, cipherKey, keyLen);
  return 0;
}

rijndael_rc
rijndaelCipherInit (rijndaelCipherInstance *cipher, rijndael_mode mode,
                    const char *IV)
{
  if ((mode == RIJNDAEL_MODE_ECB) || (mode == RIJNDAEL_MODE_CBC)
      || (mode == RIJNDAEL_MODE_CFB1))
    {
      cipher->mode = mode;
    }
  else
    {
      return RIJNDAEL_BAD_CIPHER_MODE;
    }
  if (IV != NULL)
    {
      int i;
      for (i = 0; i < RIJNDAEL_MAX_IV_SIZE; i++)
        {
          int t, j;

          t = IV[2 * i];
          if ((t >= '0') && (t <= '9'))
            j = (t - '0') << 4;
          else if ((t >= 'a') && (t <= 'f'))
            j = (t - 'a' + 10) << 4;
          else if ((t >= 'A') && (t <= 'F'))
            j = (t - 'A' + 10) << 4;
          else
            return RIJNDAEL_BAD_CIPHER_INSTANCE;

          t = IV[2 * i + 1];
          if ((t >= '0') && (t <= '9'))
            j ^= (t - '0');
          else if ((t >= 'a') && (t <= 'f'))
            j ^= (t - 'a' + 10);
          else if ((t >= 'A') && (t <= 'F'))
            j ^= (t - 'A' + 10);
          else
            return RIJNDAEL_BAD_CIPHER_INSTANCE;

          cipher->IV[i] = (uint8_t) j;
        }
    }
  else
    {
      memset (cipher->IV, 0, RIJNDAEL_MAX_IV_SIZE);
    }
  return 0;
}

int
rijndaelBlockEncrypt (rijndaelCipherInstance *cipher,
                      const rijndaelKeyInstance *key,
                      const char *input,
                      size_t inputLen, char *outBuffer)
{
  size_t i, k, t, numBlocks;
  union { char bytes[16]; uint32_t words[4]; } block;
  char *iv;

  if (cipher == NULL || key == NULL || key->direction == RIJNDAEL_DIR_DECRYPT)
    {
      return RIJNDAEL_BAD_CIPHER_STATE;
    }
  if (input == NULL || inputLen <= 0)
    {
      return 0;                 /* nothing to do */
    }

  numBlocks = inputLen / 128;

  switch (cipher->mode)
    {
    case RIJNDAEL_MODE_ECB:
      for (i = numBlocks; i > 0; i--)
        {
          rijndaelEncrypt (key->rk, key->Nr, input, outBuffer);
          input += 16;
          outBuffer += 16;
        }
      break;

    case RIJNDAEL_MODE_CBC:
      iv = cipher->IV;
      for (i = numBlocks; i > 0; i--)
        {
          block.words[0] = ((uint32_t *) input)[0] ^ ((uint32_t *) iv)[0];
          block.words[1] = ((uint32_t *) input)[1] ^ ((uint32_t *) iv)[1];
          block.words[2] = ((uint32_t *) input)[2] ^ ((uint32_t *) iv)[2];
          block.words[3] = ((uint32_t *) input)[3] ^ ((uint32_t *) iv)[3];
          rijndaelEncrypt (key->rk, key->Nr, block.bytes, outBuffer);
          memcpy (cipher->IV, outBuffer, 16);
          input += 16;
          outBuffer += 16;
        }
      break;

    case RIJNDAEL_MODE_CFB1:
      iv = cipher->IV;
      for (i = numBlocks; i > 0; i--)
        {
          memcpy (outBuffer, input, 16);
          for (k = 0; k < 128; k++)
            {
              rijndaelEncrypt (key->ek, key->Nr, iv, block.bytes);
              outBuffer[k >> 3] ^= (block.bytes[0] & 0x80U) >> (k & 7);
              for (t = 0; t < 15; t++)
                {
                  iv[t] = (iv[t] << 1) | (iv[t + 1] >> 7);
                }
              iv[15] = (iv[15] << 1) |
                ((outBuffer[k >> 3] >> (7 - (k & 7))) & 1);
            }
          outBuffer += 16;
          input += 16;
        }
      break;

    default:
      return RIJNDAEL_BAD_CIPHER_STATE;
    }

  return 128 * numBlocks;
}

int
rijndaelPadEncrypt (rijndaelCipherInstance *cipher,
                    const rijndaelKeyInstance *key,
                    const char *input,
                    size_t inputOctets, char *outBuffer)
{
  size_t i, numBlocks, padLen;
  union { char bytes[16]; uint32_t words[4]; } block;
  char *iv;

  if (cipher == NULL || key == NULL || key->direction == RIJNDAEL_DIR_DECRYPT)
    {
      return RIJNDAEL_BAD_CIPHER_STATE;
    }
  if (input == NULL || inputOctets <= 0)
    {
      return 0;                 /* nothing to do */
    }

  numBlocks = inputOctets / 16;

  switch (cipher->mode)
    {
    case RIJNDAEL_MODE_ECB:
      for (i = numBlocks; i > 0; i--)
        {
          rijndaelEncrypt (key->rk, key->Nr, input, outBuffer);
          input += 16;
          outBuffer += 16;
        }
      padLen = 16 - (inputOctets - 16 * numBlocks);
      assert (padLen > 0 && padLen <= 16);
      memcpy (block.bytes, input, 16 - padLen);
      memset (block.bytes + 16 - padLen, padLen, padLen);
      rijndaelEncrypt (key->rk, key->Nr, block.bytes, outBuffer);
      break;

    case RIJNDAEL_MODE_CBC:
      iv = cipher->IV;
      for (i = numBlocks; i > 0; i--)
        {
          block.words[0] = ((uint32_t *) input)[0] ^ ((uint32_t *) iv)[0];
          block.words[1] = ((uint32_t *) input)[1] ^ ((uint32_t *) iv)[1];
          block.words[2] = ((uint32_t *) input)[2] ^ ((uint32_t *) iv)[2];
          block.words[3] = ((uint32_t *) input)[3] ^ ((uint32_t *) iv)[3];
          rijndaelEncrypt (key->rk, key->Nr, block.bytes, outBuffer);
          memcpy (cipher->IV, outBuffer, 16);
          input += 16;
          outBuffer += 16;
        }
      padLen = 16 - (inputOctets - 16 * numBlocks);
      assert (padLen > 0 && padLen <= 16);
      for (i = 0; i < 16 - padLen; i++)
        {
          block.bytes[i] = input[i] ^ iv[i];
        }
      for (i = 16 - padLen; i < 16; i++)
        {
          block.bytes[i] = (char) padLen ^ iv[i];
        }
      rijndaelEncrypt (key->rk, key->Nr, block.bytes, outBuffer);
      memcpy (cipher->IV, outBuffer, 16);
      break;

    default:
      return RIJNDAEL_BAD_CIPHER_STATE;
    }

  return 16 * (numBlocks + 1);
}

int
rijndaelBlockDecrypt (rijndaelCipherInstance *cipher,
                      const rijndaelKeyInstance *key,
                      const char *input,
                      size_t inputLen, char *outBuffer)
{
  size_t i, k, t, numBlocks;
  union { char bytes[16]; uint32_t words[4]; } block;
  char *iv;

  if (cipher == NULL
      || key == NULL
      || (cipher->mode != RIJNDAEL_MODE_CFB1
          && key->direction == RIJNDAEL_DIR_ENCRYPT))
    {
      return RIJNDAEL_BAD_CIPHER_STATE;
    }
  if (input == NULL || inputLen <= 0)
    {
      return 0;                 /* nothing to do */
    }

  numBlocks = inputLen / 128;

  switch (cipher->mode)
    {
    case RIJNDAEL_MODE_ECB:
      for (i = numBlocks; i > 0; i--)
        {
          rijndaelDecrypt (key->rk, key->Nr, input, outBuffer);
          input += 16;
          outBuffer += 16;
        }
      break;

    case RIJNDAEL_MODE_CBC:
      iv = cipher->IV;
      for (i = numBlocks; i > 0; i--)
        {
          rijndaelDecrypt (key->rk, key->Nr, input, block.bytes);
          block.words[0] ^= ((uint32_t *) iv)[0];
          block.words[1] ^= ((uint32_t *) iv)[1];
          block.words[2] ^= ((uint32_t *) iv)[2];
          block.words[3] ^= ((uint32_t *) iv)[3];
          memcpy (cipher->IV, input, 16);
          memcpy (outBuffer, block.bytes, 16);
          input += 16;
          outBuffer += 16;
        }
      break;

    case RIJNDAEL_MODE_CFB1:
      iv = cipher->IV;
      for (i = numBlocks; i > 0; i--)
        {
          memcpy (outBuffer, input, 16);
          for (k = 0; k < 128; k++)
            {
              rijndaelEncrypt (key->ek, key->Nr, iv, block.bytes);
              for (t = 0; t < 15; t++)
                {
                  iv[t] = (iv[t] << 1) | (iv[t + 1] >> 7);
                }
              iv[15] = (iv[15] << 1) | ((input[k >> 3] >> (7 - (k & 7))) & 1);
              outBuffer[k >> 3] ^= (block.bytes[0] & 0x80U) >> (k & 7);
            }
          outBuffer += 16;
          input += 16;
        }
      break;

    default:
      return RIJNDAEL_BAD_CIPHER_STATE;
    }

  return 128 * numBlocks;
}

int
rijndaelPadDecrypt (rijndaelCipherInstance *cipher,
                    const rijndaelKeyInstance *key,
                    const char *input,
                    size_t inputOctets, char *outBuffer)
{
  size_t i, numBlocks, padLen;
  union { char bytes[16]; uint32_t words[4]; } block;
  char *iv;

  if (cipher == NULL || key == NULL || key->direction == RIJNDAEL_DIR_ENCRYPT)
    {
      return RIJNDAEL_BAD_CIPHER_STATE;
    }
  if (input == NULL || inputOctets <= 0)
    {
      return 0;                 /* nothing to do */
    }
  if (inputOctets % 16 != 0)
    {
      return RIJNDAEL_BAD_DATA;
    }

  numBlocks = inputOctets / 16;

  switch (cipher->mode)
    {
    case RIJNDAEL_MODE_ECB:
      /* all blocks but last */
      for (i = numBlocks - 1; i > 0; i--)
        {
          rijndaelDecrypt (key->rk, key->Nr, input, outBuffer);
          input += 16;
          outBuffer += 16;
        }
      /* last block */
      rijndaelDecrypt (key->rk, key->Nr, input, block.bytes);
      padLen = block.bytes[15];
      if (padLen >= 16)
        {
          return RIJNDAEL_BAD_DATA;
        }
      for (i = 16 - padLen; i < 16; i++)
        {
          if (block.bytes[i] != padLen)
            {
              return RIJNDAEL_BAD_DATA;
            }
        }
      memcpy (outBuffer, block.bytes, 16 - padLen);
      break;

    case RIJNDAEL_MODE_CBC:
      iv = cipher->IV;
      /* all blocks but last */
      for (i = numBlocks - 1; i > 0; i--)
        {
          rijndaelDecrypt (key->rk, key->Nr, input, block.bytes);
          block.words[0] ^= ((uint32_t *) iv)[0];
          block.words[1] ^= ((uint32_t *) iv)[1];
          block.words[2] ^= ((uint32_t *) iv)[2];
          block.words[3] ^= ((uint32_t *) iv)[3];
          memcpy (iv, input, 16);
          memcpy (outBuffer, block.bytes, 16);
          input += 16;
          outBuffer += 16;
        }
      /* last block */
      rijndaelDecrypt (key->rk, key->Nr, input, block.bytes);
      block.words[0] ^= ((uint32_t *) iv)[0];
      block.words[1] ^= ((uint32_t *) iv)[1];
      block.words[2] ^= ((uint32_t *) iv)[2];
      block.words[3] ^= ((uint32_t *) iv)[3];
      padLen = block.bytes[15];
      if (padLen <= 0 || padLen > 16)
        {
          return RIJNDAEL_BAD_DATA;
        }
      for (i = 16 - padLen; i < 16; i++)
        {
          if (block.bytes[i] != padLen)
            {
              return RIJNDAEL_BAD_DATA;
            }
        }
      memcpy (outBuffer, block.bytes, 16 - padLen);
      break;

    default:
      return RIJNDAEL_BAD_CIPHER_STATE;
    }

  return 16 * numBlocks - padLen;
}
