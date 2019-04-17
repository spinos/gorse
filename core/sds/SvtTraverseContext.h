/*
 *  SvtTraverseContext.h
 *  aloe
 *
 *  T is node
 *  N is subdiv order
 *  Tr is traverse rule
 *  update ray and box
 *
 *  Created by zhang on 18-2-19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_SVT_TRAVERSE_CONTEXT_H
#define ALO_SDS_SVT_TRAVERSE_CONTEXT_H

namespace alo {

namespace sds {

/// ray-svt intersection
template<typename T, typename Tr>
class RaySvtContext {
	
    Tr m_rule;
/// (origin, direction, t_min, t_max)
	float m_ray[8];
/// (center, half_span)
	float m_coord[4];
	
public:

	RaySvtContext();
	
/// initialize
	void setRay(float* ray);
	
	bool intersect(const T& svt);
/// update    
    void getRay(float* ray) const;
    
protected:

    bool findParentCell(float* cellBox, const float* p,
                    const T& svt) const;
	
};

template<typename T, typename Tr>
RaySvtContext<T, Tr>::RaySvtContext()
{}

template<typename T, typename Tr>
void RaySvtContext<T, Tr>::setRay(float* ray)
{ memcpy(m_ray, ray, 32); }

template<typename T, typename Tr>
void RaySvtContext<T, Tr>::getRay(float* ray) const
{ memcpy(ray, m_ray, 32); }

//#define DEBG_RAY_L1

template<typename T, typename Tr>
bool RaySvtContext<T, Tr>::intersect(const T& svt)
{
    m_rule. template attach <T> (svt);
	memcpy(m_coord, svt.coord(), 16);

    if(!Tr::RayBoxIntersect(m_ray, m_coord) )
/// missed root box
        return false;
      
/// initiate from root box
    float parentBox[4];
    memcpy(parentBox, m_coord, 16);
    
    float rayInCell[8];
    memcpy(rayInCell, m_ray, 32);
       
    float cellBox[4];
    float rayHead[3];
/// advance ahead of entry point becuase float value loss 
    float adv = m_ray[6] + m_coord[3] * .001f;
    rayHead[0] = m_ray[0] + m_ray[3] * adv;
    rayHead[1] = m_ray[1] + m_ray[4] * adv;
    rayHead[2] = m_ray[2] + m_ray[5] * adv;

#ifdef DEBG_RAY    
    std::cout<<"\n entry at";
    Tr::PrintPnt(rayHead);
    std::cout.flush();
#endif

    int it = 0;
/// until exit root box or exceed max iteration
    for(;;) {
    
        if(it++>100) return false;
    
        if(Tr::IsRayExitingBox(rayHead, m_ray, parentBox) ) {
        
#ifdef DEBG_RAY_L1
            std::cout<<"\n ray exiting box";
            Tr::PrintRayRange(m_ray);
            Tr::PrintBox(parentBox);
            std::cout.flush();
#endif
            
/// coarser level
            adv = parentBox[3] * .001f;
            rayHead[0] += m_ray[3] * adv; 
            rayHead[1] += m_ray[4] * adv; 
            rayHead[2] += m_ray[5] * adv; 
            
            if(!findParentCell(parentBox, rayHead, svt) )
                return false;

            if(Tr::IsRayExitingBox(rayHead, m_ray, parentBox) )
/// exiting root box
                return false;
            
#ifdef DEBG_RAY_L1
            std::cout<<"\n coarser level after "<<it;
            Tr::PrintBox(parentBox);
            std::cout.flush();
#endif
        }
        
        const int k = m_rule.keyToCell(cellBox, rayHead, parentBox);
        const int j = svt.findNode(k);
        if(j > -1) {
            typename T::NodeType x = svt.nodes()[j];
            if(m_rule.intersectLeafCell(m_ray, cellBox, x) ) {
                return true;
            } else {
/// finer level
                memcpy(parentBox, cellBox, 16);
            
#ifdef DEBG_RAY_L2
            std::cout<<"\n finer level after "<<it;
            Tr::PrintBox(parentBox);
            std::cout.flush();
#endif
                continue;
            }
        }
        
/// empty cell 
/// advance to next cell 
        memcpy(rayInCell, rayHead, 12);
        rayInCell[6] = 0.f;
        rayInCell[7] = m_ray[7];
        if(Tr::RayBoxIntersect(rayInCell, cellBox) ) {
            adv = rayInCell[7] + cellBox[3] * .001f;
            
        } else {
/// edge/corner touching event
#ifdef DEBG_RAY
            std::cout<<"\n WARNING missed cell!";
            
            Tr::PrintPnt(rayHead);  
            Tr::PrintRayPnt(rayInCell);  
            Tr::PrintBoxRange(cellBox);
            Tr::PrintBoxRange(parentBox);
            std::cout.flush();
#endif            
            adv = cellBox[3] * .001f;
        }
        
        rayHead[0] += m_ray[3] * adv;
        rayHead[1] += m_ray[4] * adv;
        rayHead[2] += m_ray[5] * adv;
        m_ray[6] += adv;
        
#ifdef DEBG_RAY
        std::cout<<"\n it"<<it;
        Tr::PrintRayRange(m_ray);  
        Tr::PrintBox(cellBox);
        std::cout.flush();
#endif

    }
#ifdef DEBG_RAY
    std::cout<<"\n\n ray end after "<<it;
    std::cout.flush();
#endif
	return false;
}

template<typename T, typename Tr>
bool RaySvtContext<T, Tr>::findParentCell(float* cellBox, const float* p,
                            const T& svt) const
{
/// already in root box
    if(cellBox[3] > m_coord[3] / 2)
        return false;
        
    int parentLevel = 1;
    while(parentLevel > 0) {
        m_rule.getCoarserCell(cellBox, parentLevel, p);
        
        if(svt.findNode(m_rule.keyToPoint(cellBox) ) > -1) 
            break;
    }
    return true;
}

}

}
#endif
