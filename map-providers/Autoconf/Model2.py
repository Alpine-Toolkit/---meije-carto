####################################################################################################
#
# PyGeoPortail - A IGN GeoPortail Map Viewer
# Copyright (C) 2015 Fabrice Salvaire
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
####################################################################################################

""" See OpenGIS Web Map Tile Service Implementation Standard.
"""

####################################################################################################

import json

####################################################################################################

class JsonEncoder(json.JSONEncoder):

    ##############################################

    def default(self, obj):

        if isinstance(obj, JsonAble):
            return obj.__json_interface__

        # Let the base class default method raise the TypeError
        return json.JSONEncoder.default(self, obj)

####################################################################################################

class JsonAble(object):

    """This class implements a mixin for object which wan be converted to JSON."""

    ##############################################

    # def _jsonify(self, value):

    #     if isinstance(value, list):
    #         return [self._jsonify(x) for x in value]
    #     elif isinstance(value, JsonAble):
    #         return value.to_dict()
    #     else:
    #         return value

    ##############################################

    # def to_dict(self):

    #     d = {}
    #     for key, value in self.__dict__.items():
    #         d[key] = self._jsonify(value)
    #     return d

    ##############################################

    @property
    def __json_interface__(self):
        return {'_object':self.__class__.__name__, 'attributes':self.__dict__}

    ##############################################

    def to_json(self):

        kwargs = dict(cls=JsonEncoder, indent=2, ensure_ascii=False, sort_keys=True)
        return json.dumps(self, **kwargs)

####################################################################################################

class Autoconf(JsonAble):

    ##############################################

    def __init__(self, contents):

        self.contents = contents # Contents

####################################################################################################

class Contents(JsonAble):

    ##############################################

    def __init__(self, layers, tile_matrix_sets):

        self.layers = layers # [Layer]
        self.tile_matrix_sets = tile_matrix_sets # [TileMatrixSet]

####################################################################################################

class General(JsonAble):

    ##############################################

    def __init__(self, window, bounding_box, title, extension):

        self.window = window # Window
        self.bounding_box = bounding_box # BoundingBox
        self.title = title # str
        self.extension = extension # Extension

####################################################################################################

class Extension(JsonAble):

    ##############################################

    def __init__(self,
                 theme, default_GMLGFI_style_url, territories, tile_matrix_sets, resolutions, services):

        self.theme = theme # str
        self.default_GMLGFI_style_url = default_GMLGFI_style_url # str / url
        self.territories = territories # [Territory]
        self.tile_matrix_sets = tile_matrix_sets # [TileMatrixSet]
        self.resolutions = resolutions # [float]
        self.services = services # [Server]

####################################################################################################

class Territory(JsonAble):

    ##############################################

    def __init__(self,
                 default, id_, name,
                 default_crs, additional_crs,
                 bounding_box,
                 min_scale_denominator, max_scale_denominator,
                 resolution,
                 center,
                 default_layers):

        self.default = default
        self.id = id_
        self.name = name # str
        self.default_crs = default_crs
        self.additional_crs = additional_crs
        self.bounding_box = bounding_box # [float]
        self.min_scale_denominator = min_scale_denominator # float
        self.max_scale_denominator = max_scale_denominator # float
        self.resolution = resolution # float
        self.center = center # Center
        self.default_layers = default_layers

####################################################################################################

class TileMatrixSet(JsonAble):

    """A description of the geometry of a tile cut."""

    ##############################################

    def __init__(self, identifier, supported_crs, tile_matrices):

        # Tile matrix set identifier
        self.identifier = identifier # str
        # Reference to one coordinate reference system (CRS)
        self.supported_crs = supported_crs
        self.tile_matrices = tile_matrices # [TileMatrix]

####################################################################################################

class TileMatrix(JsonAble):

    ##############################################

    def __init__(self,
                 identifier, scale_denominator, top_left_corner,
                 tile_width, tile_height, matrix_width, matrix_height):

        # Tile matrix identifier
        self.identifier = identifier # int
        # Scale denominator level of this tile matrix
        self.scale_denominator = scale_denominator # float
        # Position in CRS coordinates of the top-left corner of this tile matrix
        self.top_left_corner = top_left_corner # float
        # Width of each tile of this tile matrix in pixels
        self.tile_width = tile_width # float
        self.tile_height = tile_height # float
        # Width of the matrix (number of tiles in width)
        self.matrix_width = matrix_width # float
        self.matrix_height = matrix_height # float

    ##############################################

    def __repr__(self):

        template = "[{}] @{} {}x{} tiles {}x{} px "
        return template.format(self.identifier,
                               self.scale_denominator, # * 0.28 mm
                               self.matrix_width, self.matrix_height,
                               self.tile_width, self.tile_height)

####################################################################################################

