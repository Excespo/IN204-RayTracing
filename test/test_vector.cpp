//
// Created by 18490 on 2024/3/18.
//

#include "../vector.h"

#include <iostream>
using std::cout;
using std::endl;

int main() {
    auto v1 = Vector3d();
    auto v2 = Vector3d(1., -4., 6.6);
    cout << "v1= " << v1 << "\nv2= " << v2 << endl;
    auto v3_1 = 3*v2, v3_2 = v2 * 3;
    cout << "3v2= " << v3_1 << "\nv2*3= " << v3_2 << endl;
    auto v4 = v1 + v2;
    cout << "v1+v2= " << v4 << endl;
    auto v5 = dot(v2, v3_2);
    auto v6 = cross(v2, v3_2);
    cout << "v2.3v2= " << v5 << "\nv2x3v2= " << v6 << endl;

    return 0;
}