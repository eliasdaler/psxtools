struct Vertex {
    s16 pos[3];
    u8 uv[2];
    u8 color[3];
};

struct Tri {
    Vertex vertices[3];
};


struct Quad {
    Vertex vertices[4];
};

struct Submesh {
    u16 tri_num;
    u16 quad_num;
    Tri tris[tri_num];
    Quad quad[quad_num];
};

struct Model {
    u16 submeshes_size;
    Submesh submeshes[submeshes_size];
};

Model model @ 0x0;
