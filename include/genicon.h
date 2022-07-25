/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2021
** Pentacle Technologie 2008-2022
** EFRITS SAS 2022-2022
**
** Medal Generator
*/

#ifndef			__GENICON_H__
# define		__GENICON_H__
# include		<stdio.h>
# include		<lapin.h>

typedef struct		s_medal
{
  const char		*name;
  const char		*picfile;
  const char		*texfile;
  const char		*specificator;
  const char		*label;
}			t_medal;

int			generate_icon(t_bunny_configuration	*cnf,
				      const char		*type,
				      t_medal			*medal);
int			band_medal(t_bunny_configuration	*cnf,
				   t_bunny_picture		*pic,
				   t_medal			*medal);
int			round_medal(t_bunny_configuration	*cnf,
				    t_bunny_picture		*pic,
				    t_medal			*medal);

extern unsigned int	gl_colors[6];

#endif	/*	__GENICON_H__	*/
