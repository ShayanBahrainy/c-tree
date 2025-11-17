struct Button {
    int x;
    int y;
    int xwidth;
    int ywidth;
    std::string name;

    void (*mousePressed) (Button& self);
};