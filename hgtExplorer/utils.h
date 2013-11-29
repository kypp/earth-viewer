#include <string>

float randFloat(float min=0.0, float max=1.0);

/* generateSurfaceOrder - generates vertex drawing order for triangle strip based surface */
/* order should be an array of size x/jump*(y-1)/jump*2	*/
void generateSurfaceOrder( int x, int y, unsigned *order, int jump=1);

template<class T>
std::string tostr(T data);

template std::string tostr<int>(int);