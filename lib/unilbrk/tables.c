/* Line breaking auxiliary tables.
   Copyright (C) 2001-2003, 2006-2008 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unilbrk/tables.h"

/* Define unilbrkprop, table of line breaking properties.  */
#include "unilbrk/lbrkprop2.h"

const unsigned char unilbrk_table[19][19] =
{
				/* after */
	/* ZW IN GL BA BB B2 HY NS OP CL QU EX ID NU IS SY AL PR PO */
/* ZW */ { P, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, },
/* IN */ { P, I, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* GL */ { P, I, I, I, I, I, I, I, I, P, I, P, I, I, P, P, I, I, I, },
/* BA */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* BB */ { P, I, I, I, I, I, I, I, I, P, I, P, I, I, P, P, I, I, I, },
/* B2 */ { P, D, I, I, D, P, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* HY */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* NS */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* OP */ { P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, P, },
/* CL */ { P, D, I, I, D, D, I, P, D, P, I, P, D, D, P, P, D, D, I, },
/* QU */ { P, I, I, I, I, I, I, I, P, P, I, P, I, I, P, P, I, I, I, },
/* EX */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* ID */ { P, I, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, I, },
/* NU */ { P, I, I, I, D, D, I, I, D, P, I, P, D, I, P, P, I, D, I, },
/* IS */ { P, D, I, I, D, D, I, I, D, P, I, P, D, I, P, P, D, D, D, },
/* SY */ { P, D, I, I, D, D, I, I, D, P, I, P, D, I, P, P, D, D, D, },
/* AL */ { P, I, I, I, D, D, I, I, D, P, I, P, D, I, P, P, I, D, D, },
/* PR */ { P, D, I, I, D, D, I, I, I, P, I, P, I, I, P, P, I, D, D, },
/* PO */ { P, D, I, I, D, D, I, I, D, P, I, P, D, D, P, P, D, D, D, },
/* "" */
/* before */
};
/* Note: The (B2,B2) entry should probably be D instead of P.  */
/* Note: The (PR,ID) entry should probably be D instead of I.  */