class Server(JsonAble):

    ##############################################

    def __init__(self, service, title, version, href):

        self.service = service # str
        self.title = title # str
        self.version = version
        self.href = href # str / url

    ##############################################

    def __repr__(self):

        return "{} {} {}".format(self.title, self.href, self.service)

####################################################################################################

class Format(JsonAble):

    """Supported valid output formats for a tile request.

    """

    ##############################################

    def __init__(self, current, name):

        self.current = current
        self.name = name

####################################################################################################

class Style(JsonAble):

    """Description of the style that has been applied to this layer.

    """

    ##############################################

    def __init__(self, current, name, title):

        self.current = current
        self.name = name
        self.title = title

####################################################################################################

class Dimension(JsonAble):

    """Extra dimensions for a tile and FeatureInfo resource requests.

    """

    ##############################################

    def __init__(self,
                 name, unit_symbol, units, user_value,
                 value):

        self.name = name
        # Symbol of the units
        self.unit_symbol = unit_symbol
        self.units = units
        self.user_value = user_value
        # Indicates an available value for this dimension
        self.value = value

####################################################################################################

class Legend(JsonAble):

    ##############################################

    def __init__(self, min_scale_denominator, url):

        self.min_scale_denominator = min_scale_denominator
        self.url = url

####################################################################################################

class Constraint(JsonAble):

    ##############################################

    def __init__(self, crs, bounding_box, min_scale_denominator, max_scale_denominator):

        self.crs = crs
        self.bounding_box = bounding_box
        self.min_scale_denominator = min_scale_denominator
        self.max_scale_denominator = max_scale_denominator

####################################################################################################

class Originator(JsonAble):

    ##############################################

    def __init__(self,
                 name,
                 attribution, logo, url, constraints):

        self.name = name
        self.attribution = attribution
        self.logo = logo
        self.url = url
        self.constraints = constraints

####################################################################################################

class Key(JsonAble):

    ##############################################

    def __init__(self, id_, url):

        self.id = id_
        self.url = url

####################################################################################################

class Layer(JsonAble):

    """Metadata describing one top-level dataset available from this server.

    """

    ##############################################

    def __init__(self,
                 hidden, queryable,
                 server, name, title, abstract,
                 min_scale_denominator, max_scale_denominator,
                 srs,
                 format_list, style_list, dimension_list, extension):

        self.hidden = hidden
        self.queryable = queryable
        self.server = server
        self.name = name
        # Title of this layer, normally used for display to a human
        self.title = title
        # Brief narrative description of this layer, normally available for display to a human
        self.abstract = abstract
        self.min_scale_denominator = min_scale_denominator
        self.max_scale_denominator = max_scale_denominator
        self.format_list = format_list # [Format]
        self.style_list = style_list # [Style]
        self.dimension_list = dimension_list # [Dimension]
        self.extension = extension
        self.srs = srs

####################################################################################################

class TileMatrixSetLink(JsonAble):

    ##############################################

    def __init__(self, name, limits):

        self.name = name # str
        self.limits = limits # [TileMatrixLimits]

####################################################################################################

class TileMatrixLimits(JsonAble):

    ##############################################

    def __init__(self, level, min_tile_row, max_tile_row, min_tile_col, max_tile_col):

        self.level = level
        self.min_tile_row = min_tile_row
        self.max_tile_row = max_tile_row
        self.min_tile_col = min_tile_col
        self.max_tile_col = max_tile_col

####################################################################################################

class ExtensionLayer(JsonAble):

    ##############################################

    def __init__(self,
                 id_,
                 constraints, thematics, inspire_thematics,
                 bounding_box, additional_crs, originators, legends,
                 quicklook, tile_matrix_set_link, metadata_url, keys):

        self.id = id_
        self.constraints = constraints
        self.thematics = thematics
        self.inspire_thematics = inspire_thematics
        self.bounding_box = bounding_box
        self.additional_crs = additional_crs
        self.originators = originators
        self.legends = legends
        self.quicklook = quicklook
        self.tile_matrix_set_link = tile_matrix_set_link # TileMatrixSetLink
        self.metadata_url = metadata_url
        self.keys = keys

####################################################################################################

class Window(JsonAble):

    ##############################################

    def __init__(self, width, height):

        self.width = width
        self.height = height

####################################################################################################

class BoundingBox(JsonAble):

    ##############################################

    def __init__(self, srs, x_min, y_min, x_max, y_max):

        self.srs = srs
        self.x_min = x_min
        self.y_min = y_min
        self.x_max = x_max
        self.y_max = y_max

    ##############################################

    def __repr__(self):

        return "Bounding Box: {} [{}, {}]*[{}, {}]".format(self.srs,
                                                           self.x_min, self.x_max,
                                                           self.y_min, self.y_max)

####################################################################################################

class Center(JsonAble):

    ##############################################

    def __init__(self, x, y):

        self.x = x
        self.y = y

####################################################################################################
#
# End
#
####################################################################################################
