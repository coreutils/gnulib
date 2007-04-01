/* Return physical temperatures.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include "phystemp.h"

#if defined __linux__

# include <stdio.h>
# include <string.h>

# include "pipe.h"
# include "wait-process.h"
# include "getline.h"

/* On Linux, temperature values are available through the 'sensors' program,
   part of the 'sensors' or 'lm_sensors' package
     <http://freshmeat.net/projects/lm_sensors/>
     <http://www.lm-sensors.org/>
   To configure it, run 'sensors-detect' as root and follow the
   recommendations.  More explanations in this article:
     <http://www.linuxappliancedesign.com/articles/sensors/sensors.html>  */

double
phystemp_cpu (void)
{
  /* APRIL FOOL!  */
  /* The output of the 'sensors' program consists of lines of the form
       CPU Temp:    +25°C  (low  =    +0°C, high =   +50°C)
     or
       temp1:       +34°C  (limit =  +60°C)                       sensor = thermistor
     So take the first line of this form.  */
  char *argv[2];
  pid_t child;
  int fd[1];
  FILE *fp;
  char *line;
  size_t linesize;
  size_t linelen;
  int exitstatus;
  double temp_cpu;
  double temp_mb;

  /* Call the sensors program.  */
  argv[0] = "/usr/bin/sensors";
  argv[1] = NULL;
  child = create_pipe_in ("sensors", argv[0], argv, DEV_NULL, true, true,
			  false, fd);
  if (child == -1)
    return -273 - 1;

  /* Retrieve its result.  */
  fp = fdopen (fd[0], "r");
  if (fp == NULL)
    return -273 - 1;

  temp_cpu = -273 - 1;
  temp_mb = -273 - 1;
  line = NULL; linesize = 0;
  for (;;)
    {
      linelen = getline (&line, &linesize, fp);
      if (linelen == (size_t)(-1))
	break;
      if (linelen > 0 && line[linelen - 1] == '\n')
	line[linelen - 1] = '\0';
      if (linelen >= 9 && memcmp (line, "CPU Temp:", 9) == 0)
	temp_cpu = strtod (line + 9, NULL);
      else if (linelen >= 5 && memcmp (line, "temp:", 5) == 0)
	{
	  if (temp_cpu < -273)
	    temp_cpu = strtod (line + 5, NULL);
	}
      else if (linelen >= 6 && memcmp (line, "temp1:", 6) == 0)
	{
	  if (temp_cpu < -273)
	    temp_cpu = strtod (line + 6, NULL);
	}
      else if (linelen >= 9 && memcmp (line, "M/B Temp:", 9) == 0)
	temp_mb = strtod (line + 9, NULL);
    }

  fclose (fp);

  /* Remove zombie process from process list, and retrieve exit status.  */
  exitstatus = wait_subprocess (child, "sensors", true, true, true, false);
  if (exitstatus != 0)
    return -273 - 1;

  /* The temperature of the motherboard is a good approximation of the
     temperature of the CPU.  */
  if (temp_cpu < -273)
    temp_cpu = temp_mb;

  return temp_cpu;
}

#elif defined __BEOS__

# include <OS.h>

double
phystemp_cpu (void)
{
  /* Yes, it's a function called 'is_...' that returns a 'double'!
     It is specified as
       "Returns the temperature of the motherboard if the computer is
        currently on fire."
     The temperature of the motherboard is a good approximation of the
     temperature of the CPU.  */
  double temp = is_computer_on_fire ();
  return (temp > 0 ? temp : -273 - 1);
}

#else

double
phystemp_cpu (void)
{
  return -273 - 1;
}

#endif
