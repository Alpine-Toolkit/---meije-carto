####################################################################################################

import os
from ArithmeticInterval import IntervalInt2D

####################################################################################################

cache_directory = "/home/fabrice/.cache/QtCarto/geoportail/offline"

number_of_levels = 20
level_counter = [0]*number_of_levels
for level in range(0, number_of_levels):
    level_interval = None
    counter = 0
    level_directory = os.path.join(cache_directory, str(level))
    for tile_filename in os.listdir(level_directory):
        if tile_filename.startswith('geoportail'):
            map_id, _level, row, column = [int(x) for x in
                                           tile_filename[len('geoportail-'):-len('.jpg')].split('-')]
            tile_interval = IntervalInt2D([row, row], [column, column])
            if level_interval is None:
                level_interval = tile_interval
            else:
                level_interval |= tile_interval
            # print(map_id, level, row, column)
            if map_id == 0:
                counter += 1
    level_counter[level] = counter
    print(level, level_interval)

print(level_counter)
print(sum(level_counter))

####################################################################################################
#
# End
#
####################################################################################################
