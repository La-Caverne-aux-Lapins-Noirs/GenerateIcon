/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#include		"genicon.h"

static char		*fetch_param(int	argc,
				     char	**argv,
				     const char	*opt)
{
  for (int i = 1; i < argc; ++i)
    {
      if (strcmp(argv[i], opt) == 0)
	{
	  if (i + 1 >= argc)
	    {
	      fprintf(stderr, "%s: Missing parameter after %s option.\n", argv[i], opt);
	      return (NULL);
	    }
	  return (argv[i + 1]);
	}
    }
  return (NULL);
}

static char		*merge(char		**wt,
			       int		len)
{
  char			buffer[4096];
  size_t		p;
  int			i;
  int			l;

  if (len == 0)
    return (NULL);
  for (i = 0, p = 0; i < len && p < sizeof(buffer) - 1; ++i)
    {
      l = snprintf(&buffer[p], sizeof(buffer) - p - 1, "%s,", wt[i]);
      p += l;
    }
  buffer[p - 1] = '\0';
  return (bunny_strdup(&buffer[0]));
}

int			main(int		argc,
			     char		**argv)
{
  if (argc < 3)
    {
      fprintf(stderr, "%s: Usage is:\n\t./genicon [band | sband | pins] medal_name [-p picture]? [-s specificator]? [-c configuration_file]?].\n\n", argv[0]);
      return (EXIT_FAILURE);
    }
  bunny_enable_full_blit(true);

  // On génère un PNG sur la sortie standard.
  char		*opt = fetch_param(argc, argv, "-c");
  t_bunny_configuration	*cnf;

  if (opt == NULL)
    opt = "/opt/technocore/genicon.dab";
  if ((cnf = bunny_open_configuration(opt, NULL)) == NULL)
    {
      fprintf(stderr, "%s: Cannot load configuration file %s.\n", argv[0], opt);
      return (EXIT_FAILURE);
    }
  t_medal	medal;
  char		*specs[10];
  int		spec = 0;

  medal.name = argv[2];
  medal.picfile = fetch_param(argc, argv, "-p");
  medal.label = fetch_param(argc, argv, "-l");
  medal.texfile = fetch_param(argc, argv, "-t");
  for (int i = 0; i < argc; ++i)
    if (strcmp(argv[i], "-s") == 0 && i + 1 < argc)
      specs[spec++] = argv[++i];
  medal.specificator = merge(&specs[0], spec);
  return (generate_icon(cnf, argv[1], &medal));
}
