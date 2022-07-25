/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#define			_POSIX_C_SOURCE				199309L
#include		<time.h>
#include		"genicon.h"

int			generate_icon(t_bunny_configuration	*cnf,
				      const char		*type,
				      t_medal			*medal)
{
  char			buffer[128];
  char			cmd[512];
  t_bunny_picture	*pic;
  int			ret;

  if (strcmp(type, "band") == 0)
    ret = band_medal(cnf, pic = bunny_new_picture(400, 100), medal);
  else if (strcmp(type, "sband") == 0)
    ret = band_medal(cnf, pic = bunny_new_picture(400, 400), medal);
  else
    // Médaille médaille
    ret = round_medal(cnf, pic = bunny_new_picture(400, 400), medal);
  if (ret == 0)
    {
      struct timespec ts;

      clock_gettime(CLOCK_MONOTONIC, &ts);
      snprintf(&buffer[0], sizeof(buffer), ".output%zu.png", ts.tv_sec + ts.tv_nsec);
      bunny_save_picture(pic, buffer);
      snprintf(&cmd[0], sizeof(cmd), "cat %s ; rm -f %s", buffer, buffer);
      system(cmd);
    }
  return (ret);
}
