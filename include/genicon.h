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

int			generate_icon(t_bunny_configuration	*cnf,
				      const char		*type,
				      const char		*medal_name,
				      const char		*pic,
				      const char		*specificator);

int			band_medal(t_bunny_configuration	*cnf,
				   t_bunny_picture		*pic,
				   const char			*medal_name,
				   const char			*spec);
int			medal(t_bunny_configuration		*cnf,
			      t_bunny_picture			*pic,
			      const char			*picfile,
			      const char			*spec);

extern unsigned int	gl_colors[6];

#endif	/*	__GENICON_H__	*/
