#include "SECP256K1.h"
#include <string.h>

Secp256K1::Secp256K1() {
}

void Secp256K1::Init() {
    // Prime for the finite field
    Int P;
    P.SetBase16("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");

    // Set up field
    Int::SetupField(&P);

    // Generator point and order
    G.x.SetBase16("79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
    G.y.SetBase16("483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");
    G.z.SetInt32(1);
    order.SetBase16("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");

    Int::InitK1(&order);

    // Compute Generator table
    Point N(G);
    for (int i = 0; i < 32; i++) {
        GTable[i * 256] = N;
        N = DoubleDirect(N);
        for (int j = 1; j < 255; j++) {
            GTable[i * 256 + j] = N;
            N = AddDirect(N, GTable[i * 256]);
        }
        GTable[i * 256 + 255] = N; // Dummy point for check function
    }
}

Secp256K1::~Secp256K1() {
}

Point Secp256K1::AddDirect(Point &p1, Point &p2) {
    Int _s;
    Int _p;
    Int dy;
    Int dx;
    Point r;
    r.z.SetInt32(1);

    dy.ModSub(&p2.y, &p1.y);
    dx.ModSub(&p2.x, &p1.x);
    dx.ModInv();
    _s.ModMulK1(&dy, &dx); // s = (p2.y - p1.y) * inverse(p2.x - p1.x);

    _p.ModSquareK1(&_s); // _p = pow2(s)

    r.x.ModSub(&_p, &p1.x);
    r.x.ModSub(&p2.x); // rx = pow2(s) - p1.x - p2.x;

    r.y.ModSub(&p2.x, &r.x);
    r.y.ModMulK1(&_s);
    r.y.ModSub(&p2.y); // ry = - p2.y - s * (ret.x - p2.x);

    return r;
}

Point Secp256K1::Add2(Point &p1, Point &p2) {
    Int u;
    Int v;
    Int u1;
    Int v1;
    Int vs2;
    Int vs3;
    Int us2;
    Int a;
    Int us2w;
    Int vs2v2;
    Int vs3u2;
    Int _2vs2v2;
    Point r;

    u1.ModMulK1(&p2.y, &p1.z);
    v1.ModMulK1(&p2.x, &p1.z);
    u.ModSub(&u1, &p1.y);
    v.ModSub(&v1, &p1.x);
    us2.ModSquareK1(&u);
    vs2.ModSquareK1(&v);
    vs3.ModMulK1(&vs2, &v);
    us2w.ModMulK1(&us2, &p1.z);
    vs2v2.ModMulK1(&vs2, &p1.x);
    _2vs2v2.ModAdd(&vs2v2, &vs2v2);
    a.ModSub(&us2w, &vs3);
    a.ModSub(&_2vs2v2);

    r.x.ModMulK1(&v, &a);

    vs3u2.ModMulK1(&vs3, &p1.y);
    r.y.ModSub(&vs2v2, &a);
    r.y.ModMulK1(&r.y, &u);
    r.y.ModSub(&vs3u2);

    r.z.ModMulK1(&vs3, &p1.z);

    return r;
}

Point Secp256K1::Add(Point &p1, Point &p2) {
    Int u, v;
    Int u1, u2;
    Int v1, v2;
    Int vs2, vs3;
    Int us2, w;
    Int a, us2w;
    Int vs2v2, vs3u2;
    Int _2vs2v2, x3;
    Int vs3y1;
    Point r;

    // Compute intermediate values
    u1.ModMulK1(&p2.y, &p1.z);
    u2.ModMulK1(&p1.y, &p2.z);
   