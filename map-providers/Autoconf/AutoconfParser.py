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

####################################################################################################

from .XmlParser import XmlParser, QXmlStreamReader
from .Model import *

####################################################################################################

def int_attr(attr, key):
    return int(attr[key])

def float_attr(attr, key):
    return float(attr[key])

####################################################################################################

class AutoconfParser(XmlParser):

    ##############################################

    def parser_loop(self):

        # <?xml version="1.0" encoding="UTF-8"?>
        # <ViewContext
        #     xmlns="http://www.opengis.net/context"
        #     xmlns:gpp="http://api.ign.fr/geoportail"
        #     xmlns:ows="http://www.opengis.net/ows/1.1"
        #     xmlns:sld="http://www.opengis.net/sld"
        #     xmlns:wmts="http://www.opengis.net/wmts/1.0"
        #     xmlns:xlink="http://www.w3.org/1999/xlink"
        #     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        #     id="autoConf"
        #     version="1.1.0"
        #     xsi:schemaLocation="http://www.opengis.net/context
        #                         http://gpp3-wxs.ign.fr/schemas/extContext.xsd
        #                         http://api.ign.fr/geoportail
        #                         http://wxs.ign.fr/schemas/autoconf/autoconf.xsd">
        #   <General> ...  </General>
        #   <LayerList> ...  </LayerList>
        # </ViewContext>

        if self._xml_parser.readNext() != QXmlStreamReader.StartDocument:
            self._raise()
        if not self._read_match_start_element('ViewContext'):
            self._raise()
        
        # while self._xml_parser.readNextStartElement():
        #     ename = self._xml_parser.name()
        #     if ename == 'General':
        #         self._parse_General(autoconf.general)
        #     elif ename == 'LayerList':
        #         self._parse_LayerList(autoconf.layers)
        #     else:
        #         self._xml_parser.skipCurrentElement()
        #         self._raise()
        
        while not self._read_match_end_element('Capabilities'):
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'General':
                    general = self._parse_General()
                elif ename == 'LayerList':
                    layers = self._parse_LayerList()
                else:
                    self._raise()
            # else
            # or match end and
            # elif not self._match_empty():
            #     self._raise()
        
        if self._xml_parser.readNext() != QXmlStreamReader.EndDocument:
            self._raise()
        
        return Autoconf(general, layers)

    ##############################################

    def _parse_General(self):

        # <General>
        #   <Window height="300" width="500"/>
        #   <BoundingBox SRS="EPSG:4326" maxx="180.0" maxy="90.0" minx="-90.0" miny="-180.0"/>
        #   <Title>Service d'autoconfiguration des API</Title>
        #   <Extension> ... </Extension>
        # </General>

        # dispatched
        while not self._read_match_end_element('General'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Window':
                    window = self._parse_Window()
                elif ename == 'BoundingBox':
                    bounding_box = self._parse_BoundingBox()
                elif ename == 'Title':
                    title = self._read_text()
                elif ename == 'Extension':
                    extension = self._parse_Extension()
                else:
                    self._raise()
            # else
        
        return General(window, bounding_box, title, extension)

    ##############################################

    def _parse_Window(self):

        # <Window height="300" width="500"/>

        # dispatched
        attr = self._attribute_to_dict('height', 'width')
        return Window(int_attr(attr, 'height'), int_attr(attr, 'height'))

    ##############################################

    def _parse_BoundingBox(self):

        # <BoundingBox SRS="EPSG:4326" maxx="180.0" maxy="90.0" minx="-90.0" miny="-180.0"/>

        # dispatched
        attr = self._attribute_to_dict('SRS', 'maxx', 'maxy', 'minx', 'miny')
        return BoundingBox(srs=attr['SRS'],
                           x_min=float_attr(attr, 'minx'),
                           y_min=float_attr(attr, 'miny'),
                           x_max=float_attr(attr, 'maxx'),
                           y_max=float_attr(attr, 'maxy'))

    ##############################################

    def _parse_Extension(self):

        # <Extension>
        #   <gpp:General>
        #     <gpp:Theme>default</gpp:Theme>
        #     <gpp:defaultGMLGFIStyleUrl>http://wxs.ign.fr/static/style_wms_getfeatureinfo/default.xsl</gpp:defaultGMLGFIStyleUrl>
        #     <gpp:Territories> ... </gpp:Territories>
        #     <gpp:TileMatrixSets> ... </gpp:TileMatrixSets>
        #     <gpp:Resolutions> ... </gpp:Resolutions>
        #     <gpp:Services>
        #     </gpp:Services>
        #   </gpp:General>
        # </Extension>

        # dispatched
        self._read_until_empty()
        if not self._match_start_element('General'):
            self._raise()
        
        while not self._read_match_end_element('General'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement:
                ename = self._xml_parser.name()
                if ename == 'Theme':
                    theme = self._read_text()
                elif ename == 'defaultGMLGFIStyleUrl':
                    defaultGMLGFIStyleUrl = self._read_text()
                elif ename == 'Territories':
                    territories = self._parse_Territories()
                elif ename == 'TileMatrixSets':
                    tile_matrix_sets = self._parse_TileMatrixSets()
                elif ename == 'Resolutions':
                    resolutions = self._parse_Resolutions()
                elif ename == 'Services':
                    services = self._parse_Services()
                else:
                    self._raise()
            # else
        
        self._read_until_empty()
        if not self._match_end_element('Extension'):
            self._raise()
        
        return Extension(theme, defaultGMLGFIStyleUrl, territories, tile_matrix_sets, resolutions, services)

    ##############################################

    def _parse_Territories(self):

        # <gpp:Territories>
	#   <gpp:Territory default="1" id="FXX" name="FXX"> ... </gpp:Territory>
        # </gpp:Territories>

        # dispatched
        territories = []
        while not self._read_match_end_element('Territories'):
            if self._match_empty():
                continue
            territories.append(self._parse_Territory())
        return territories

    ##############################################

    def _parse_Territory(self):

	# <gpp:Territory default="1" id="FXX" name="FXX">
	#   <gpp:defaultCRS>EPSG:3857</gpp:defaultCRS>
	#   <gpp:AdditionalCRS>CRS:84</gpp:AdditionalCRS>
	#   <gpp:AdditionalCRS>IGNF:RGF93G</gpp:AdditionalCRS>
        #   ...
        #   <gpp:BoundingBox>-31.17,27.33,69.03,80.83</gpp:BoundingBox>
	#   <sld:MinScaleDenominator>533</sld:MinScaleDenominator>
	#   <sld:MaxScaleDenominator>128209039</sld:MaxScaleDenominator>
	#   <gpp:Resolution>2445.984905</gpp:Resolution>
	#   <gpp:Center> ... </gpp:Center>
	#   <gpp:DefaultLayers> ... </gpp:DefaultLayers>
	# </gpp:Territory>

        if not self._match_start_element('Territory'):
            self._raise()
        
        attr = self._attribute_to_dict('default', 'id', 'name')
        default = attr['default']
        id_ = attr['id']
        name = attr['name']
        additional_crs = []
        
        while not self._read_match_end_element('Territory'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'defaultCRS':
                    default_crs = self._read_text()
                elif ename == 'AdditionalCRS':
                    additional_crs.append(self._read_text())
                elif ename == 'BoundingBox':
                    bounding_box = self._read_float_list(sep=',')
                elif ename == 'MinScaleDenominator':
                    min_scale_denominator = self._read_float()
                elif ename == 'MaxScaleDenominator':
                    max_scale_denominator = self._read_float()
                elif ename == 'Resolution':
                    resolution = self._read_float()
                elif ename == 'Center':
                    center = self._parse_Center()
                elif ename == 'DefaultLayers':
                    default_layers = self._parse_DefaultLayers()
                else:
                    self._raise()
            # else
        
        return Territory(default, id_, name,
                         default_crs, additional_crs,
                         bounding_box,
                         min_scale_denominator, max_scale_denominator,
                         resolution,
                         center,
                         default_layers)

    ##############################################

    def _parse_Center(self):

	#   <gpp:Center>
	#     <gpp:x>2.345274398</gpp:x>
	#     <gpp:y>48.860832558</gpp:y>
	#   </gpp:Center>

        # dispatched
        while not self._read_match_end_element('Center'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'x':
                    x = self._read_float()
                elif ename == 'y':
                    y = self._read_float()
                else:
                    self._raise()
            # else
        
        return Center(x, y)

    ##############################################

    def _parse_DefaultLayers(self):

	#   <gpp:DefaultLayers>
	#     <gpp:DefaultLayer layerId="ORTHOIMAGERY.ORTHOPHOTOS$GEOPORTAIL:OGC:WMTS"/>
	#     <gpp:DefaultLayer layerId="GEOGRAPHICALGRIDSYSTEMS.MAPS$GEOPORTAIL:OGC:WMTS"/>
        #     ...
	#   </gpp:DefaultLayers>

        default_layers = []
        while not self._read_match_end_element('DefaultLayers'):
            if self._match_empty():
                continue
            # if self._xml_parser.isStartElement():
            #     default_layers.append(self._parse_DefaultLayer())
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'DefaultLayer':
                    attr = self._attribute_to_dict('layerId')
                    default_layers.append(attr['layerId'])
                else:
                    self._raise()
            # else
        return default_layers

    ##############################################

    # def _parse_DefaultLayer(self):

    #     # dispatched
    #     if not self._match_start_element('DefaultLayer'):
    #         self._raise()
    #     default_layer = self._attribute_to_dict('layerId')
    #     if not self._read_match_end_element('DefaultLayer'):
    #         self._raise()
    #     return default_layer

    ##############################################

    def _parse_TileMatrixSets(self):

	# <gpp:TileMatrixSets>
	#   <wmts:TileMatrixSet> ... </wmts:TileMatrixSet>
        #   ...
        # </gpp:TileMatrixSets>

        tile_matrix_sets = []
        while not self._read_match_end_element('TileMatrixSets'):
            if self._match_empty():
                continue
            tile_matrix_sets.append(self._parse_TileMatrixSet())
        return tile_matrix_sets

    ##############################################

    def _parse_TileMatrixSet(self):

	# <wmts:TileMatrixSet>
	#   <ows:Identifier>PM</ows:Identifier>
	#   <ows:SupportedCRS>EPSG:3857</ows:SupportedCRS>
	#   <wmts:TileMatrix> ... </wmts:TileMatrix>
        # ...
        # </wmts:TileMatrixSet>

        if not self._match_start_element('TileMatrixSet'):
            self._raise()
        
        tile_matrices = []
        while not self._read_match_end_element('TileMatrixSet'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Identifier':
                    identifier = self._read_text()
                elif ename == 'SupportedCRS':
                    supported_crs = self._read_text()
                elif ename == 'TileMatrix':
                    tile_matrices.append(self._parse_TileMatrix())
                else:
                    self._raise()
            # else
        
        return TileMatrixSet(identifier, supported_crs, tile_matrices)

    ##############################################

    def _parse_TileMatrix(self):

        # <wmts:TileMatrix>
	#   <ows:Identifier>0</ows:Identifier>
	#   <wmts:ScaleDenominator>559082264.0287178958533332</wmts:ScaleDenominator>
	#   <wmts:TopLeftCorner>-20037508 20037508</wmts:TopLeftCorner>
	#   <wmts:TileWidth>256</wmts:TileWidth>
	#   <wmts:TileHeight>256</wmts:TileHeight>
	#   <wmts:MatrixWidth>1</wmts:MatrixWidth>
	#   <wmts:MatrixHeight>1</wmts:MatrixHeight>
	# </wmts:TileMatrix>

        while not self._read_match_end_element('TileMatrix'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Identifier':
                    identifier = self._read_int()
                elif ename == 'ScaleDenominator':
                    scale_denominator = self._read_float()
                elif ename == 'TopLeftCorner':
                    top_left_corner = self._read_int_list(sep=' ')
                elif ename == 'TileWidth':
                    tile_width = self._read_int()
                elif ename == 'TileHeight':
                    tile_height = self._read_int()
                elif ename == 'MatrixWidth':
                    matrix_width = self._read_int()
                elif ename == 'MatrixHeight':
                    matrix_height = self._read_int()
                else:
                    self._raise()
            # else
        
        return TileMatrix(identifier, scale_denominator, top_left_corner,
                          tile_width, tile_height, matrix_width, matrix_height)

    ##############################################

    def _parse_Resolutions(self):

        return self._read_float_list(sep=',')

    ##############################################

    def _parse_Services(self):

	# <gpp:Services>
	#   <Server service="OGC:OPENLS;Geocode" title="Service de Geocodage" version="1.2"> ... </Server>
        #   ...
        # <gpp:Services>

        services = []
        while not self._read_match_end_element('Services'):
            if self._match_empty():
                continue
            services.append(self._parse_Server())
        return services

    ##############################################

    def _parse_Server(self):

	#   <Server service="OGC:OPENLS;Geocode" title="Service de Geocodage" version="1.2">
	#     <OnlineResource xlink:href="http://wxs.ign.fr/geoportail/ols" xlink:type="simple"/>
	#   </Server>

        if not self._match_start_element('Server'):
            self._raise()
        attr = self._attribute_to_dict('service', 'title', 'version')
        
        service = attr['service']
        title = attr['title']
        version = attr['version']
        
        while not self._read_match_end_element('Server'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'OnlineResource':
                    attr = self._attribute_to_dict('xlink:href')
                    href = attr['xlink:href']
                else:
                    self._raise()
            # else
        
        return Server(service, title, version, href)

    ##############################################

    def _parse_LayerList(self):

        # <LayerList>
        #   <Layer hidden="1" queryable="1"> ... </Layer>
        #   ...
        # </LayerList>

        layer_list = []
        while not self._read_match_end_element('LayerList'):
            if self._match_empty():
                continue
            layer_list.append(self._parse_Layer())
        return layer_list

    ##############################################

    def _parse_Layer(self):

        # <Layer hidden="1" queryable="1">
        #   <Server service="OGC:WMS" title="Monuments nationaux" version="1.3.0"> ... </Server>
        #   <Name>POI.MONUMENTS_BDD_WLD_WM</Name>
        #   <Title>Monuments nationaux</Title>
        #   <Abstract><![CDATA[Le Centre des monuments nationaux ...]]></Abstract>
        #   <sld:MinScaleDenominator>0</sld:MinScaleDenominator>
        #   <sld:MaxScaleDenominator>62236752975597</sld:MaxScaleDenominator>
        #   <SRS>EPSG:4326</SRS>
        #   <FormatList> ... </FormatList>
        #   <StyleList> ... </StyleList>
        #   <DimensionList> ... </DimensionList>
        #   <Extension> ... </Extension>
        # </Layer>

        if not self._match_start_element('Layer'):
            self._raise()
        
        attr = self._attribute_to_dict('hidden', 'queryable')
        hidden = bool(attr['hidden'])
        queryable = bool(attr['queryable'])
        
        min_scale_denominator = None
        max_scale_denominator = None
        srs = None
        dimension_list = None
        format_list = None
        style_list = None
        
        while not self._read_match_end_element('Layer'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Server':
                    server = self._parse_Server()
                elif ename == 'Name':
                    name = self._read_text()
                elif ename == 'Title':
                    title = self._read_text()
                elif ename == 'Abstract':
                    abstract = self._read_text()
                elif ename == 'MinScaleDenominator':
                    min_scale_denominator = self._read_float()
                elif ename == 'MaxScaleDenominator':
                    max_scale_denominator = self._read_float()
                elif ename == 'SRS':
                    srs = self._read_text()
                elif ename == 'FormatList':
                    format_list = self._parse_FormatList()
                elif ename == 'StyleList':
                    style_list = self._parse_StyleList()
                elif ename == 'DimensionList':
                    dimension_list = self._parse_DimensionList()
                elif ename == 'Extension':
                    extension = self._parse_Layer_Extension()
                else:
                    self._raise()
            # else
        return Layer(hidden, queryable,
                     server, name, title, abstract,
                     min_scale_denominator, max_scale_denominator,
                     srs,
                     format_list, style_list, dimension_list, extension)

    ##############################################

    def _parse_FormatList(self):

        # <FormatList>
        #   <Format current="1">text/xml</Format>
        #   ...
        # </FormatList>

        format_list = []
        while not self._read_match_end_element('FormatList'):
            if self._match_empty():
                continue
            format_list.append(self._parse_Format())
        return format_list

    ##############################################

    def _parse_Format(self):

        if not self._match_start_element('Format'):
            self._raise()
        
        attr = self._attribute_to_dict('current')
        current = attr['current']
        name = self._read_text()
        
        return Format(current, name)

    ##############################################

    def _parse_StyleList(self):

        # <StyleList>
        #   <Style current="1"> ... </Style>
        #   ...
        # <StyleList>

        style_list = []
        while not self._read_match_end_element('StyleList'):
            if self._match_empty():
                continue
            style_list.append(self._parse_Style())
        return style_list

    ##############################################

    def _parse_Style(self):

        #   <Style current="1">
        #     <Name>normal</Name>
        #     <Title>Données Brutes</Title>
        #   </Style>

        if not self._match_start_element('Style'):
            self._raise()
        attr = self._attribute_to_dict('current')
        current = attr['current']
        
        while not self._read_match_end_element('Style'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Name':
                    name = self._read_text()
                elif ename == 'Title':
                    title = self._read_text()
                else:
                    self._raise()
            # else
        
        return Style(current, name, title)

    ##############################################

    def _parse_DimensionList(self):

        # <DimensionList>
        #   <Dimension name="GeometricType" unitSymbol="" units="" userValue="">-</Dimension>
        # </DimensionList>

        dimension_list = []
        while not self._read_match_end_element('DimensionList'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Dimension':
                    dimension_list.append(self._parse_Dimension())
                else:
                    self._raise()
            # else
        return dimension_list

    ##############################################

    def _parse_Dimension(self):

        # <Dimension name="GeometricType" unitSymbol="" units="" userValue="">-</Dimension>

        if not self._match_start_element('Dimension'):
            self._raise()
        
        attr = self._attribute_to_dict('name', 'unitSymbol', 'units', 'userValue')
        name = attr['name']
        unit_symbol = attr['unitSymbol']
        units = attr['units']
        user_value = attr['userValue']
        
        value = self._read_text()
        
        return Dimension(name, unit_symbol, units, user_value,
                         value)

    ##############################################

    def _parse_Layer_Extension(self):


        # <Extension>
        #   <gpp:Layer id="BDPARCEL_PYR-PNG_WLD$GEOPORTAIL:OGC:WMS">
        #     <gpp:Constraints> ... </gpp:Constraints>
        #     <gpp:Thematics> ... </gpp:Thematics>
        #     <gpp:InspireThematics> ... </gpp:InspireThematics>
        #     <gpp:BoundingBox maxT="2015-08-24" minT="2015-08-24">-63.160706,-21.39223,55.84643,51.090965</gpp:BoundingBox>
        #     <gpp:AdditionalCRS>EPSG:2975</gpp:AdditionalCRS>
        #     <gpp:AdditionalCRS>EPSG:3727</gpp:AdditionalCRS>
        #     ...
        #     <gpp:Originators> ... </gpp:Originators>
        #     <gpp:Legends> ... </gpp:Legends>
        #     <gpp:QuickLook> ... </gpp:QuickLook>
        #     <wmts:TileMatrixSetLink> ... </wmts:TileMatrixSetLink>
        #     <gpp:MetadataURL format="xml"> ... </gpp:MetadataURL>
        #     <gpp:Keys> ... </gpp:Keys>
        #   </gpp:Layer>
        # </Extension>

        if not self._match_start_element('Extension'):
            self._raise()
        self._read_until_empty()
        if not self._match_start_element('Layer'):
            self._raise()
        
        attr = self._attribute_to_dict('id')
        id_ = bool(attr['id'])
        
        bounding_box = None
        constraints = None
        inspire_thematics = None
        legends = None
        metadata_url = None
        originators = None
        tile_matrix_set_link = None
        quicklook = None
        additional_crs = []
        
        while not self._read_match_end_element('Layer'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Constraints':
                    constraints = self._parse_Constraints()
                elif ename == 'Thematics':
                    thematics = self._parse_Thematics()
                elif ename == 'InspireThematics':
                    inspire_thematics = self._parse_InspireThematics()
                elif ename == 'BoundingBox':
                    bounding_box = self._parse_LayerBoundingBox()
                elif ename == 'AdditionalCRS':
                    additional_crs.append(self._read_text())
                elif ename == 'Originators':
                    originators = self._parse_Originators()
                elif ename == 'Legends':
                    legends = self._parse_Legends()
                elif ename == 'QuickLook':
                    quicklook = self._parse_Quicklook()
                elif ename == 'TileMatrixSetLink':
                    tile_matrix_set_link = self._parse_TileMatrixSetLink()
                elif ename == 'MetadataURL':
                    metadata_url = self._parse_OnlineResource() # MetadataURL
                elif ename == 'Keys':
                    keys = self._parse_Keys()
                else:
                    self._raise()
            # else
        
        self._read_until_empty()
        if not self._match_end_element('Extension'):
            self._raise()
        
        return ExtensionLayer(id_,
                              constraints, thematics, inspire_thematics,
                              bounding_box, additional_crs, originators, legends,
                              quicklook, tile_matrix_set_link, metadata_url, keys)

    ##############################################

    def _parse_Thematics(self):

        # <gpp:Thematics>
        #   <gpp:Thematic>Parcelles cadastrales</gpp:Thematic>
        # </gpp:Thematics>

        thematics = []
        while not self._read_match_end_element('Thematics'):
            if self._match_empty():
                continue
            thematics.append(self._read_text())
        return thematics

    ##############################################

    def _parse_InspireThematics(self):

        # <gpp:InspireThematics>
        #   <gpp:InspireThematic>Parcelles cadastrales</gpp:InspireThematic>
        # </gpp:InspireThematics>

        inspire_thematics = []
        while not self._read_match_end_element('InspireThematics'):
            if self._match_empty():
                continue
            inspire_thematics.append(self._read_text())
        return inspire_thematics

    ##############################################

    def _parse_LayerBoundingBox(self):

        attr = self._attribute_to_dict('maxT', 'minT')
        max_time = attr['maxT']
        min_time = attr['minT']
        bounding_box = self._read_float_list(sep=',')
        return bounding_box # Fixme:

    ##############################################

    def _parse_Originators(self):

        # <gpp:Originators>
        #   <gpp:Originator name="IGN"> ... </gpp:Originator>
        # </gpp:Originators>

        originators = []
        while not self._read_match_end_element('Originators'):
            if self._match_empty():
                continue
            originators.append(self._parse_Originator())
        return originators

    ##############################################

    def _parse_Originator(self):

        # <gpp:Originator name="IGN">
        #   <gpp:Attribution>Institut national de l'information géographique et forestière</gpp:Attribution>
        #   <gpp:Logo>http://wxs.ign.fr/static/logos/IGN/IGN.gif</gpp:Logo>
        #   <gpp:URL>http://www.ign.fr</gpp:URL>
        #   <gpp:Constraints> ... </gpp:Constraints>
        # </gpp:Originator>

        if not self._match_start_element('Originator'):
            self._raise()
        
        attr = self._attribute_to_dict('name')
        name = attr['name']
        
        while not self._read_match_end_element('Originator'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'Attribution':
                    attribution = self._read_text()
                elif ename == 'Logo':
                    logo = self._read_text()
                elif ename == 'URL':
                    url = self._read_text()
                elif ename == 'Constraints':
                    constraints = self._parse_Constraints()
                else:
                    self._raise()
            # else
        
        return Originator(name,
                          attribution, logo, url, constraints)

    ##############################################

    def _parse_Constraints(self):

        # <gpp:Constraints>
        #   <gpp:Constraint> ... </gpp:Constraint>
        # </gpp:Constraints>

        constraints = []
        while not self._read_match_end_element('Constraints'):
            if self._match_empty():
                continue
            constraints.append(self._parse_Constraint())
        return constraints

    ##############################################

    def _parse_Constraint(self):

        # <gpp:Constraint>
        #   <gpp:CRS>EPSG:4326</gpp:CRS>
        #   <gpp:BoundingBox maxT="2015-08-24" minT="2015-08-24">-63.160706,-21.39223,55.84643,51.090965</gpp:BoundingBox>
        #   <sld:MinScaleDenominator>69885284</sld:MinScaleDenominator>
        #   <sld:MaxScaleDenominator>69885284</sld:MaxScaleDenominator>
        # </gpp:Constraint>

        if not self._match_start_element('Constraint'):
            self._raise()
        
        while not self._read_match_end_element('Constraint'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'CRS':
                    crs = self._read_text()
                elif ename == 'BoundingBox':
                    bounding_box = self._parse_LayerBoundingBox()
                elif ename == 'MinScaleDenominator':
                    min_scale_denominator = self._read_int()
                elif ename == 'MaxScaleDenominator':
                    max_scale_denominator = self._read_int()
                else:
                    self._raise()
            # else
        
        return Constraint(crs, bounding_box, min_scale_denominator, max_scale_denominator)

    ##############################################

    def _parse_Legends(self):

        # <gpp:Legends>
        #   <gpp:Legend> ... </gpp:Legend>
        # </gpp:Legends>

        legends = []
        while not self._read_match_end_element('Legends'):
            if self._match_empty():
                continue
            legends.append(self._parse_Legend())
        return legends

    ##############################################

    def _parse_Legend(self):

        #   <gpp:Legend>
        #     <sld:MinScaleDenominator>534</sld:MinScaleDenominator>
        #     <gpp:LegendURL format="format">
        #       <OnlineResource xlink:href="http://wxs.ign.fr/static/legends/NOLEGEND.JPG" xlink:type="simple"/>
        #     </gpp:LegendURL>
        #   </gpp:Legend>

        if not self._match_start_element('Legend'):
            self._raise()
        
        while not self._read_match_end_element('Legend'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'MinScaleDenominator':
                    min_scale_denominator = self._read_int()
                elif ename == 'LegendURL':
                    url = self._parse_OnlineResource()
                else:
                    self._raise()
            # else
        
        return Legend(min_scale_denominator, url)

    ##############################################

    def _parse_Quicklook(self):

        # <gpp:QuickLook>
        #   <OnlineResource xlink:href="http://wxs.ign.fr/static/pictures/BDPARCELLAIRE.png" xlink:type="simple"/>
        # </gpp:QuickLook>

        self._read_until_empty()
        if not self._match_start_element('OnlineResource'):
            self._raise()
        attr = self._attribute_to_dict('href', 'type')
        return attr['href'] # type

    ##############################################

    def _parse_OnlineResource(self):

        # MetadataURL
        # LegendURL

        # <gpp:MetadataURL format="xml">
        #   <OnlineResource xlink:href="http://wxs.ign.fr/geoportail/csw?service=CSW&amp;version=2.0.2&amp;request=GetRecordById&amp;Id=IGNF_BDPARCELLAIREr_1-2_image.xml" xlink:type="simple"/>
        # </gpp:MetadataURL>

        attr = self._attribute_to_dict('format')
        self._read_until_empty()
        if not self._match_start_element('OnlineResource'):
            self._raise()
        attr = self._attribute_to_dict('href', 'type')
        return attr['href'] # format type

    ##############################################

    def _parse_Keys(self):

        # <gpp:Keys>
        #   <gpp:Key id="algzhye2iogn8fvb0nkgf0zx">http://wxs.ign.fr/algzhye2iogn8fvb0nkgf0zx/geoportail/r/wms</gpp:Key>
        # </gpp:Keys>

        keys = []
        while not self._read_match_end_element('Keys'):
            if self._match_empty():
                continue
            url = self._read_text()
            attr = self._attribute_to_dict('id')
            id_ = attr['id']
            keys.append(Key(url, id_))
        return keys

    ##############################################

    def _parse_TileMatrixSetLink(self):

        # <wmts:TileMatrixSetLink>
        #   <wmts:TileMatrixSet>PM</wmts:TileMatrixSet>
        #   <wmts:TileMatrixSetLimits> ... </wmts:TileMatrixLimits>
        #     ...
        #   </wmts:TileMatrixSetLimits>
        # </wmts:TileMatrixSetLink>

        if not self._match_start_element('TileMatrixSetLink'):
            self._raise()
        
        while not self._read_match_end_element('TileMatrixSetLink'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'TileMatrixSet':
                    name = self._read_text()
                elif ename == 'TileMatrixSetLimits':
                    limits = self._parse_TileMatrixSetLimits()
                else:
                    self._raise()
            # else
        
        return TileMatrixSetLink(name, limits)

    ##############################################

    def _parse_TileMatrixSetLimits(self):

        limits = []
        while not self._read_match_end_element('TileMatrixSetLimits'):
            if self._match_empty():
                continue
            limits.append(self._parse_TileMatrixLimits())
        return limits

    ##############################################

    def _parse_TileMatrixLimits(self):

        # <wmts:TileMatrixLimits>
        #   <wmts:TileMatrix>10</wmts:TileMatrix>
        #   <wmts:MinTileRow>342</wmts:MinTileRow>
        #   <wmts:MaxTileRow>574</wmts:MaxTileRow>
        #   <wmts:MinTileCol>332</wmts:MinTileCol>
        #   <wmts:MaxTileCol>670</wmts:MaxTileCol>
        # </wmts:TileMatrixLimits>

        if not self._match_start_element('TileMatrixLimits'):
            self._raise()
        
        while not self._read_match_end_element('TileMatrixLimits'):
            if self._match_empty():
                continue
            if self._xml_parser.isStartElement():
                ename = self._xml_parser.name()
                if ename == 'TileMatrix':
                    level = self._read_int()
                elif ename == 'MinTileRow':
                    min_tile_row = self._read_int()
                elif ename == 'MaxTileRow':
                    max_tile_row = self._read_int()
                elif ename == 'MinTileCol':
                    min_tile_col = self._read_int()
                elif ename == 'MaxTileCol':
                    max_tile_col = self._read_int()
                else:
                    self._raise()
            # else
        
        return TileMatrixLimits(level, min_tile_row, max_tile_row, min_tile_col, max_tile_col)

####################################################################################################
#
# End
#
####################################################################################################
