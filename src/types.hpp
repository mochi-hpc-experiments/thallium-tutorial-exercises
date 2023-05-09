class vector3d {

    public:

    double x;
    double y;
    double z;

    vector3d(double a=0.0, double b=0.0, double c=0.0)
    : x(a), y(b), z(c) {}

    template<typename A>
    void serialize(A& ar) {
        ar & x;
        ar & y;
        ar & z;
    }

    vector3d operator+(const vector3d& other) const {
        return {x+other.x, y+other.y, z+other.z};
    }
};

/* (2) Add an "entry" class containing an std::string name
 * and a uint64_t phone number, an the appropriate serialize
 * function. Note that you may need to include
 * thallium/serialization/stl/string.hpp for string
 * serialization to work.
 */
