/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#include		<strings.h>
#include		<ctype.h>
#include		<math.h>
#include		<time.h>
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
			      t_bunny_accurate_position		*scale,
			      const char			*shap,
			      double				rnd,
			      double				rnd2,
			      t_bunny_configuration		*tbox)
{
  double		sun[2] = {0.9, 1.0};
  double		spike[2] = {0.5, 1.0};
  double		step = 0.45 / corner;
  t_bunny_accurate_position	arbitrary;

  if (strcasecmp(shap, "noshape") == 0)
    return ;
  for (int i = 0; i <= corner; ++i)
    {
      double		q = 2 * M_PI / corner;
      t_bunny_vertex	*v = &va->vertex[i + 1];
      double		cx = cos(q * i + rot) * scale->x;
      double		cy = sin(q * i + rot) * scale->y;

      v->color = color;
      v->pos.x = cx * medmiddle->x * coef;
      v->pos.y = cy * medmiddle->y * coef;
      if (strcasecmp(shap, "sun") == 0)
	{
	  v->pos.x *= sun[i % 2];
	  v->pos.y *= sun[i % 2];
	}
      else if (strcasecmp(shap, "spike") == 0)
	{
	  v->pos.x *= spike[i % 2];
	  v->pos.y *= spike[i % 2];
	}
      else if (strcasecmp(shap, "sinusoid") == 0)
	{
	  double	r = cos(rnd * 2 * M_PI * i / corner);

	  v->pos.x *= r * rnd2 + (1.0 - rnd2);
	  v->pos.y *= r * rnd2 + (1.0 - rnd2);
	}
      else if (strcasecmp(shap, "snail") == 0)
	{
	  v->pos.x *= step * i + 0.7;
	  v->pos.y *= step * i + 0.7;
	}
      else if (strcasecmp(shap, "random") == 0)
	{
	  double	r = (rand() % 1000 / 1000.0) * rnd;

	  v->pos.x *= (1.0 - rnd) + r;
	  v->pos.y *= (1.0 - rnd) + r;
	}
      else if (strcasecmp(shap, "arbitrary") == 0)
	{
	  const char *test = NULL;
	  
	  if (!bunny_configuration_getf(tbox, &arbitrary.x, "Coordinates[%d]", i * 2 + 0) ||
	      !bunny_configuration_getf(tbox, &arbitrary.y, "Coordinates[%d]", i * 2 + 1))
	    arbitrary.x = arbitrary.y = 0;
	  if (!bunny_configuration_getf(tbox, &test, "PositionType"))
	    test = "Absolute";
	  if (strcasecmp(test, "relative"))
	    {
	      // de -100% a 100%
	      v->pos.x = arbitrary.x * medmiddle->x * coef;
	      v->pos.y = arbitrary.y * medmiddle->y * coef;
	    }
	  else
	    {
	      v->pos.x = arbitrary.x * coef;
	      v->pos.y = arbitrary.y * coef;
	    }
	  double	ang = atan2(v->pos.y, v->pos.x);
	  double	norm = sqrt(powf(v->pos.y, 2) + powf(v->pos.x, 2));

	  ang += rot;
	  v->pos.x = cos(ang) * norm;
	  v->pos.y = sin(ang) * norm;
	}
      v->pos.x += medmiddle->x + shift->x;
      v->pos.y += medmiddle->y + shift->y;
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

  srand(clock());
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
	t_bunny_picture *tmppic = bunny_new_picture(texture->buffer.width, texture->buffer.height);

	bunny_clear(&tmppic->buffer, TRANSPARENT);
	texmiddle.x = texture->buffer.width / 2;
	texmiddle.y = texture->buffer.height / 2;

	// On redessine la texture sur la texture avec le masque,
	// car le masque n'est pas pris en considération par set_geometry
	if (bunny_color_configuration("ColorMask", &texture->color_mask, tbox))
	  bunny_blit(&tmppic->buffer, texture, NULL);
	bunny_delete_clipable(texture);
	texture = tmppic;
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

  const char *shap = "Regular";
  double rnd = 10;
  double rnd2 = 0.1;
  bunny_configuration_getf(tbox, &shap, "Shape");
  bunny_configuration_getf(tbox, &rnd, "ShapeConfiguration[0]");
  bunny_configuration_getf(tbox, &rnd2, "ShapeConfiguration[1]");

  int			dynrot = 1;
  double		dynrotamp = 0;
  bool			dynrotalpha = false;
  t_bunny_picture	*curpic;

  bunny_configuration_getf(tbox, &dynrot, "DynamicRotation[0]");
  bunny_configuration_getf(tbox, &dynrotamp, "DynamicRotation[1]");
  bunny_configuration_getf(tbox, &dynrotalpha, "DynamicRotation[2]");
  dynrotamp = dynrotamp / 360.0 * (2 * M_PI);
  if (dynrotalpha)
    {
      if ((curpic = bunny_new_picture(pic->buffer.width, pic->buffer.height)) == NULL)
	return (EXIT_FAILURE);
      bunny_clear(&curpic->buffer, TRANSPARENT);
    }
  else
    curpic = pic;
  for (int dr = 1; dr <= dynrot; ++dr)
    {
      double		dra = (dynrot - dr) * dynrotamp;

      // D'abord on fait la forme en noir
      va->vertex[0].color = ALPHA(border_color.argb[ALPHA_CMP], BLACK);
      shape(curpic, corner, va, &medmiddle, &texmiddle, NULL,
	    va->vertex[0].color, 1.0, rot - dra,
	    &shift, &scale, shap, rnd, rnd2, tbox);
      
      // Ensuite on fait le boudin
      va->vertex[0].color = border_color.full;
      shape(curpic, corner, va, &medmiddle, &texmiddle, NULL,
	    border_color.full, 1.0 - 0.01, rot - dra,
	    &shift, &scale, shap, rnd, rnd2, tbox);
      
      // On fait l'interieur
      va->vertex[0].color = middle_color.full;
      shape(curpic, corner, va, &medmiddle, &texmiddle, texture,
	    inside_color.full, 1.0 - 0.01 - border_width, rot - dra,
	    &shift, &scale, shap, rnd, rnd2, tbox);

      if (curpic != pic)
	{
	  if (dynrot > 1)
	    curpic->color_mask.argb[ALPHA_CMP] = ((float)dr / dynrot) * 255;
	  else
	    curpic->color_mask.argb[ALPHA_CMP] = 255;
	  bunny_blit(&pic->buffer, curpic, NULL);
	  bunny_clear(&curpic->buffer, TRANSPARENT);
	}
    }
  if (pic != curpic)
    bunny_delete_clipable(curpic);

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
	  if (strcmp(fnt->string, "LABEL") == 0)
	    fnt->string = medal->name;
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
