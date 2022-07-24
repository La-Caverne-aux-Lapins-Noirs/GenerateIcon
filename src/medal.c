/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

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

int			medal(t_bunny_configuration		*cnf,
			      t_bunny_picture			*pic,
			      const char			*picfile,
			      const char			*spec)
{
  t_bunny_configuration *tbox = NULL;
  t_bunny_picture	*icon = NULL;

  bunny_clear(&pic->buffer, 0);
  if (picfile)
    icon = bunny_load_picture(picfile);
  // Dessin du texte
  if (spec != NULL)
    bunny_configuration_getf(cnf, &tbox, "Medal[%s]", spec);
  else
    bunny_configuration_getf(cnf, &tbox, "Medal");
  if (!tbox)
    {
      fprintf(stderr, "Missing Medal node in configuration.\n");
      return (EXIT_FAILURE);
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
  t_bunny_position	shift;
  t_bunny_position	cshift;
  t_bunny_position	texmiddle;
  t_bunny_position	medmiddle =
    {
     pic->buffer.width / 2,
     pic->buffer.height / 2
    };
  t_bunny_accurate_position scale;

  bunny_configuration_getf(tbox, &corner, "Corner");
  if (corner > 126)
    corner = 126;
  if (bunny_configuration_getf(tbox, &texture_file, "Texture"))
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

  // D'abord on fait la forme en noir
  va->vertex[0].color = BLACK;
  shape(pic, corner, va, &medmiddle, &texmiddle, NULL, BLACK, 1.0, rot, &shift, &scale);
  // Ensuite on fait le boudin
  va->vertex[0].color = border_color.full;
  shape(pic, corner, va, &medmiddle, &texmiddle, NULL, border_color.full, 1.0 - 0.01, rot, &shift, &scale);
  // On fait l'interieur
  va->vertex[0].color = inside_color.full;
  shape(pic, corner, va, &medmiddle, &texmiddle, texture, middle_color.full, 1.0 - 0.01 - border_width, rot, &shift, &scale);

  // L'icone
  if (icon != NULL)
    {
      bunny_configuration_getf(tbox, &icon->rotation, "IconRotation");
      bunny_configuration_getf(tbox, &icon->scale.x, "IconScale[0]");
      bunny_configuration_getf(tbox, &icon->scale.y, "IconScale[1]");
      icon->origin.x = icon->buffer.width / 2;
      icon->origin.y = icon->buffer.height / 2;
      icon->position.x = pic->buffer.width / 2;
      icon->position.y = pic->buffer.height / 2;
      // Un léger contour noir à l'icone
      icon->color_mask.full = BLACK;
      icon->scale.x += 0.01;
      icon->scale.y += 0.01;
      bunny_blit(&pic->buffer, icon, NULL);
      // Puis l'authentique icone
      icon->scale.x -= 0.01;
      icon->scale.y -= 0.01;
      if (bunny_color_configuration("IconColor", &icon->color_mask, tbox) == false)
	icon->color_mask.full = WHITE;
      bunny_blit(&pic->buffer, icon, NULL);
    }

  return (EXIT_SUCCESS);
}
