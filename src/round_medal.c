/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#include		<ctype.h>
#include		<math.h>
#include		"genicon.h"

struct			bva
{
  size_t		length;
  t_bunny_vertex	vertex[128];
};

void			shape(t_bunny_picture			*pic,
			      int				corner,
			      t_bunny_vertex_array		*va,
			      t_bunny_position			*medmiddle,
			      t_bunny_position			*texmiddle,
			      t_bunny_picture			*texture,
			      unsigned int			color,
			      double				coef,
			      double				rot,
			      t_bunny_position			*shift,
			      t_bunny_accurate_position		*scale)
{
  for (int i = 0; i <= corner; ++i)
    {
      double		q = 2 * M_PI / corner;
      t_bunny_vertex	*v = &va->vertex[i + 1];
      double		cx = cos(q * i + rot) * scale->x;
      double		cy = sin(q * i + rot) * scale->y;

      v->color = color;
      v->pos.x = cx * medmiddle->x * coef + medmiddle->x + shift->x;
      v->pos.y = cy * medmiddle->y * coef + medmiddle->y + shift->y;
      if (texture)
	{
	  v->tex.x = cx * texmiddle->x + texmiddle->x;
	  v->tex.y = cy * texmiddle->y + texmiddle->y;
	}
    }
  bunny_set_geometry(&pic->buffer, BGY_TRIANGLE_FAN, (t_bunny_vertex_array*)va, texture);
}

