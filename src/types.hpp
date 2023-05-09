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
