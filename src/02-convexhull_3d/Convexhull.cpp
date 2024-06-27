#include "Convexhull.h"
#include "Core/GeoUtils.h"
#include "Core/Distance.h"
#include "Core/Inclusion.h"

#include <algorithm>
#include <iterator>
#include <list>

using namespace jmk;


static bool incident_face(Face* _face, Edge3d* _edge)
{
	auto r1 = std::find(_face->vertices.begin(), _face->vertices.end(), _edge->vertices[0]);
	auto r2 = std::find(_face->vertices.begin(), _face->vertices.end(), _edge->vertices[1]);
	if (r1 != std::end(_face->vertices) && r1 != std::end(_face->vertices))
		return true;
	return false;
}

static void adjust_normal(Face* _face, Point3d& _ref_point)
{
	// ref point is inside one. If it sees the orientation as counter clockwise, we need to adjust the face normal direction.
	// If the this is opposite what we need. 
	int order = FaceVisibility(*_face, _ref_point);
	if (order < 0 ){
		_face->normal_switch_needed = true;
	}
}

static void construct_initial_polyhedron(std::vector<Vertex3d*>& _points, int i,std::vector<Face*> &faces, std::vector<Edge3d*> &edges, 
	Point3d& ref_point)
{
	// Create the initial tetrahedron
	faces.push_back(new Face(_points[i + 0], _points[i + 1], _points[i + 2]));
	faces.push_back(new Face(_points[i + 0], _points[i + 1], _points[i + 3]));
	faces.push_back(new Face(_points[i + 1], _points[i + 2], _points[i + 3]));
	faces.push_back(new Face(_points[i + 2], _points[i + 0], _points[i + 3]));

	for (size_t i = 0; i < faces.size(); i++)
	{
		adjust_normal(faces[i], ref_point);
	}

	edges.push_back(new Edge3d(_points[i + 0], _points[i + 1]));
	edges.push_back(new Edge3d(_points[i + 1], _points[i + 2]));
	edges.push_back(new Edge3d(_points[i + 2], _points[i + 0]));
	edges.push_back(new Edge3d(_points[i + 0], _points[i + 3]));
	edges.push_back(new Edge3d(_points[i + 1], _points[i + 3]));
	edges.push_back(new Edge3d(_points[i + 2], _points[i + 3]));

	//Set the boundary edges for faces
	faces[0]->addEdge(edges[0]);
	faces[0]->addEdge(edges[1]);
	faces[0]->addEdge(edges[2]);

	faces[1]->addEdge(edges[0]);
	faces[1]->addEdge(edges[3]);
	faces[1]->addEdge(edges[4]);

	faces[2]->addEdge(edges[1]);
	faces[2]->addEdge(edges[4]);
	faces[2]->addEdge(edges[5]);

	faces[3]->addEdge(edges[2]);
	faces[3]->addEdge(edges[5]);
	faces[3]->addEdge(edges[3]);

	// set the incident faces for edges
	edges[0]->faces[0] = faces[0];
	edges[0]->faces[1] = faces[1];

	edges[1]->faces[0] = faces[0];
	edges[1]->faces[1] = faces[2];

	edges[2]->faces[0] = faces[0];
	edges[2]->faces[1] = faces[3];

	edges[3]->faces[0] = faces[1];
	edges[3]->faces[1] = faces[3];

	edges[4]->faces[0] = faces[2];
	edges[4]->faces[1] = faces[1];

	edges[5]->faces[0] = faces[3];
	edges[5]->faces[1] = faces[2];
}