int			round_medal(t_bunny_configuration	*cnf,
				    t_bunny_picture		*pic,
				    t_medal			*medal)
{
  t_bunny_configuration *tbox = NULL;
  t_bunny_picture	*icon = NULL;

  bunny_clear(&pic->buffer, 0);
  if (medal->picfile)
    icon = bunny_load_picture(medal->picfile);

  // Dessin du texte
  if (!bunny_configuration_getf(cnf, &tbox, "Medal"))
    {
      fprintf(stderr, "Missing Medal node in configuration.\n");
      return (EXIT_FAILURE);
    }
  if (medal->specificator != NULL)
    {
      t_bunny_configuration *spec;
      const char *toks[2] = {",", NULL};
      const char * const *spl = bunny_split(medal->specificator, toks, true);

      for (int z = 0; spl[z]; ++z)
	{
	  if (!bunny_configuration_getf(cnf, &spec, "Medal.%s", spl[z]))
	    {
	      fprintf(stderr, "Missing Medal[%s] node in configuration.\n", spl[z]);
	      return (EXIT_FAILURE);
	    }
	  bunny_configuration_merge(1, tbox, spec);
	}
      bunny_delete_split(spl);
    }

  t_bunny_vertex_array	*va = calloc(1, sizeof(struct bva));
  int			corner = 126;
  double		rot = 0;
  double		border_width = 0.1;
  t_bunny_color		border_color = {.full = WHITE};
  t_bunny_color		inside_color = {.full = BLACK};
  t_bunny_color		middle_color = {.full = BLACK};
  const char		*texture_file = NULL;
  t_bunny_picture	*texture = NULL;
  t_bunny_position	shift = {0, 0};
  t_bunny_position	cshift = {0, 0};
  t_bunny_position	texmiddle;
  t_bunny_position	medmiddle =
    {
     pic->buffer.width / 2,
     pic->buffer.height / 2
    };
  t_bunny_accurate_position scale = {1.0, 1.0};

  bunny_configuration_getf(tbox, &corner, "Corner");
  if (corner > 126)
    corner = 126;
  texture_file = medal->texfile;
  bunny_configuration_getf(tbox, &texture_file, "Texture");
  if (texture_file)
    if ((texture = bunny_load_picture(texture_file)))
      {
	texmiddle.x = texture->buffer.width / 2;
	texmiddle.y = texture->buffer.height / 2;
      }
  if (corner < 3)
    corner = 3;
  bunny_configuration_getf(tbox, &rot, "Rotation");
  rot = rot / 360.0 * (2 * M_PI);
  bunny_color_configuration("BorderColor", &border_color, tbox);
  bunny_color_configuration("InsideColor", &inside_color, tbox);
  bunny_color_configuration("MiddleColor", &middle_color, tbox);
  bunny_configuration_getf(tbox, &border_width, "BorderWidth");
  bunny_configuration_getf(tbox, &shift.x, "Shift[0]");
  bunny_configuration_getf(tbox, &shift.y, "Shift[1]");
  bunny_configuration_getf(tbox, &cshift.x, "CenterShift[0]");
  bunny_configuration_getf(tbox, &cshift.y, "CenterShift[1]");
  bunny_configuration_getf(tbox, &scale.x, "Scale[0]");
  bunny_configuration_getf(tbox, &scale.y, "Scale[1]");

  va->length = corner + 2;
  va->vertex[0].pos.x = medmiddle.x + shift.x + cshift.x;
  va->vertex[0].pos.y = medmiddle.y + shift.y + cshift.y;
  va->vertex[0].tex.x = texmiddle.x;
  va->vertex[0].tex.y = texmiddle.y;

  // Il serait sympathique de pouvoir préciser les coordonnées dans la conf
  // pour ne pas faire que des formes régulières... comme "l'oeil" d'avant.

  // D'abord on fait la forme en noir
  va->vertex[0].color = ALPHA(border_color.argb[ALPHA_CMP], BLACK);
  shape(pic, corner, va, &medmiddle, &texmiddle, NULL, va->vertex[0].color, 1.0, rot, &shift, &scale);
  // Ensuite on fait le boudin
  va->vertex[0].color = border_color.full;
  shape(pic, corner, va, &medmiddle, &texmiddle, NULL, border_color.full, 1.0 - 0.01, rot, &shift, &scale);
  // On fait l'interieur
  va->vertex[0].color = middle_color.full;
  shape(pic, corner, va, &medmiddle, &texmiddle, texture, inside_color.full, 1.0 - 0.01 - border_width, rot, &shift, &scale);

  // L'icone
  if (icon != NULL)
    {
      t_bunny_position ishift = {0, 0};

      bunny_configuration_getf(tbox, &ishift.x, "Icon.Shift[0]");
      bunny_configuration_getf(tbox, &ishift.y, "Icon.Shift[1]");
      bunny_configuration_getf(tbox, &icon->rotation, "Icon.Rotation");
      bunny_configuration_getf(tbox, &icon->scale.x, "Icon.Scale[0]");
      bunny_configuration_getf(tbox, &icon->scale.y, "Icon.Scale[1]");
      icon->origin.x = icon->buffer.width / 2;
      icon->origin.y = icon->buffer.height / 2;
      icon->position.x = pic->buffer.width / 2 + ishift.x;
      icon->position.y = pic->buffer.height / 2 + ishift.y;
      // Un léger contour noir à l'icone
      if (bunny_color_configuration("Icon.Color", &icon->color_mask, tbox) != BD_OK)
	icon->color_mask.full = WHITE;
      icon->color_mask.full = ALPHA(icon->color_mask.argb[ALPHA_CMP], BLACK);
      icon->scale.x += 0.01;
      icon->scale.y += 0.01;
      bunny_blit(&pic->buffer, icon, NULL);
      // Puis l'authentique icone
      icon->scale.x -= 0.01;
      icon->scale.y -= 0.01;
      if (bunny_color_configuration("Icon.Color", &icon->color_mask, tbox) != BD_OK)
	icon->color_mask.full = WHITE;
      bunny_blit(&pic->buffer, icon, NULL);

      // Il serait sympathique d'avoir une propriété "KeepInside", permettant
      // d'imposer la bordure sur l'icone, au lieu de permettre à l'icone de déborder,
      // en fonction des dessins...
    }

  // Des images à mettre par dessus.
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

  t_bunny_configuration *lab;

  if (bunny_configuration_getf(tbox, &lab, "Label"))
    {
      t_bunny_font	*fnt = bunny_read_textbox(lab);

      if (!fnt)
	{
	  fprintf(stderr, "Cannot load Label node.\n");
	  return (EXIT_FAILURE);
	}
      if (medal->label)
	fnt->string = bunny_strdup(medal->label);
      if (fnt->string)
	{
	  for (int i = 0; fnt->string[i]; ++i)
	    ((char*)fnt->string)[i] = toupper(fnt->string[i]);
	  fnt->string_len = strlen(fnt->string);
	  bunny_draw(&fnt->clipable);
	  bunny_blit(&pic->buffer, &fnt->clipable, NULL);
	}
    }

  // Images sur le texte
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
