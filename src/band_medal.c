/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#include	<ctype.h>
#include	"genicon.h"

int		band_medal(t_bunny_configuration	*cnf,
			   t_bunny_picture		*pic,
			   const char			*medal_name,
			   const char			*spec)
{
  t_bunny_configuration *tbox = NULL;

  // Dessin du texte
  if (spec != NULL)
    bunny_configuration_getf(cnf, &tbox, "BandMedal[%s]", spec);
  else
    bunny_configuration_getf(cnf, &tbox, "BandMedal");
  if (!tbox)
    {
      fprintf(stderr, "Missing BandMedal node in configuration.\n");
      return (EXIT_FAILURE);
    }
  t_bunny_font	*fnt = bunny_read_textbox(tbox);

  if (!fnt)
    {
      fprintf(stderr, "Cannot load BandMedal node.\n");
      return (EXIT_FAILURE);
    }
  fnt->string = bunny_strdup(medal_name);
  for (int i = 0; fnt->string[i]; ++i)
    ((char*)fnt->string)[i] = toupper(fnt->string[i]);

  // Dessin des bandes
  t_bunny_hash	res = bunny_hash(BH_DJB2, medal_name, strlen(medal_name));
  unsigned int	cols[6];

  srand(res);
  for (int i = 0; i < NBRCELL(cols); ++i)
    cols[i] = gl_colors[rand() % NBRCELL(gl_colors)] | BLACK;
  int		bands = rand() % 4 + 1;
  int		x, w;

  bunny_clear(&pic->buffer, cols[rand() % NBRCELL(cols)]);
  for (int i = 0; i < bands; ++i)
    {
      x = rand() % pic->buffer.width / 2 / bands;
      w = rand() % (pic->buffer.width - x) / 2 / bands;
      pic->clip_x_position = x;
      pic->clip_width = w;
      bunny_clear(&pic->buffer, cols[i + 1]);
    }
  pic->clip_width = pic->buffer.width / 2;
  pic->clip_x_position = 0;
  pic->position.x = pic->buffer.width;
  pic->position.y = pic->buffer.height;
  pic->rotation = 180;
  bunny_blit(&pic->buffer, pic, NULL);
  pic->position.x = 0;
  pic->position.y = 0;
  pic->clip_width = pic->buffer.width;

  fnt->clipable.position.y = 5;
  bunny_draw(&fnt->clipable);
  bunny_blit(&pic->buffer, &fnt->clipable, NULL);
  bunny_delete_clipable(&fnt->clipable);
  return (EXIT_SUCCESS);
}

