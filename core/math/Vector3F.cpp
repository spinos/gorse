#include "Vector3F.h"
#include "miscfuncs.h"

namespace alo {

Vector3F Vector3F::XAxis(1.f, 0.f, 0.f);
Vector3F Vector3F::YAxis(0.f, 1.f, 0.f);
Vector3F Vector3F::ZAxis(0.f, 0.f, 1.f);
Vector3F Vector3F::Zero(0.f, 0.f, 0.f);
Vector3F Vector3F::One(1.f, 1.f, 1.f);

Vector3F::Vector3F() 
{
	x = y = z = 0.f;
}

Vector3F::Vector3F(float v)
{ x = y = z = v; }

Vector3F::Vector3F(const float& vx, const float& vy, const float& vz) 
{
    x = vx;
    y = vy;
    z = vz;
}

Vector3F::Vector3F(const float* p) 
{
    x = p[0]; 
    y = p[1]; 
    z = p[2];
}

Vector3F::Vector3F(float* p) 
{
    x = p[0];
    y = p[1];
    z = p[2];
}

Vector3F::Vector3F(const Vector3F& from, const Vector3F& to) 
{
    x = to.x - from.x; 
    y = to.y - from.y; 
    z = to.z - from.z; 
}

void Vector3F::setZero()
{
	x = y = z = 0.f;
}

void Vector3F::set(float vx, float vy, float vz)
{
    x = vx;
    y = vy;
    z = vz;
}

void Vector3F::setComp(float v, int icomp)
{
	if(icomp < 1) x = v;
	else if(icomp < 2) y = v;
	else z = v;
}

char Vector3F::equals(const Vector3F &other ) const
{
	return (x == other.x && y == other.y && z == other.z);
}

char Vector3F::operator==( const Vector3F& other ) const
{
        return (x == other.x && y == other.y && z == other.z);
}

char Vector3F::operator!=( const Vector3F& other ) const
{
        return (x != other.x || y != other.y || z != other.z);
}
	
void Vector3F::operator+=( const Vector3F& other )
{
        x += other.x;
        y += other.y;
        z += other.z;
}
	
void Vector3F::operator-=( const Vector3F& other )
{
        x -= other.x;
        y -= other.y;
        z -= other.z;
}
	
void Vector3F::operator/=( const float& scale )
{
    x /= scale;
    y /= scale;
    z /= scale;
}

void Vector3F::operator*=( const float& scale )
{
    x *= scale;
    y *= scale;
    z *= scale;
}

void Vector3F::operator*=( const Vector3F & scale )
{
	x *= scale.x;
    y *= scale.y;
    z *= scale.z;
}

Vector3F Vector3F::operator*(const float& scale ) const
{
        return Vector3F(x*scale,y*scale,z*scale);
}

Vector3F Vector3F::operator/(const float& scale ) const
{
        return Vector3F(x/scale,y/scale,z/scale);
}

Vector3F Vector3F::operator*( const Vector3F& other ) const
{
        return Vector3F(x*other.x,y* other.y, z* other.z);
}

Vector3F Vector3F::operator+(Vector3F& other ) const
{
        return Vector3F(x+other.x, y+other.y, z+other.z);
}
	
Vector3F Vector3F::operator+(const Vector3F& other ) const
{
        return Vector3F(x+other.x, y+other.y, z+other.z);
}

Vector3F Vector3F::operator-(Vector3F& other ) const
{
        return Vector3F(x-other.x, y-other.y, z-other.z);
}

Vector3F Vector3F::operator-(const Vector3F& other ) const
{
        return Vector3F(x-other.x, y-other.y, z-other.z);
}
	
const float Vector3F::length() const
{
        return (float)sqrt(x*x + y*y + z*z);
}

const float Vector3F::length2() const
{
        return (x*x + y*y + z*z);
}
	
float Vector3F::dot(const Vector3F& other) const
{
        return ( x*other.x + y*other.y + z*other.z);
}

Vector3F Vector3F::cross(const Vector3F& other) const
{
        return Vector3F(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}
	
void Vector3F::normalize()
{
        float len = length();
        if(len > 10e-8)
        {
        x /= len;
        y /= len;
        z /= len;
        }
        else
        {
                x = y = z = 0.577350f;
        }
}

Vector3F Vector3F::normal() const
{
        double mag = sqrt( x * x + y * y + z * z );
        return Vector3F(x /(float)mag, y /(float)mag, z /(float)mag);
}
	
void Vector3F::reverse()
{
        x = -x;
        y = -y;
        z = -z;
}
	
Vector3F Vector3F::reversed() const
{
        return Vector3F(-x, -y, -z);
}

void Vector3F::rotateAroundAxis(const Vector3F& axis, float theta)
{
	if(theta==0) return;
	Vector3F ori(x,y,z);
	float l = ori.length();
	ori.normalize();
	
	Vector3F up = axis.cross(ori);
	up.normalize();
	
	Vector3F side = ori - axis*(axis.dot(ori));
	
	up *=side.length();
	
	ori += side*(cos(theta) - 1);
	ori += up*sin(theta);
	
	ori.normalize();
	x = ori.x*l;
	y = ori.y*l;
	z = ori.z*l;
}

Vector3F Vector3F::perpendicular() const
{
	Vector3F ref(0,-1,0);
	Vector3F n = normal();
	if(n.y < -0.9f || n.y > 0.9f) ref = Vector3F(-1,0,0);
	Vector3F per = cross(ref);
	per.normalize();
	return per;
}

float Vector3F::comp(int dim) const
{
	if(dim < 1) return x;
	if(dim < 2) return y;
	return z;
}

int Vector3F::longestAxis() const
{
/// find the higest absolute 
	float a = (x >= 0) ? x : -x;
	float b = (y >= 0) ? y : -y;
	float c = (z >= 0) ? z : -z;
	if(a >= b && a >= c) return 0;
	if(b >= c && b >= a) return 1;
	return 2;
}

float Vector3F::angleX() const
{
	float r = sqrt(y * y + z * z);
	if(r < 10e-5) return 0.f;
	if(y <= 0.f) return acos(z / r);
	return 6.283f - acos(z / r);
}
	
float Vector3F::angleY() const
{
	float r = sqrt(x * x + z * z);
	if(r < 10e-5) return 0.f;
	if(x > 0.f) return acos(z / r);
	return 6.283f - acos(z / r);
}

float Vector3F::angleBetween(const Vector3F& another, const Vector3F& up) const
{
	float ang = acos(this->normal().dot(another.normal()));
	if(another.dot(up) >= 0.f) return ang;
	return -ang;
}

float Vector3F::distanceTo(const Vector3F& another) const
{
	float d = distance2To(another);
	if(d < 10e-8) return 0.f;
	return sqrt(d);
}

float Vector3F::distance2To(const Vector3F& another) const
{
	return (x - another.x) * (x - another.x) + (y - another.y) * (y - another.y) + (z - another.z) * (z - another.z);
}

void Vector3F::resize(float l)
{
    normalize();
    x *= l;
    y *= l;
    z *= l;
}

Vector3F Vector3F::progress(const Vector3F& linearVelocity, const float & timeStep) const
{
    return *this + linearVelocity * timeStep;
}

void Vector3F::clamp(const float & limit)
{
	if(x > limit) x = limit;
	if(x < -limit) x = -limit;
	if(y > limit) y = limit;
	if(y < -limit) y = -limit;
	if(z > limit) z = limit;
	if(z < -limit) z = -limit;
}

Vector3F Vector3F::FromPolarXYZ(float phi, float theta, float zdir)
{
    Vector3F result;
    result.x = cos(theta) * cos(phi);
    result.y = sin(theta);
    result.z = zdir * cos(theta) * sin(phi);
    return result;
}

int Vector3F::orientation() const
{ 
	int j = longestAxis();
	if(j==0) {
		if(x<0.f) return 0;
		return 1;
	}
	if(j==1) {
		if(y<0.f) return 2;
		return 3;
	}
	if(z<0.f) return 4;
	return 5;
}

Vector3F Vector3F::inOrientation(const float & l) const
{
	int j = orientation();
	if(j==0) return Vector3F(-l, 0.f, 0.f);
	if(j==1) return Vector3F( l, 0.f, 0.f);
	if(j==2) return Vector3F(0.f, -l, 0.f);
	if(j==3) return Vector3F(0.f,  l, 0.f);
	if(j==4) return Vector3F(0.f, 0.f, -l);
	return Vector3F(0.f, 0.f, l);
}

std::ostream& operator<<(std::ostream &output, const Vector3F & p) 
{
    output <<"("<<p.x<<", "<<p.y<<", "<<p.z<<")";
    return output;
}

void Vector3F::inverse()
{
	if(Absolute<float>(x) > 1e-4f) {
		x = 1.f/ x;
	}
	if(Absolute<float>(y) > 1e-4f) {
		y = 1.f/ y;
	}
	if(Absolute<float>(z) > 1e-4f) {
		z = 1.f/ z;
	}
}

Vector3F Vector3F::inversed() const
{
	Vector3F r = *this;
	r.inverse();
	return r;
}

void Vector3F::getColor(float * dst) const
{ memcpy(dst, this, 12); }

void Vector3F::addTo(float* c, const float& b) const
{
	c[0] += x * b;
	c[1] += y * b;
	c[2] += z * b;
}

void Vector3F::extract(char* c) const
{ memcpy(c, this, 12); }

void Vector3F::saturate()
{
    if(x < 0.f) x = 0.f;
    if(x > 1.f) x = 1.f;
    if(y < 0.f) y = 0.f;
    if(y > 1.f) y = 1.f;
    if(z < 0.f) z = 0.f;
    if(z > 1.f) z = 1.f;
}
    
}