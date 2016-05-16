####################################################################################################

from struct import unpack

####################################################################################################

data = b'\x00\x01\xE6\x10\x00\x00\x9A\x99\x99\x99\x99\x99\xF1?\x9A\x99\x99\x99\x99\x99\x01@\x9A\x99\x99\x99\x99\x99\xF1?\x9A\x99\x99\x99\x99\x99\x01@|\x01\x00\x00\x00\x9A\x99\x99\x99\x99\x99\xF1?\x9A\x99\x99\x99\x99\x99\x01@\xFE'

if (data[0] == 0):
    print('START')

is_big_endian = data[1] == 0
if (is_big_endian):
    print('BIG_ENDIAN')
    endian_prefix = '>'
else:
    print('LITTLE_ENDIAN')
    endian_prefix = '<'

start, endianess, srid = unpack(endian_prefix + 'BBi', data[:6])

mbr_min_x, mbr_min_y, mbr_max_x, mbr_max_y = unpack(endian_prefix + 'dddd', data[6:38])

print(srid)
print(mbr_min_x, mbr_min_y, mbr_max_x, mbr_max_y)

if (data[38] == int('7c', 16)):
    print('MBR_END')

if (data[-1] == int('fe', 16)):
    print('END')

#     XY coordinates
#         1 = POINT
#         2 = LINESTRING
#         3 = POLYGON
#         4 = MULTIPOINT
#         5 = MULTILINESTRING
#         6 = MULTIPOLYGON
#         7 = GEOMETRYCOLLECTION
#
# Please note well: the above 2D [XY] WKB classes are supported indifferently by any SpatiaLite version.
# Using recent versions (v.2.4 or any subsequent) the following extended classes are supported as well:
#
#     XYZ coordinates
#         1001 = POINT Z
#         1002 = LINESTRING Z
#         1003 = POLYGON Z
#         1004 = MULTIPOINT Z
#         1005 = MULTILINESTRING Z
#         1006 = MULTIPOLYGON Z
#         1007 = GEOMETRYCOLLECTION Z
#     XYM coordinates
#         2001 = POINT M
#         2002 = LINESTRING M
#         2003 = POLYGON M
#         2004 = MULTIPOINT M
#         2005 = MULTILINESTRING M
#         2006 = MULTIPOLYGON M
#         2007 = GEOMETRYCOLLECTION M
#     XYZM coordinates
#         3001 = POINT ZM
#         3002 = LINESTRING ZM
#         3003 = POLYGON ZM
#         3004 = MULTIPOINT ZM
#         3005 = MULTILINESTRING ZM
#         3006 = MULTIPOLYGON ZM
#         3007 = GEOMETRYCOLLECTION ZM
#     compressed geometries
#         1000002 = COMPRESSED LINESTRING
#         1000003 = COMPRESSED POLYGON
#         1001002 = COMPRESSED LINESTRING Z
#         1001002 = COMPRESSED POLYGON Z
#         1002002 = COMPRESSED LINESTRING M
#         1002002 = COMPRESSED POLYGON M
#         1003002 = COMPRESSED LINESTRING ZM
#         1003002 = COMPRESSED POLYGON ZM

print("WKB class", int(data[39]))

print(unpack(endian_prefix + 'dd', data[43:43+2*8]))

####################################################################################################
#
# End
#
####################################################################################################
