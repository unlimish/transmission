/*
 * This file Copyright (C) 2007-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id$
 */

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h> /* abs (), srand (), rand () */

#include "transmission.h"
#include "crypto-utils.h"
#include "utils.h"

/***
****
***/

bool
tr_sha1 (uint8_t    * hash,
         const void * data1,
         int          data1_length,
                      ...)
{
  tr_sha1_ctx_t sha;

  if ((sha = tr_sha1_init ()) == NULL)
    return false;

  if (tr_sha1_update (sha, data1, data1_length))
    {
      va_list vl;
      const void * data;

      va_start (vl, data1_length);
      while ((data = va_arg (vl, const void *)) != NULL)
        {
          const int data_length = va_arg (vl, int);
          assert (data_length >= 0);
          if (!tr_sha1_update (sha, data, data_length))
            break;
        }
      va_end (vl);

      /* did we reach the end of argument list? */
      if (data == NULL)
        return tr_sha1_final (sha, hash);
    }

  tr_sha1_final (sha, NULL);
  return false;
}

/***
****
***/

int
tr_rand_int (int upper_bound)
{
  int noise;

  assert (upper_bound > 0);

  while (tr_rand_buffer (&noise, sizeof (noise)))
    {
      noise = abs(noise) % upper_bound;
      /* abs(INT_MIN) is undefined and could return negative value */
      if (noise >= 0)
        return noise;
    }

  /* fall back to a weaker implementation... */
  return tr_rand_int_weak (upper_bound);
}

int
tr_rand_int_weak (int upper_bound)
{
  static bool init = false;

  assert (upper_bound > 0);

  if (!init)
    {
      srand (tr_time_msec ());
      init = true;
    }

  return rand () % upper_bound;
}