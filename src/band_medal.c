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
			   t_medal			*medal)
{
  t_bunny_configuration *tbox = NULL;

  // Dessin du texte
  if (!bunny_configuration_getf(cnf, &tbox, "BandMedal"))
    {
      fprintf(stderr, "Missing BandMedal node in configuration.\n");
      return (EXIT_FAILURE);
    }
  if (medal->specificator != NULL)
    {
      t_bunny_configuration *spec;
      const char *toks[2] = {",", NULL};
      const char * const *spl = bunny_split(medal->specificator, toks, true);

      for (int z = 0; spl[z]; ++z)
	{
	  if (!bunny_configuration_getf(cnf, &spec, "BandMedal.%s", spl[z]))
	    {
	      fprintf(stderr, "Missing BandMedal.%s node in configuration.\n", spl[z]);
	      return (EXIT_FAILURE);
	    }
	  bunny_configuration_merge(1, tbox, spec);
	}
      bunny_delete_split(spl);
    }

  bunny_configuration_setf(tbox, (int)pic->buffer.height, "BoxSize[1]");
  t_bunny_font	*fnt = bunny_read_textbox(tbox);

  if (!fnt)
    {
      fprintf(stderr, "Cannot load BandMedal node.\n");
      return (EXIT_FAILURE);
    }
  if (medal->label)
    fnt->string = bunny_strdup(medal->label);
  else
    fnt->string = bunny_strdup(medal->name);
  fnt->string_len = strlen(fnt->string);
  for (int i = 0; fnt->string[i]; ++i)
    ((char*)fnt->string)[i] = toupper(fnt->string[i]);

  // Dessin des bandes
  t_bunny_hash	res = bunny_hash(BH_DJB2, medal->name, strlen(medal->name));
  unsigned int	cols[6];

  srand(res);
  for (int i = 0; i < NBRCELL(cols); ++i)
    cols[i] = gl_colors[rand() % NBRCELL(gl_colors)] | BLACK;
  int		bands = rand() % 4 + 2;
  int		x, w;

  x = 0;
  bunny_clear(&pic->buffer, cols[rand() % NBRCELL(cols)]);
  for (int i = 0; i < bands; ++i)
    {
      x += rand() % pic->buffer.width / 2 / bands;
      w = rand() % pic->buffer.width / 2 / bands;
      x += w;
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

  // Une texture par dessus les couleurs?
  if (medal->texfile)
    {
      int		transparency = 255;
      t_bunny_picture *tex = bunny_load_picture(medal->texfile);

      bunny_configuration_getf(tbox, &transparency, "TextureTransparency");
      if (tex == NULL)
	{
	  fprintf(stderr, "Cannot load %s.\n", medal->texfile);
	  return (EXIT_FAILURE);
	}
      tex->origin.x = tex->buffer.width / 2;
      tex->origin.y = tex->buffer.height / 2;
      tex->position.x = tex->origin.x;
      tex->position.y = tex->origin.y;
      tex->color_mask.argb[ALPHA_CMP] = transparency;
      bunny_blit(&pic->buffer, tex, NULL);
    }

  // Des images à mettre par dessus, mais sous le texte
  t_bunny_configuration *ovr;

  for (int i = 0; bunny_configuration_getf(tbox, &ovr, "Overlay[%d]", i); ++i)
    {
      t_bunny_picture *lay = NULL;
      bool		undertext = false;

      bunny_configuration_getf(ovr, &undertext, "UnderText");
      if (!undertext)
	continue ;
      bunny_set_clipable_attribute(NULL, &lay, &ovr, BCT_PICTURE);
      bunny_blit(&pic->buffer, lay, NULL);
      bunny_delete_clipable(lay);
    }

  fnt->clipable.position.y = 5;
  bunny_draw(&fnt->clipable);
  bunny_blit(&pic->buffer, &fnt->clipable, NULL);
  bunny_delete_clipable(&fnt->clipable);

  // Des images a mettre par dessus, sur le texte
  for (int i = 0; bunny_configuration_getf(tbox, &ovr, "Overlay[%d]", i); ++i)
    {
      t_bunny_picture *lay = NULL;
      bool		undertext = false;

      bunny_configuration_getf(ovr, &undertext, "UnderText");
      if (undertext)
	continue ;
      bunny_set_clipable_attribute(NULL, &lay, &ovr, BCT_PICTURE);
      bunny_blit(&pic->buffer, lay, NULL);
      bunny_delete_clipable(lay);
    }
  return (EXIT_SUCCESS);
}