void jmk::convexhull3D(std::vector<Point3d>& _points, std::vector<Face*>& faces)
{
	// Step 1 : Pick 4 points that do not lie in same plane. If we cannot find such points
	//			- then all the points as in one plane and we can use 2d convexhull algo to find the hull.
	//				1. Pick two points.(P1, P2)
	//				2. Pick thired point that do not in P1, P2 line. P3
	//				3. Pick fourth point that do not lie in P1, P2, P3 plane.

	std::vector<Vertex3d*> vertices;
	for (size_t i = 0; i < _points.size(); i++)
	{
		vertices.push_back(new Vertex3d(&_points[i]));
	}

	std::vector<Edge3d*> edges;

	size_t i = 0 , j =0 ;
	bool found_noncoplaner = false;
	for (  i = 0; i < _points.size() -3 ; i++ )
	{
		if (!coplaner(_points[i], _points[i + 1], _points[i + 2], _points[i + 3]))
		{
			found_noncoplaner = true;
			break;
		}
	}

	if (!found_noncoplaner)
	{
		std::cout << "All the points are coplaner" << std::endl;
		return;
	}

	// We need to find a point inside the 
	float x_mean = (_points[i][X] + _points[i + 1][X] + _points[i + 2][X] + _points[i + 3][X]) / 4;
	float y_mean = (_points[i][Y] + _points[i + 1][Y] + _points[i + 2][Y] + _points[i + 3][Y]) / 4;
	float z_mean = (_points[i][Z] + _points[i + 1][Z] + _points[i + 2][Z] + _points[i + 3][Z]) / 4;
	float x_p = x_mean ;
	float y_p = y_mean ;
	float z_p = z_mean ;

	Point3d point_ref(x_p, y_p, z_p);
	construct_initial_polyhedron(vertices, i, faces, edges, point_ref);
	
	//Points used to construct the p
	vertices[i]->processed = true;
	vertices[i+1]->processed = true;
	vertices[i+2]->processed = true;
	vertices[i+3]->processed = true;

	// Step 2 : Add next point Pr to the current convexhull
	//				1. Pr can be inside the current hull. Then there's nothing to be done.
	//				2. Pr lies outside the convexhull. In this case we need to compute new hull.

	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (vertices[i]->processed)
			continue;

		std::vector<Face*> visible_faces;
		std::vector<Edge3d*> border_edges;
		std::vector<Edge3d*> tobe_deleted_edges;

		// Point has not yet processed and it is outside the current hull.
		for (size_t j = 0; j < faces.size(); j++)
		{
			float volum = FaceVisibility(*faces[j], *vertices[i]->point);

			//if (order == CCW && volum < 0 || order == CW && volum > 0)
			if ((!faces[j]->normal_switch_needed && volum < 0)
				|| (faces[j]->normal_switch_needed && volum > 0))
			{
				faces[j]->visible = true;
				visible_faces.push_back(faces[j]);
			}
		}

		if (!visible_faces.size())
			continue;	// Point is inside

		for (size_t k = 0; k < visible_faces.size(); k++)
		{
			for (size_t j = 0; j < visible_faces[k]->edges.size(); j++)
			{
				if ( visible_faces[k]->edges[j]->faces[0]->visible && 
					visible_faces[k]->edges[j]->faces[1]->visible )
				{
					tobe_deleted_edges.push_back(visible_faces[k]->edges[j]);
				}
				else
				{
					//Atleast one edge in visible faces is visible
					border_edges.push_back(visible_faces[k]->edges[j]);
				}
			}
		}

		std::vector<Face*> new_faces;
		std::vector<Edge3d*> new_edges;

		const unsigned int new_size = border_edges.size();

		// We need to find the unique points in border edges.
		std::list<Vertex3d*> unque_vertices;
		for (size_t j = 0; j < new_size; j++)
		{	
			unque_vertices.push_back(border_edges[j]->vertices[0]);
			unque_vertices.push_back(border_edges[j]->vertices[1]);
		}

		// Due to below sorting we cannot rely on the created order for adding faces to the edge. So we need to explicitly check
		// for incident faces in that case.
		unque_vertices.sort();
		unque_vertices.unique( [](Vertex3d* a, Vertex3d* b){ return *(a->point) == *(b->point); } );
		std::list<Vertex3d*>::iterator it;

		for (size_t j = 0; j < new_size; j++)
		{
			it = unque_vertices.begin();
			std::advance(it, j);
			
			// New faces and edges for new convex polyhedron
			new_edges.push_back(new Edge3d(*it, vertices[i]));
			new_faces.push_back(new Face(border_edges[j]->vertices[0], vertices[i], border_edges[j]->vertices[1]));
		
			//Add new face referece to borader edges
			if (border_edges[j]->faces[0]->visible)
			{
				border_edges[j]->faces[0] = new_faces[new_faces.size() - 1];
			}
			else
			{
				border_edges[j]->faces[1] = new_faces[new_faces.size() - 1];
			}	
		}

		//Adjust the faces normals
		for (size_t j = 0; j < new_size; j++)
		{
			adjust_normal(new_faces[j], point_ref);
		}

		// Added faces for edges
		// Based on our assumptions we must have exactly two faces incident wiht each edge
		for (size_t j = 0; j < new_edges.size(); j++)
		{
			std::vector<Face*> incident_faces;

			for (size_t k = 0; k < new_faces.size(); k++)
			{
				if(incident_face(new_faces[k],new_edges[j]))
					incident_faces.push_back(new_faces[k]);
			}

			new_edges[j]->faces[0] = incident_faces[0];
			new_edges[j]->faces[1] = incident_faces[1];
		}

		// Added edges for faces
		for (size_t j = 0; j < new_size; j++)
		{
			new_faces[j]->addEdge(border_edges[j]);
			for (size_t k = 0; k < new_edges.size(); k++)
			{
				auto r1 = std::find(new_faces[j]->vertices.begin(), 
					new_faces[j]->vertices.end(), new_edges[k]->vertices[0]);
				auto r2 = std::find(new_faces[j]->vertices.begin(), 
					new_faces[j]->vertices.end(), new_edges[k]->vertices[1]);

				if (incident_face(new_faces[j], new_edges[k]))
					new_faces[j]->addEdge(new_edges[k]);
			}
		}

		// Deleted the edges from to be deleted list
		for (size_t k = 0; k< tobe_deleted_edges.size(); k++)
		{
			for (size_t j = 0; j < edges.size(); j++)
			{
				if (*tobe_deleted_edges[k] == *edges[j])
					edges.erase(edges.begin() + j);
			}
		}

		// Delete the visible faces. 
		for (size_t k = 0; k < visible_faces.size(); k++)
		{
			for (size_t j = 0; j < faces.size(); j++)
			{
				if( visible_faces[k] == faces[j])
					faces.erase(faces.begin()+j);
			}
		}

		faces.insert(faces.end(), new_faces.begin(), new_faces.end());
		edges.insert(edges.end(), new_edges.begin(), new_edges.end());
	}
}

void jmk::convexhull3DQuickhull(std::vector<Point3d>& _points, Polygon& _results)
{
}
