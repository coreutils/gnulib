/* Test of command line argument processing.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

static int a_seen;
static int b_seen;

static const struct option long_options_required[] =
  {
    { "alpha",    no_argument,       NULL, 'a' },
    { "beta",     no_argument,       &b_seen, 1 },
    { "prune",    required_argument, NULL, 'p' },
    { "quetsche", required_argument, NULL, 'q' },
    { "xtra",     no_argument,       NULL, 1001 },
    { "xtreme",   no_argument,       NULL, 1002 },
    { "xtremely", no_argument,       NULL, 1003 },
    { NULL,       0,                 NULL, 0 }
  };

static const struct option long_options_optional[] =
  {
    { "alpha",    no_argument,       NULL, 'a' },
    { "beta",     no_argument,       &b_seen, 1 },
    { "prune",    optional_argument, NULL, 'p' },
    { "quetsche", optional_argument, NULL, 'q' },
    { NULL,       0,                 NULL, 0 }
  };

static void
getopt_long_loop (int argc, const char **argv,
		  const char *options, const struct option *long_options,
		  const char **p_value, const char **q_value,
		  int *non_options_count, const char **non_options,
		  int *unrecognized)
{
  int option_index;
  int c;

  opterr = 0;
  while ((c = getopt_long (argc, (char **) argv, options, long_options,
			   &option_index))
	 != -1)
    {
      switch (c)
	{
	case 0:
	  /* An option with a non-NULL flag pointer was processed.  */
	  break;
	case 'a':
	  a_seen++;
	  break;
	case 'b':
	  b_seen = 1;
	  break;
	case 'p':
	  *p_value = optarg;
	  break;
	case 'q':
	  *q_value = optarg;
	  break;
	case '\1':
	  /* Must only happen with option '-' at the beginning.  */
	  ASSERT (options[0] == '-');
	  non_options[(*non_options_count)++] = optarg;
	  break;
	case '?':
	  *unrecognized = optopt;
	  break;
	default:
	  *unrecognized = c;
	  break;
	}
    }
}

/* Reduce casting, so we can use string literals elsewhere.
   getopt_long takes an array of char*, but luckily does not modify
   those elements, so we can pass const char*.  */
static int
do_getopt_long (int argc, const char **argv, const char *shortopts,
                const struct option *longopts, int *longind)
{
  return getopt_long (argc, (char **) argv, shortopts, longopts, longind);
}

