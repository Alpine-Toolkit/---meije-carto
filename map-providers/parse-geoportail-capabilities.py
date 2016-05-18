####################################################################################################

from Autoconf.AutoconfParser import AutoconfParser

####################################################################################################

xml_path = 'countries/france/capabilities.xml'
with open(xml_path) as f:
    xml_document = f.read()
xml_document = bytes(xml_document, encoding='utf8')

autoconf_parser = AutoconfParser()
autoconf = autoconf_parser.parse_document(xml_document)

# print(autoconf.to_json())

general = autoconf.general
print(general.title)
print(general.bounding_box)

extension = general.extension
print(extension.resolutions)

print("\nTerritories")
for territory in extension.territories:
    print(territory.name,
          territory.bounding_box,
          territory.resolution, territory.min_scale_denominator)

print("\nTile Matrix Sets")
for tile_matrix_set in extension.tile_matrix_sets:
    print(tile_matrix_set.identifier)
    for tile_matrix in sorted(tile_matrix_set.tile_matrices, key=lambda x: x.identifier):
        print(tile_matrix)

print("\nServices")
for service in extension.services:
    print(service)

print("\nLayers")
for layer in sorted(autoconf.layers, key=lambda x: x.name):
    print('-'*50)
    print(layer.name)
    print(layer.title)
    print(layer.server)

####################################################################################################
#
# End
#
####################################################################################################
