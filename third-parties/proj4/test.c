/***************************************************************************************************
**
** $QTCARTO_BEGIN_LICENSE:GPL3$
**
** Copyright (C) 2016 Fabrice Salvaire
** Contact: http://www.fabrice-salvaire.fr
**
** This file is part of the QtCarto library.
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
** $QTCARTO_END_LICENSE$
**
***************************************************************************************************/

/**************************************************************************************************/

#include <stdio.h>
#include "proj_api.h"

/**************************************************************************************************/

int
main(int argc, char *argv[])
{
  projCtx context = pj_get_default_ctx();
  // context = pj_ctx_alloc();

  projPJ projection_4326 = pj_init_plus_ctx(context, "+init=epsg:4326");
  projPJ projection_3857 = pj_init_plus_ctx(context, "+init=epsg:3857");

  // projPJ projection_4326 = pj_init_plus_ctx(context, "+proj=longlat +datum=WGS84 +no_defs");
  // projPJ projection_3857 = pj_init_plus_ctx(context, "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");

  long point_count = 1;
  int point_offset = 1;
  double x = .1; // rad
  double y = .2;
  double z = 0;
  printf("input: %e %e\n", x, y, z);
  int error = pj_transform(projection_4326, projection_3857, point_count, point_offset, &x, &y, &z);
  if (error == 0)
    printf("output: %e %e\n", x, y, z);
  else
    printf("Transform failed: %i %s\n", error, pj_strerrno(error));

  pj_free(projection_4326);
  pj_free(projection_3857);

  return 0;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
