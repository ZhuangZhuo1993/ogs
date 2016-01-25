/**
 * \file   BoostXmlGmlInterface.h
 * \author Karsten Rink
 * \date   2014-01-31
 * \brief  Definition of the BoostXmlGmlInterface class.
 *
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef BOOSTXMLGMLINTERFACE_H_
#define BOOSTXMLGMLINTERFACE_H_

#include <map>
#include <string>
#include <vector>


#include "BaseLib/ConfigTreeNew.h"
#include "../XMLInterface.h"

namespace GeoLib
{
    class GEOObjects;
    class Point;
    class Polyline;
    class Surface;
}


namespace FileIO
{

class BoostXmlGmlInterface : public XMLInterface
{
public:
	BoostXmlGmlInterface(GeoLib::GEOObjects& geo_objs);
	virtual ~BoostXmlGmlInterface() {}

	/// Reads an xml-file containing OGS geometry
	bool readFile(const std::string &fname);

protected:
	/// Required method for writing geometry. This is not implemented here, use the Qt class for writing.
	bool write();

private:
	/// Reads GeoLib::Point-objects from an xml-file
	void readPoints    ( BaseLib::ConfigTreeNew const& pointsRoot,
	                     std::vector<GeoLib::Point*>* points,
	                     std::map<std::string, std::size_t>* &pnt_names );

	/// Reads GeoLib::Polyline-objects from an xml-file
	void readPolylines ( BaseLib::ConfigTreeNew const& polylinesRoot,
	                   std::vector<GeoLib::Polyline*>* polylines,
	                   std::vector<GeoLib::Point*> const* points,
	                   const std::vector<std::size_t>& pnt_id_map,
	                   std::map<std::string, std::size_t>*& ply_names);

	/// Reads GeoLib::Surface-objects from an xml-file
	void readSurfaces  ( BaseLib::ConfigTreeNew const& surfacesRoot,
	                  std::vector<GeoLib::Surface*>* surfaces,
	                  std::vector<GeoLib::Point*> const* points,
	                  const std::vector<std::size_t>& pnt_id_map,
	                  std::map<std::string, std::size_t>*& sfc_names);

	void addSurfacesToPropertyTree(BaseLib::ConfigTreeNew::PTree & pt);
	void addPolylinesToPropertyTree(BaseLib::ConfigTreeNew::PTree & geometry_set);

	std::map<std::size_t, std::size_t> _idx_map;
	GeoLib::GEOObjects &_geo_objects;
};

} // end namespace FileIO

#endif /* BOOSTXMLGMLINTERFACE_H_ */
