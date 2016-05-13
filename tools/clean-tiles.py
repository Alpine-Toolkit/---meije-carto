####################################################################################################

import os

####################################################################################################

cache_directory = "/home/fabrice/.cache/QtCarto/geoportail/offline"

for level in range(6, 11):
    level_directory = os.path.join(cache_directory, str(level))
    for tile_filename in os.listdir(level_directory):
        absolut_tile_filename = os.path.join(level_directory, tile_filename)
        tile_stat = os.stat(absolut_tile_filename)
        tile_size = tile_stat.st_size
        if (tile_size < 1700): # 2 * 1024
            print(tile_filename, tile_stat.st_size)
            os.unlink(absolut_tile_filename)

####################################################################################################
#
# End
#
####################################################################################################
