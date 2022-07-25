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

int		main(int			argc,
		     char			**argv)
{
  if (argc < 3 || (strcmp(argv[1], "get") != 0 && strcmp(argv[1], "gen") != 0))
    {
      fprintf(stderr, "%s: Usage is:\n\t./genicon [get medal_name | gen [band | pins] medal_name [-p picture]? [-s specificator]? [-c configuration_file]?].\n\n", argv[0]);
      return (EXIT_FAILURE);
    }

  bunny_enable_full_blit(true);

  // Récupérer des infos dans la BDD, c'est plutot le boulot de PHP.
  if (strcmp(argv[1], "get") == 0)
    {
      char	buffer[1024];

      snprintf(&buffer[0], sizeof(buffer), "./fetchicon %s", argv[1]);
      system(&buffer[0]);
      return (EXIT_SUCCESS);
    }

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

  medal.name = argv[3];
  medal.picfile = fetch_param(argc, argv, "-p");
  medal.specificator = fetch_param(argc, argv, "-s");
  medal.label = fetch_param(argc, argv, "-l");
  medal.texfile = fetch_param(argc, argv, "-t");
  return (generate_icon(cnf, argv[2], &medal));
}
