/*
 * tube.h
 * 
 */
 
#ifndef TUBE_H
#define TUBE_H

#include <geom/AdaptableMesh.h>

class Tube : public alo::AdaptableMesh {
  
public:

    Tube();
    
    void createTube();
    
};

#endif
