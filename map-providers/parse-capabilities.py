####################################################################################################

import argparse

####################################################################################################

from Autoconf.AutoconfParser2 import AutoconfParser

####################################################################################################

argument_parser = argparse.ArgumentParser(description='')

argument_parser.add_argument('xml_path', metavar='capbilities.xml',
                             help='XMl file')

args = argument_parser.parse_args()

####################################################################################################

with open(args.xml_path) as f:
    xml_document = f.read()
xml_document = bytes(xml_document, encoding='utf8')

autoconf_parser = AutoconfParser()
autoconf = autoconf_parser.parse_document(xml_document)

# print(autoconf.to_json())

# contents = autoconf.contents

# print("\nTile Matrix Sets")
# for tile_matrix_set in contents.tile_matrix_sets:
#     print('-'*25)
#     print(tile_matrix_set.identifier)
#     for tile_matrix in sorted(tile_matrix_set.tile_matrices, key=lambda x: x.identifier):
#         print(tile_matrix)

# print("\nLayers")
# for layer in contents.layers:
#     print('-'*50)
#     print(layer.title)

####################################################################################################
#
# End
#
####################################################################################################
