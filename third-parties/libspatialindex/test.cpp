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

#include <cstdio>

#include <iostream>

#include <spatialindex/capi/sidx_api.h>
#include <spatialindex/capi/sidx_impl.h>
#include <spatialindex/capi/sidx_config.h>

using namespace std;
using namespace SpatialIndex;

/**************************************************************************************************/

// function to create a new spacial index
Index *
create_index()
{
  // create a property set with default values.
  // see utility.cc for all defaults  http://libspatialindex.github.io/doxygen/Utility_8cc_source.html#l00031
  Tools::PropertySet * property_set = GetDefaults();
  Tools::Variant variant;

  // set index type to R*-Tree
  variant.m_varType = Tools::VT_ULONG;
  variant.m_val.ulVal = RT_RTree;
  property_set->setProperty("IndexType", variant);

  // Set index to store in memory (default is disk)
  variant.m_varType = Tools::VT_ULONG;
  variant.m_val.ulVal = RT_Memory;
  property_set->setProperty("IndexStorageType", variant);

  // initalise index
  Index * index = new Index(*property_set);
  delete property_set;

  // check index is ok
  if (!index->index().isIndexValid())
    throw "Failed to create valid index";
  else
    cout << "created index" << endl;

  return index;
}

/**************************************************************************************************/

// Add a point to index.
void
add_point(Index * index, double lat, double lon, int64_t id)
{
  // create array with lat/lon points
  double coordinates[] = {lat, lon};

  // shapes can also have an object associated with them but we'll leave that for the moment.
  uint8_t * data = 0; // nullptr
  uint32_t data_size = 0;

  // create shape
  SpatialIndex::IShape * shape = 0;
  shape = new SpatialIndex::Point(coordinates, 2);

  // insert into index along with the an object and an ID
  index->index().insertData(data_size, data, *shape,id);

  cout << "Point " << id << " inserted into index." << endl;

  delete shape;
}

/**************************************************************************************************/

std::vector<SpatialIndex::IData *> *
get_nearest(Index * index, double lat, double lon, double max_results)
{
  double coordinates[] = {lat, lon};

  // get a visitor object and a point from which to search
  ObjVisitor * visitor = new ObjVisitor;

  // make point from lat/lon with two dimentions
  SpatialIndex::Point * point = new SpatialIndex::Point(coordinates, 2);

  // get nearesr maxResults shapes form index
  index->index().nearestNeighborQuery(max_results, *point, *visitor);

  // get count of results
  int64_t number_of_results = visitor->GetResultCount();
  cout << "found " << number_of_results << " results." << endl;

  // get actual results
  std::vector<SpatialIndex::IData *> & results = visitor->GetResults();

  for (const auto * item : results) {
    id_type id = item->getIdentifier();
    cout << "ID " << id << endl;
  }

  // an empty vector that we will copt the results to
  vector<SpatialIndex::IData *> * results_copy = new vector<SpatialIndex::IData *>();

  // copy the Items into the newly allocated vector array we need to
  // make sure to clone the actual Item instead of just the pointers,
  // as the visitor will nuke them upon destroy
  for (auto * item : results)
    results_copy->push_back(dynamic_cast<SpatialIndex::IData *>(item->clone()));

  delete point;
  delete visitor;

  return results_copy;
}

/**************************************************************************************************/

void
write_to_kml(std::vector<SpatialIndex::IData *> * items)
{
  // create a kml file header called points.kml
  ofstream myfile;
  myfile.open("points.kml");

  myfile << fixed; // ensure it doesn't use scientific notation

  myfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  myfile << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"><Document>\n";

  // for each item
  for (const SpatialIndex::IData * item : (*items)) {
    // get ID of shape
    id_type id = item->getIdentifier();

    // get the generic shape object which we can cast to other types
    SpatialIndex::IShape * shape;
    item->getShape(&shape);

    // cast the shape to a Point
    SpatialIndex::Point center;
    shape->getCenter(center);

    // write to file as a placemarker for each point
    myfile << "<Placemark><name>Point id: " << id << "</name><Point><coordinates>";
    myfile << center.m_pCoords[1] << ", " << center.m_pCoords[0]; // lon, lat for reason
    myfile << "</coordinates></Point></Placemark>\n";
  }

  myfile << "</Document></kml>";

  myfile.close();

  cout << "KML file saved, try opening it in google earth." << endl;
}

/**************************************************************************************************/

int
main(int argc, char *argv[])
{
  char * psz_version = SIDX_Version();
  fprintf(stdout, "libspatialindex version: %s\n", psz_version);
  fflush(stdout);
  delete psz_version;

  // Initalise Indexpointer
  Index * index = create_index();

  // Add some points
  add_point(index, 51.501364, -0.141890, 1); // buckingham palace
  add_point(index, 51.505456, -0.075356, 2); // tower bridge
  add_point(index, 51.507273, -0.165739, 3); // hyde park

  // Get nearest two locations to the royal albert hall
  std::vector<SpatialIndex::IData *> * results = get_nearest(index, 51.5009157, -0.1773691, 2);

  // Results should now contain point 1 and 3, let's output it to a kml file
  write_to_kml(results);

  return 0;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
