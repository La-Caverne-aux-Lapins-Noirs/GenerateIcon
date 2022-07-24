/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#include		"genicon.h"

int			generate_icon(t_bunny_configuration	*cnf,
				      const char		*type,
				      const char		*medal_name,
				      const char		*picfile,
				      const char		*specificator)
{
  t_bunny_picture	*pic;
  int			ret;

  if (strcmp(type, "band") == 0)
    ret = band_medal(cnf, pic = bunny_new_picture(400, 100), medal_name, specificator);
  else
    // Médaille médaille
    ret = medal(cnf, pic = bunny_new_picture(400, 400), picfile, specificator);
  if (ret == 0)
    {
      bunny_save_picture(pic, ".outputgenicon.png");
      // system("cat .outputgenicon.png ; rm -f .outputgenicon.png");
    }
  return (ret);
}
