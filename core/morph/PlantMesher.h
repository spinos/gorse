/* 
 *  PlantMesher.h
 *  gorse
 *
 *  2019/8/19
 */
 
#ifndef ALO_MORPH_PLANT_MESHER_H
#define ALO_MORPH_PLANT_MESHER_H

namespace alo {

class AdaptableMesh;

namespace morph {
    
class Plant;
class PlantProfile;

class PlantMesher {

    AdaptableMesh *m_mesh;
    
public:

	PlantMesher();
	virtual ~PlantMesher();

	void attach(AdaptableMesh *mesh);
    void triangulate(const Plant &pl, PlantProfile &prof);
    void detach();
    
protected:

    AdaptableMesh *mesh();

private:

};

}

}

#endif