static void
test_getopt_long (void)
{
  int start;

  /* Test disambiguation of options.  */
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--x";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == '?');
    ASSERT (optopt == 0);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xt";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == '?');
    ASSERT (optopt == 0);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtr";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == '?');
    ASSERT (optopt == 0);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtra";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == 1001);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtre";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == '?');
    ASSERT (optopt == 0);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtrem";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == '?');
    ASSERT (optopt == 0);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtreme";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == 1002);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtremel";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == 1003);
  }
  {
    int argc = 0;
    const char *argv[10];
    int option_index;
    int c;

    argv[argc++] = "program";
    argv[argc++] = "--xtremely";
    optind = 1;
    opterr = 0;
    c = do_getopt_long (argc, argv, "ab", long_options_required, &option_index);
    ASSERT (c == 1003);
  }

  /* Test processing of boolean options.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-a";
      argv[argc++] = "foo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "ab", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 2);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-b";
      argv[argc++] = "-a";
      argv[argc++] = "foo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "ab", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 1);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 3);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-ba";
      argv[argc++] = "foo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "ab", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 1);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 2);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-ab";
      argv[argc++] = "-a";
      argv[argc++] = "foo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "ab", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 2);
      ASSERT (b_seen == 1);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 3);
    }

  /* Test processing of options with arguments.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-pfoo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "p:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "foo") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 2);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-p";
      argv[argc++] = "foo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "p:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "foo") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 3);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-ab";
      argv[argc++] = "-q";
      argv[argc++] = "baz";
      argv[argc++] = "-pfoo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 1);
      ASSERT (p_value != NULL && strcmp (p_value, "foo") == 0);
      ASSERT (q_value != NULL && strcmp (q_value, "baz") == 0);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 5);
    }

  /* Test processing of options with optional arguments.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-pfoo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "p::q::", long_options_optional,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "foo") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 2);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-p";
      argv[argc++] = "foo";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "p::q::", long_options_optional,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 2);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-p";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp::q::", long_options_optional,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 3);
    }

  /* Check that invalid options are recognized.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-p";
      argv[argc++] = "foo";
      argv[argc++] = "-x";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "foo") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 'x');
      ASSERT (optind == 5);
    }

  /* Check that by default, non-options arguments are moved to the end.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "-p") == 0);
      ASSERT (strcmp (argv[2], "billy") == 0);
      ASSERT (strcmp (argv[3], "-a") == 0);
      ASSERT (strcmp (argv[4], "donald") == 0);
      ASSERT (strcmp (argv[5], "duck") == 0);
      ASSERT (strcmp (argv[6], "bar") == 0);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "billy") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 4);
    }

  /* Check that '--' ends the argument processing.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[20];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "--";
      argv[argc++] = "-b";
      argv[argc++] = "foo";
      argv[argc++] = "-q";
      argv[argc++] = "johnny";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "-p") == 0);
      ASSERT (strcmp (argv[2], "billy") == 0);
      ASSERT (strcmp (argv[3], "-a") == 0);
      ASSERT (strcmp (argv[4], "--") == 0);
      ASSERT (strcmp (argv[5], "donald") == 0);
      ASSERT (strcmp (argv[6], "duck") == 0);
      ASSERT (strcmp (argv[7], "-b") == 0);
      ASSERT (strcmp (argv[8], "foo") == 0);
      ASSERT (strcmp (argv[9], "-q") == 0);
      ASSERT (strcmp (argv[10], "johnny") == 0);
      ASSERT (strcmp (argv[11], "bar") == 0);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "billy") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 5);
    }

  /* Check that the '-' flag causes non-options to be returned in order.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "-abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "donald") == 0);
      ASSERT (strcmp (argv[2], "-p") == 0);
      ASSERT (strcmp (argv[3], "billy") == 0);
      ASSERT (strcmp (argv[4], "duck") == 0);
      ASSERT (strcmp (argv[5], "-a") == 0);
      ASSERT (strcmp (argv[6], "bar") == 0);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "billy") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 3);
      ASSERT (strcmp (non_options[0], "donald") == 0);
      ASSERT (strcmp (non_options[1], "duck") == 0);
      ASSERT (strcmp (non_options[2], "bar") == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 7);
    }

  /* Check that '--' ends the argument processing.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[20];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "--";
      argv[argc++] = "-b";
      argv[argc++] = "foo";
      argv[argc++] = "-q";
      argv[argc++] = "johnny";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "-abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "donald") == 0);
      ASSERT (strcmp (argv[2], "-p") == 0);
      ASSERT (strcmp (argv[3], "billy") == 0);
      ASSERT (strcmp (argv[4], "duck") == 0);
      ASSERT (strcmp (argv[5], "-a") == 0);
      ASSERT (strcmp (argv[6], "--") == 0);
      ASSERT (strcmp (argv[7], "-b") == 0);
      ASSERT (strcmp (argv[8], "foo") == 0);
      ASSERT (strcmp (argv[9], "-q") == 0);
      ASSERT (strcmp (argv[10], "johnny") == 0);
      ASSERT (strcmp (argv[11], "bar") == 0);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "billy") == 0);
      ASSERT (q_value == NULL);
      if (non_options_count == 2)
      {
	/* glibc behaviour.  */
	ASSERT (non_options_count == 2);
	ASSERT (strcmp (non_options[0], "donald") == 0);
	ASSERT (strcmp (non_options[1], "duck") == 0);
	ASSERT (unrecognized == 0);
	ASSERT (optind == 7);
      }
      else
      {
	/* Another valid behaviour.  */
	ASSERT (non_options_count == 7);
	ASSERT (strcmp (non_options[0], "donald") == 0);
	ASSERT (strcmp (non_options[1], "duck") == 0);
	ASSERT (strcmp (non_options[2], "-b") == 0);
	ASSERT (strcmp (non_options[3], "foo") == 0);
	ASSERT (strcmp (non_options[4], "-q") == 0);
	ASSERT (strcmp (non_options[5], "johnny") == 0);
	ASSERT (strcmp (non_options[6], "bar") == 0);
	ASSERT (unrecognized == 0);
	ASSERT (optind == 12);
      }
    }

  /* Check that the '-' flag has to come first.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp:q:-", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "-p") == 0);
      ASSERT (strcmp (argv[2], "billy") == 0);
      ASSERT (strcmp (argv[3], "-a") == 0);
      ASSERT (strcmp (argv[4], "donald") == 0);
      ASSERT (strcmp (argv[5], "duck") == 0);
      ASSERT (strcmp (argv[6], "bar") == 0);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "billy") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 4);
    }

  /* Check that the '+' flag causes the first non-option to terminate the
     loop.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "+abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "donald") == 0);
      ASSERT (strcmp (argv[2], "-p") == 0);
      ASSERT (strcmp (argv[3], "billy") == 0);
      ASSERT (strcmp (argv[4], "duck") == 0);
      ASSERT (strcmp (argv[5], "-a") == 0);
      ASSERT (strcmp (argv[6], "bar") == 0);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 1);
    }
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "-+";
      optind = start;
      getopt_long_loop (argc, argv, "+abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == '+');
      ASSERT (optind == 2);
    }

  /* Check that '--' ends the argument processing.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[20];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "--";
      argv[argc++] = "-b";
      argv[argc++] = "foo";
      argv[argc++] = "-q";
      argv[argc++] = "johnny";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "+abp:q:", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "donald") == 0);
      ASSERT (strcmp (argv[2], "-p") == 0);
      ASSERT (strcmp (argv[3], "billy") == 0);
      ASSERT (strcmp (argv[4], "duck") == 0);
      ASSERT (strcmp (argv[5], "-a") == 0);
      ASSERT (strcmp (argv[6], "--") == 0);
      ASSERT (strcmp (argv[7], "-b") == 0);
      ASSERT (strcmp (argv[8], "foo") == 0);
      ASSERT (strcmp (argv[9], "-q") == 0);
      ASSERT (strcmp (argv[10], "johnny") == 0);
      ASSERT (strcmp (argv[11], "bar") == 0);
      ASSERT (a_seen == 0);
      ASSERT (b_seen == 0);
      ASSERT (p_value == NULL);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind = 1);
    }

  /* Check that the '+' flag has to come first.  */
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      const char *p_value = NULL;
      const char *q_value = NULL;
      int non_options_count = 0;
      const char *non_options[10];
      int unrecognized = 0;
      int argc = 0;
      const char *argv[10];
      a_seen = 0;
      b_seen = 0;

      argv[argc++] = "program";
      argv[argc++] = "donald";
      argv[argc++] = "-p";
      argv[argc++] = "billy";
      argv[argc++] = "duck";
      argv[argc++] = "-a";
      argv[argc++] = "bar";
      optind = start;
      getopt_long_loop (argc, argv, "abp:q:+", long_options_required,
			&p_value, &q_value,
			&non_options_count, non_options, &unrecognized);
      ASSERT (strcmp (argv[0], "program") == 0);
      ASSERT (strcmp (argv[1], "-p") == 0);
      ASSERT (strcmp (argv[2], "billy") == 0);
      ASSERT (strcmp (argv[3], "-a") == 0);
      ASSERT (strcmp (argv[4], "donald") == 0);
      ASSERT (strcmp (argv[5], "duck") == 0);
      ASSERT (strcmp (argv[6], "bar") == 0);
      ASSERT (a_seen == 1);
      ASSERT (b_seen == 0);
      ASSERT (p_value != NULL && strcmp (p_value, "billy") == 0);
      ASSERT (q_value == NULL);
      ASSERT (non_options_count == 0);
      ASSERT (unrecognized == 0);
      ASSERT (optind == 4);
    }
}

/* Test behavior of getopt_long when POSIXLY_CORRECT is set in the
   environment.  Options with optional arguments should not change
   behavior just because of an environment variable.
   http://lists.gnu.org/archive/html/bug-m4/2006-09/msg00028.html  */
static void
test_getopt_long_posix (void)
{
  int c = 3;
  const char *v[4] = {"test", "-r", "foo", NULL};
  struct option l[] = {{NULL, 0, NULL, 0}};
  int start;
  int result;
  for (start = OPTIND_MIN; start <= 1; start++)
    {
      optind = start;
      result = do_getopt_long (c, v, "r::", l, NULL);
    }
  ASSERT (result == 'r');
  ASSERT (optarg == NULL);
}
