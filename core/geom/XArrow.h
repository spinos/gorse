/* 
 * XArrow.h
 * aloe
 *
 * triangle array of x-pointing arrow of lenght 1
 * position|normal
 */

#ifndef X_ARROW_H
#define X_ARROW_H


namespace alo {

class XArrow {

    float *m_data;
    
public:
    XArrow();
    ~XArrow();
    
    const float *c_data() const;
    int vertexCount() const;
    int count() const;
    
private:
    
};

}

#endif