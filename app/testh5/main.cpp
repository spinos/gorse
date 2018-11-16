#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5/HOocArray.h>
#include <math/Int3.h>

using namespace alo;
using namespace alo::ver1;

bool testWrite(const std::string &fileName)
{ 
    std::cout << "\n write to file " << fileName;
    H5IO hio;
    
    bool stat = hio.begin(fileName, HDocument::oCreate);
    if(!stat) return false;
    
    HBase w("/world");
    
    HBase b("/world/mesh1");
    b.addIntAttr(".nv", 3);
    int nv[3] = {1,2,3};
    b.writeIntAttr(".nv", nv);
    
    b.addFloatAttr(".bx", 4);
    float bx[4] = {-1.f,-1.5f,2.2f, 2.9f};
    b.writeFloatAttr(".bx", bx);
    
    b.addVLStringAttr(".set_name");
    b.writeVLStringAttr(".set_name", "uv_map_up");
    
    typedef HOocArray<hdata::TInt, 96, 96> I3Data;
    I3Data *tri = b.createDataStorage<I3Data>(".tri", true, stat);
    if(!stat) std::cout << "\n ERROR cannot create ooc storage .tri";
	
    int nf = 32 * 1000;
    Int3 *fv = new Int3[nf];
    for(int i=0;i<nf;++i) 
        fv[i].set(i*3, i*3 + 1, i*3 + 2);
    
    tri->writeColumns((char *)fv, 0, 1000);
    
    delete[] fv;
    
    b.close();
    
    w.close();
    
    hio.end();
    return true; 
}

bool testRead(const std::string &fileName)
{ 
    std::cout << "\n read from file " << fileName;
    H5IO hio;
    
    bool stat = hio.begin(fileName);
    if(!stat) return false;
    
    HBase b("/world/mesh1");
    
    int nv[3];
    if(b.hasNamedAttr(".nv"))
        b.readIntAttr(".nv", nv);
    std::cout<< "\n nv " << nv[0] << " " << nv[1] << " " << nv[2];
    
    float bx[4];
    if(b.hasNamedAttr(".bx"))
        b.readFloatAttr(".bx", bx);
    std::cout<< "\n bx " << bx[0] << " " << bx[1] << " " << bx[2] << " " << bx[3];
    
    std::string setName;
    if(b.hasNamedAttr(".set_name"))
        b.readVLStringAttr(".set_name", setName);
    std::cout<< "\n set_name " << setName;
    
    typedef HOocArray<hdata::TInt, 96, 96> I3Data;
    I3Data *tri = b.openDataStorage<I3Data>(".tri", stat);
    if(!stat) std::cout << "\n ERROR cannot open ooc storage .tri";
	
    int nf = 32 * 10;
    Int3 *fv = new Int3[nf];
    
    tri->readColumns((char *)fv, 0, 10);
    
    std::cout << "\n tri[0] ("<<fv[0].x<<","<<fv[0].y<<","<<fv[0].z<<")";
    std::cout << "\n tri[299] ("<<fv[99].x<<","<<fv[99].y<<","<<fv[99].z<<")";
    
    delete[] fv;
    
    b.close();
    
    hio.end();
    
    return true; 
}

int main(int argc, char *argv[])
{
    if(!testWrite("foo.h5")) return 1;
        
    if(!testRead("foo.h5")) return 1;
    
    std::cout<<"\n passed";
      
    return 0;
}
